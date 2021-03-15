#include "xmod.h"

void sigint_handler(int signumber)
{
  fprintf(stderr, "\nReceived signal %d!\n", signumber);

  char answer;
  int cicle = 0;
  printf("Should the program terminate? (y/n)\n");
  while (cicle == 0)
  {
    answer = getchar();
    if (answer == 'y')
    {
      printf("Terminated\n");
      cicle = 1;
      exit(0);
    }
    else if (answer == 'n')
    {
      printf("Execution resumed\n");
      cicle = 1;
      return;
    }
  }
}

FILE *f_ptr;
clock_t start, stop;

void print_int(double instant, pid_t pid, char event[], int info) {
    fprintf(f_ptr, "%f – %d – %s – %i\n", fabs(instant), pid, event, info);
}

void print_str(double instant, pid_t pid, char event[], char info[]) {
    fprintf(f_ptr, "%f ; %d ; %s ; %s\n", fabs(instant), pid, event, info);
}

void end_sig_print(pid_t pid, char file_dir[], int nftot, int nfmod) {
    printf("%d ; %s ; %i ; %i\n", pid, file_dir, nftot, nfmod);
}

int make_command_from_text_mode(char *mode, unsigned int *command) {
    user_type user_t;
    permission_mode perm_mode;
    permission_type perm_type = {false, false, false};

    // MODE: user type
    switch (mode[0]) {
        case 'u':
            user_t = OWNER;
            break;

        case 'g':
            user_t = GROUP;
            break;

        case 'o':
            user_t = OTHER;
            break;

        case 'a':
            user_t = ALL;
            break;

        default:
            printf("ERROR: Invalid user type. ");
            printf("Should be one of <u|g|o|a>.\n");
            return -1;
            break;
    }

    // MODE: permission mode
    switch (mode[1]) {
        case '-':
            perm_mode = REMOVE;
            break;

        case '+':
            perm_mode = ADD;
            break;

        case '=':
            perm_mode = MODIFY;
            break;

        default:
            printf("ERROR: Invalid permission mode. ");
            printf("Should be one of <-|+|=>.\n");
            return -1;
            break;
    }

    // MODE: permission type
    for (int i = 2; i < strlen(mode); i++) {
        switch (mode[i]) {
            case 'r':
                perm_type.read = true;
                break;

            case 'w':
                perm_type.write = true;
                break;

            case 'x':
                perm_type.execute = true;
                break;

            default:
                printf("ERROR: Invalid permission type. ");
                printf("Should be at least one of <rwx>.\n");
                return -1;
                break;
        }
    }

    switch (user_t) {
        case OWNER:
            // remove current permissions for OWNER
            if (perm_mode == MODIFY)
                *command &= 0077;

            if (perm_type.read) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IRUSR;
                else
                    *command |= S_IRUSR;
            }
            if (perm_type.write) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IWUSR;
                else
                    *command |= S_IWUSR;
            }
            if (perm_type.execute) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IXUSR;
                else
                    *command |= S_IXUSR;
            }

            break;

        case GROUP:
            // remove current permissions for GROUP
            if (perm_mode == MODIFY)
                *command &= 0707;

            if (perm_type.read) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IRGRP;
                else
                    *command |= S_IRGRP;
            }
            if (perm_type.write) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IWGRP;
                else
                    *command |= S_IWGRP;
            }
            if (perm_type.execute) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IXGRP;
                else
                    *command |= S_IXGRP;
            }

            break;

        case OTHER:
            // remove current permissions for OTHER
            if (perm_mode == MODIFY)
                *command &= 0770;

            if (perm_type.read) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IROTH;
                else
                    *command |= S_IROTH;
            }
            if (perm_type.write) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IWOTH;
                else
                    *command |= S_IWOTH;
            }
            if (perm_type.execute) {
                if (perm_mode == REMOVE)
                    *command &= ~S_IXOTH;
                else
                    *command |= S_IXOTH;
            }

            break;

        case ALL:
            // remove current permissions for ALL
            if (perm_mode == MODIFY)
                *command &= 0;

            if (perm_type.read) {
                if (perm_mode == REMOVE)
                    *command &= ~(S_IRUSR | S_IRGRP | S_IROTH);
                else
                    *command |= S_IRUSR | S_IRGRP | S_IROTH;
            }
            if (perm_type.write) {
                if (perm_mode == REMOVE)
                    *command &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
                else
                    *command |= S_IWUSR | S_IWGRP | S_IWOTH;
            }
            if (perm_type.execute) {
                if (perm_mode == REMOVE)
                    *command &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
                else
                    *command |= S_IXUSR | S_IXGRP | S_IXOTH;
            }

            break;

        default:
            break;
    }

    return 0;
}

