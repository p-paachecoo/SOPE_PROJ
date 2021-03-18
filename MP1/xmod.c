#include "xmod.h"

sig_info info;
FILE *f_ptr;
clock_t start, stop;
options op;

void sigint_handler(int signumber) {
    fprintf(stderr, "\nReceived signal %d!\n", signumber);

    printf("%10s | %10s | %10s | %10s\n", "pid", "fich/dir", "nftot", "nfmod");
    printf("%10d | %10s | %10d | %10d\n", getpid(), info.originalFileDir,
           info.totalFiles, info.totalMod);

    char answer;
    int cicle = 0;
    printf("Should the program terminate? (y/n)\n");
    while (cicle == 0) {
        answer = getchar();
        if (answer == 'y') {
            printf("Terminated\n");
            cicle = 1;
            exit(0);
        } else if (answer == 'n') {
            printf("Execution resumed\n");
            cicle = 1;
            return;
        }
    }
}

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

void octal_to_text(unsigned int oct, char *text) {
    for (size_t i = 0; i < 9; i += 3) {
        switch ((oct >> (6 - i)) & 07) {
            case 00:
                text[i] = '-';
                text[i + 1] = '-';
                text[i + 2] = '-';
                break;

            case 01:
                text[i] = '-';
                text[i + 1] = '-';
                text[i + 2] = 'x';
                break;

            case 02:
                text[i] = '-';
                text[i + 1] = 'w';
                text[i + 2] = '-';
                break;

            case 03:
                text[i] = '-';
                text[i + 1] = 'w';
                text[i + 2] = 'x';
                break;

            case 04:
                text[i] = 'r';
                text[i + 1] = '-';
                text[i + 2] = '-';
                break;

            case 05:
                text[i] = 'r';
                text[i + 1] = '-';
                text[i + 2] = 'x';
                break;

            case 06:
                text[i] = 'r';
                text[i + 1] = 'w';
                text[i + 2] = '-';
                break;

            case 07:
                text[i] = 'r';
                text[i + 1] = 'w';
                text[i + 2] = 'x';
                break;

            default:
                break;
        }
    }
}

int isDirectory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int changePermissionsOfFileDir(char *fileDir, char *permissions) {
    if (op.R) {
        int isDir = isDirectory(fileDir);
        if (!isDir)
            perror("Error: Used -R option on a file instead of a Directory");
        // Fork and changePermissionsOfWholeDir of children
        fork();
        // children -> changePermissionsOfWholeDir(fileDir)
    }
    // parent ->
    if (changePermissionsOfFile(fileDir, permissions))
        return 1;

    if (op.R) {
        int forkStatus;
        wait(&forkStatus);                         // Reunite forks
        printf("Forks rejoined: %d", forkStatus);  // Check if all good
    }

    return 0;
}

void changePermissionsOfWholeDir(char *Dir, char *permissions) {
    struct dirent *dp;
    DIR *dirpath = opendir(Dir);
    while ((dp = readdir(dirpath)) != NULL) {  // Go trough whole Dir
        if (isDirectory(dp->d_name)) {
            // if a Dir is found, recursively fork and call iself
        }
        // parent only
        // changePermissionsOfFile(Path Until Here + dp->d_name, permissions);
        // Path until here probably needs to be passed by parameter
    }
    (void)closedir(dirpath);

    // parent -> change permissions of current dir
    // parent -> wait() for child and check if all good before exiting

    exit(0);
}

int changePermissionsOfFile(char *file, char *permissions) {
    struct stat buffer;
    if (stat(file, &buffer) != 0) {
        printf("xmod: cannot access '%s': No such file or directory\n", file);
        return -1;
    }

    // current permissions
    unsigned int curr_perm = buffer.st_mode & 0777;

    unsigned int command = curr_perm;

    if (isdigit(permissions[0])) {
        if (make_command_from_octal_mode(permissions, &command) != 0) {
            return -1;
        }
    } else {
        if (make_command_from_text_mode(permissions, &command) != 0) {
            return -1;
        }
    }

    if (chmod(file, command) != 0) {
        perror("chmod() error");
        if (op.v)
            optionV_print_failure(file, curr_perm, command);
        else if (op.c)
            optionC_print_failure(file);
        return -1;
    } else {
        if (op.v || op.c)
            optionV_C_print_success(file, curr_perm, command);
    }

    return 0;
}

void optionV_C_print_success(char *filename, unsigned int octalModePrevious,
                             unsigned int octalModeAfter) {
    char rwxModePrevious[10], rwxModeAfter[10];
    rwxModePrevious[9] = '\0';
    rwxModeAfter[9] = '\0';
    octal_to_text(octalModePrevious, rwxModePrevious);
    octal_to_text(octalModeAfter, rwxModeAfter);
    printf("mode of '%s' changed from %04o (%s) to %04o (%s)\n", filename,
           octalModePrevious, rwxModePrevious, octalModeAfter, rwxModeAfter);
}

void optionC_print_failure(char *filename) {
    printf("xmod: cannot access '%s': No such file or directory\n", filename);
}

void optionV_print_failure(char *filename, unsigned int octalModePrevious,
                           unsigned int octalModeAfter) {
    char rwxModePrevious[10], rwxModeAfter[10];
    rwxModePrevious[9] = '\0';
    rwxModeAfter[9] = '\0';
    octal_to_text(octalModePrevious, rwxModePrevious);
    octal_to_text(octalModeAfter, rwxModeAfter);
    printf("xmod: cannot access '%s': No such file or directory\n", filename);
    printf("failed to change mode of '%s' from %o (%s) to %o (%s)\n",
           filename, octalModePrevious, rwxModePrevious, octalModeAfter, rwxModeAfter);
}

int main(int argc, char **argv, char **envp) {
    start = clock();

    if ((f_ptr = fopen(getenv("LOG_FILENAME"), "a")) == NULL) {
        printf("Error on opening register file. Set LOG_FILENAME.\n");
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "EXIT", 1);
        exit(1);
    }

    if (argc < 3) {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\n");
        printf("xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "EXIT", 1);
        return -1;
    }

    // SIGNAL
    signal(SIGINT, sigint_handler);
    struct sigaction new, old;
    sigset_t smask;  // defines signals to block while func() is running
                     // prepare struct sigaction

    if (sigemptyset(&smask) == -1)  // block no signal
        perror("sigsetfunctions");

    new.sa_handler = sigint_handler;
    new.sa_mask = smask;
    new.sa_flags = 0;  // usually works

    if (sigaction(SIGUSR1, &new, &old) == -1)
        perror("sigaction");

    size_t mode_idx = 1;

    while (argv[mode_idx][0] == '-') {
        switch (argv[mode_idx][1]) {
            case 'v':
                op.v = true;
                break;
            case 'c':
                op.c = true;
                break;
            case 'R':
                op.R = true;
                break;
        }
        mode_idx++;
    }

    if (strlen(argv[mode_idx]) < 3 || strlen(argv[mode_idx]) > 5) {
        printf("ERROR: Invalid MODE format. ");
        printf("Should be <u|g|o|a><-|+|=><rwx> or <0><0-7><0-7><0-7>\n");

        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "EXIT", 1);
        return -1;
    }

    if (changePermissionsOfFileDir(argv[mode_idx + 1], argv[mode_idx]))
        perror("Error changing permissions of file/dir");

    stop = clock();
    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    pid_t pid = getpid();
    print_int(elapsed_time, pid, "EXIT", 0);
    fclose(f_ptr);

    return 0;
}