int make_command_from_octal_mode(char *mode, unsigned int *command) {
    if (strlen(mode) != 4 || mode[0] != '0' ||
        mode[1] < '0' || mode[1] > '7' ||
        mode[2] < '0' || mode[2] > '7' ||
        mode[3] < '0' || mode[3] > '7') {
        printf("ERROR: Invalid octal number. ");
        printf("Should be a 4-digit number starting with 0.\n");
        return -1;
    }

    *command = strtol(mode, &mode, 8);

    return 0;
}

int main(int argc, char **argv, char **envp) {
    start = clock() / CLOCKS_PER_SEC;

    if ((f_ptr = fopen(getenv("LOG_FILENAME"), "a")) == NULL) {
        printf("Error on opening register file. Set LOG_FILENAME.\n");
        stop = clock() / CLOCKS_PER_SEC;
        double elapsed_time = (double)(stop - start);
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "EXIT", 1);
        exit(1);
    }

    stop = clock() / CLOCKS_PER_SEC;
    double elapsed_time = (double)(stop - start);
    pid_t pid = getpid();
    print_int(elapsed_time, pid, "EXIT", 0);
    fclose(f_ptr);

    if (argc < 3) {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\n");
        printf("xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return -1;
    }

    //SIGNAL 
    signal(SIGINT, sigint_handler);
    struct sigaction new, old;
    sigset_t smask;                // defines signals to block while func() is running            // prepare struct sigaction
    if (sigemptyset(&smask) == -1) // block no signal
      perror("sigsetfunctions");

    new.sa_handler = sigint_handler;
    new.sa_mask = smask;
    new.sa_flags = 0; // usually works            if(sigaction(SIGUSR1, &new, &old) == -1)
    if (sigaction(SIGUSR1, &new, &old) == -1)
      perror("sigaction");
    

    size_t mode_idx = 1;

    while (argv[mode_idx][0] == '-') {
        /* fazer aqui o parsing das options
         switch(argv[mode_idx][1]) {
             case v:
             ...
             case c:
             ...
             case R:
             ...
         }*/
        mode_idx++;
    }

    if (strlen(argv[mode_idx]) < 3 || strlen(argv[mode_idx]) > 5) {
        printf("ERROR: Invalid MODE format. ");
        printf("Should be <u|g|o|a><-|+|=><rwx> or <-|+|=><0-7><0-7><0-7>\n");
        return -1;
    }

    struct stat buffer;
    if (stat(argv[mode_idx + 1], &buffer) != 0) {
        perror("ERROR");
        return -1;
    }

    // current permissions
    unsigned int curr_perm = buffer.st_mode & 0777;

    unsigned int command = curr_perm;

    if (isdigit(argv[mode_idx][0])) {
        if (make_command_from_octal_mode(argv[mode_idx], &command) != 0) {
            return -1;
        }
    } else {
        if (make_command_from_text_mode(argv[mode_idx], &command) != 0) {
            return -1;
        }
    }

    if (chmod(argv[mode_idx + 1], command) != 0) {
        perror("chmod() error");
        return -1;
    }

    return 0;
}
