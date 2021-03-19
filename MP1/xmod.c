#include "xmod.h"

sig_info info;
FILE *f_ptr;
clock_t start, stop;
options op;
bool fileopen = false;
int fileNamepos;
int argvSize;
char **envpGlobal;
extern int errno;

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    snprintf(result, strlen(s1) + strlen(s2) + 1, "%s%s", s1, s2);
    return result;
}

int isOriginalProcess(int *pidno, int *size)
{
    char pidline[1024];
    char *saveptr;
    char *pid2;
    int i = 0;

    FILE *fp = popen("pidof xmod", "r");
    fgets(pidline, 1024, fp);

    pid2 = strtok_r(pidline, " ", &saveptr);
    while (pid2 != NULL)
    {

        pidno[i] = atoi(pid2);
        //printf("%d\n", pidno[i]);
        pid2 = strtok_r(NULL, " ", &saveptr);
        i++;
    }
    *size = i;
    pclose(fp);
    int minPid = pidno[0];
    for (int j = 1; j < i; j++)
    {
        if (pidno[j] < minPid)
            minPid = pidno[j];
    }
    if (getpid() == minPid)
    {
        return 1; //is original
    }
    return 0;
}

void sigint_handler(int signumber)
{
    stop = clock();
    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    pid_t pid = getpid();
    char sig[10];
    snprintf(sig, sizeof(sig), "%i", signumber);
    end_sig_print(elapsed_time, pid, "SIGNAL_RECV", sig);

    int pidno[64];
    int size;
    int original = isOriginalProcess(pidno, &size);
    if (original)
        printf("%25s | %25s | %25s | %25s\n", "PID", "FILE/DIR",
               "FILES SEARCHED", "FILES MODDED");
    else
        usleep(300000);
    printf("%25d | %25s | %25d | %25d\n", getpid(), info.originalFileDir,
           info.totalFiles, info.totalMod);

    if (original)
    {
        usleep(500000);
        char answer;
        int cycle = 0;
        printf("Should the program terminate? (y/n)\n");
        while (cycle == 0)
        {
            answer = getchar();
            if (answer == 'y')
            {
                printf("Terminated\n");
                cycle = 1;
                for (int i = 0; i < size; i++)
                {
                    stop = clock();
                    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                    char sig[30];
                    char* signame;
                    switch(signumber){
                        case 1:
                            signame = "SIGHUP";
                        case 2:
                            signame = "SIGINT";
                        case 3:
                            signame = "SIGQUIT";
                        case 4:
                            signame = "SIGILL";
                        case 5:
                            signame = "SIGTRAP";
                        case 6:
                            signame = "SIGABRT";
                        case 9:
                            signame = "SIGKILL";
                        case 17:
                            signame = "SIGCHLD";
                        case 18:
                            signame = "SIGCONT";
                        case 19:
                            signame = "SIGSTOP";
                    }
                    snprintf(sig, sizeof(sig), "%s : %i", signame, pidno[i]);
                    end_sig_print(elapsed_time, pid, "SIGNAL_SENT", sig);
                    kill(pidno[i], 9);
                }
                stop = clock();
                double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                pid_t pid = getpid();
                print_int(elapsed_time, pid, "PROC_EXIT", 0);
                exit(0);
            }
            else if (answer == 'n')
            {
                printf("Execution resumed\n");
                for (int i = 0; i < size; i++)
                {
                    stop = clock();
                    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                    char sig[30];
                    char* signame;
                    switch(signumber){
                        case 1:
                            signame = "SIGHUP";
                        case 2:
                            signame = "SIGINT";
                        case 3:
                            signame = "SIGQUIT";
                        case 4:
                            signame = "SIGILL";
                        case 5:
                            signame = "SIGTRAP";
                        case 6:
                            signame = "SIGABRT";
                        case 9:
                            signame = "SIGKILL";
                        case 17:
                            signame = "SIGCHLD";
                        case 18:
                            signame = "SIGCONT";
                        case 19:
                            signame = "SIGSTOP";
                    }
                    snprintf(sig, sizeof(sig), "%s : %d", signame, pidno[i]);
                    end_sig_print(elapsed_time, pid, "SIGNAL_SENT", sig);
                    kill(pidno[i], 18);
                }
                cycle = 1;
                return;
            }
        }
    }
    else
    {
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        char sig[30];
        char* signame;
        switch(signumber){
            case 1:
                signame = "SIGHUP";
            case 2:
                signame = "SIGINT";
            case 3:
                signame = "SIGQUIT";
            case 4:
                signame = "SIGILL";
            case 5:
                signame = "SIGTRAP";
            case 6:
                signame = "SIGABRT";
            case 9:
                signame = "SIGKILL";
            case 17:
                signame = "SIGCHLD";
            case 18:
                signame = "SIGCONT";
            case 19:
                signame = "SIGSTOP";
        }
        snprintf(sig, sizeof(sig), "%s : %d", signame, pid);
        end_sig_print(elapsed_time, pid, "SIGNAL_SENT", sig);
        kill(getpid(), 19);
    }
}

void sigchild_handler(int signumber)
{
    int pidno[64];
    int size;
    if (isOriginalProcess(pidno, &size))
    {

        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        char sig[10];
        snprintf(sig, sizeof(sig), "%i", signumber);
        end_sig_print(elapsed_time, pid, "SIGNAL_RECV", sig);
    }
}

void print_int(double instant, pid_t pid, char event[], int info)
{
    if (fileopen == true)
        fprintf(f_ptr, "%f ; %d ; %s ; %i\n", fabs(instant), pid, event, info);
}

void print_str(double instant, pid_t pid, char event[], char info[])
{
    if (fileopen == true)
        fprintf(f_ptr, "%f ; %d ; %s ; %s\n", fabs(instant), pid, event, info);
}

void end_sig_print(double instant, pid_t pid, char event[], char info[])
{
    if (fileopen == true)
        fprintf(f_ptr, "%f ; %i ; %s ; %s\n", instant, pid, event, info);
}

int make_command_from_text_mode(char *mode, unsigned int *command)
{
    user_type user_t;
    permission_mode perm_mode;
    permission_type perm_type = {false, false, false};

    // MODE: user type
    switch (mode[0])
    {
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
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "ERROR", 1);
        return -1;
        break;
    }

    // MODE: permission mode
    switch (mode[1])
    {
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
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "ERROR", 1);
        return -1;
        break;
    }

    // MODE: permission type
    for (int i = 2; i < strlen(mode); i++)
    {
        switch (mode[i])
        {
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
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
            return -1;
            break;
        }
    }

    switch (user_t)
    {
    case OWNER:
        // remove current permissions for OWNER
        if (perm_mode == MODIFY)
            *command &= 0077;

        if (perm_type.read)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IRUSR;
            else
                *command |= S_IRUSR;
        }
        if (perm_type.write)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IWUSR;
            else
                *command |= S_IWUSR;
        }
        if (perm_type.execute)
        {
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

        if (perm_type.read)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IRGRP;
            else
                *command |= S_IRGRP;
        }
        if (perm_type.write)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IWGRP;
            else
                *command |= S_IWGRP;
        }
        if (perm_type.execute)
        {
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

        if (perm_type.read)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IROTH;
            else
                *command |= S_IROTH;
        }
        if (perm_type.write)
        {
            if (perm_mode == REMOVE)
                *command &= ~S_IWOTH;
            else
                *command |= S_IWOTH;
        }
        if (perm_type.execute)
        {
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

        if (perm_type.read)
        {
            if (perm_mode == REMOVE)
                *command &= ~(S_IRUSR | S_IRGRP | S_IROTH);
            else
                *command |= S_IRUSR | S_IRGRP | S_IROTH;
        }
        if (perm_type.write)
        {
            if (perm_mode == REMOVE)
                *command &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
            else
                *command |= S_IWUSR | S_IWGRP | S_IWOTH;
        }
        if (perm_type.execute)
        {
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

int make_command_from_octal_mode(char *mode, unsigned int *command)
{
    if (strlen(mode) != 4 || mode[0] != '0' ||
        mode[1] < '0' || mode[1] > '7' ||
        mode[2] < '0' || mode[2] > '7' ||
        mode[3] < '0' || mode[3] > '7')
    {
        printf("ERROR: Invalid octal number. ");
        printf("Should be a 4-digit number starting with 0.\n");
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "ERROR", 1);
        return -1;
    }

    *command = strtol(mode, &mode, 8);

    return 0;
}

void octal_to_text(unsigned int oct, char *text)
{
    for (size_t i = 0; i < 9; i += 3)
    {
        switch ((oct >> (6 - i)) & 07)
        {
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

int isDirectory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}

int changePermissionsOfFileDir(char *fileDir, char *permissions, char **argv)
{
    int pidno[64];
    int size;
    int original = isOriginalProcess(pidno, &size);
    int isDir = 0;
    if (op.R)
    {
        isDir = isDirectory(fileDir);
        if (isDir)
        { // Fork and changePermissionsOfWholeDir of children

            if (fork() == 0)
            { // children -> changePermissionsOfWholeDir(fileDir)

                stop = clock();
                char *inf = malloc(strlen(*argv) + 1);
                snprintf(inf, strlen(*argv) + 1, "%s", *argv);
                double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                pid_t pid = getpid();
                print_str(elapsed_time, pid, "PROC_CREAT", inf);

                changePermissionsOfWholeDir(fileDir, argv, permissions);
            }
        }
    }
    if (original)
    {
        // parent ->
        if (changePermissionsOfFile(fileDir, permissions))
        {
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
            return -1;
        }
    }
    if (op.R)
    {
        if (isDir)
        {
            int forkStatus;
            while (wait(&forkStatus) > 0)
                ; // this way, the father waits for all the child processes
        }
    }

    return 0;
}

void changePermissionsOfWholeDir(char *Dir, char **argv, char *permissions)
{
    struct dirent *dp;
    DIR *dirpath = opendir(Dir);
    int contentInDir = 0;

    while ((dp = readdir(dirpath)) != NULL)
    { // Go trough whole Dir recursively fork and call iself with exec with new path (Dir+dp->d_name)

        if ((dp->d_name != NULL) && (strcmp(dp->d_name, "..") != 0) && (strcmp(dp->d_name, ".") != 0))
        {

            contentInDir = 1;
            char *newPathTemp = concat(Dir, "/");
            char *newPath = concat(newPathTemp, dp->d_name);
            char *newArgv[argvSize];

            for (int i = 0; i < argvSize; i++)
            {
                if (i == fileNamepos)
                    newArgv[i] = newPath;
                else
                {
                    newArgv[i] = argv[i];
                }
            }
            if (changePermissionsOfFile(newPath, permissions))
            {
                stop = clock();
                double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                pid_t pid = getpid();
                print_int(elapsed_time, pid, "ERROR", 1);
            }

            if (isDirectory(newPath))
            {
                if (fork() == 0)
                {
                    stop = clock();
                    char *inf = malloc(strlen(*argv) + 1);
                    snprintf(inf, strlen(*argv) + 1, "%s", *argv);
                    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                    pid_t pid = getpid();
                    print_str(elapsed_time, pid, "PROC_CREAT", inf);

                    if (execve("./xmod", newArgv, envpGlobal) == -1)
                    {
                        //printf("returned -1 on execve, value of error: %d and content: %s\n", errno, strerror(errno));
                        printf("returned -1 on execve, value of error: %d\n", errno);
                        stop = clock();
                        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
                        pid_t pid = getpid();
                        print_int(elapsed_time, pid, "ERROR", 1);
                        exit(1);
                    }
                }
            }
        }
        // parent simply continues
    }
    (void)closedir(dirpath);

    if (contentInDir)
    {
        // parent -> wait() for children and check if all good before exiting
        int forkStatus;
        while (wait(&forkStatus) > 0)
            ; // this way, the father waits for all the child processes
    }
    stop = clock();
    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    pid_t pid = getpid();
    print_int(elapsed_time, pid, "PROC_EXIT", 0);
    exit(0);
}

int changePermissionsOfFile(char *file, char *permissions)
{
    struct stat buffer;
    if (stat(file, &buffer) != 0)
    {
        if (op.v)
        {
            optionV_print_failure(file, 0, 0);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
        }
        else if (op.c)
        {
            optionC_print_failure(file);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
        }
        return -1;
    }

    // current permissions
    unsigned int curr_perm = buffer.st_mode & 0777;

    unsigned int command = curr_perm;

    if (isdigit(permissions[0]))
    {
        if (make_command_from_octal_mode(permissions, &command) != 0)
        {
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
            return -1;
        }
    }
    else
    {
        if (make_command_from_text_mode(permissions, &command) != 0)
        {
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
            return -1;
        }
    }
    if (chmod(file, command) != 0)
    {
        perror("chmod() error");
        if (op.v)
        {
            optionV_print_failure(file, curr_perm, command);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
        }
        else if (op.c)
        {
            optionC_print_failure(file);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_int(elapsed_time, pid, "ERROR", 1);
        }
        return -1;
    }
    else
    {
        info.totalFiles++;
        if (curr_perm != command)
        {
            info.totalMod++;
        }
        if (op.v || op.c)
        {
            optionV_C_print_success(file, curr_perm, command);
            char *inf = malloc(strlen(file) + 15);
            snprintf(inf, strlen(file) + 15, "%s : %04o : %04o", file, curr_perm, command);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_str(elapsed_time, pid, "FILE_MODF", inf);
        }
        else if (!(op.v && op.R && op.c))
        {
            char *inf = malloc(strlen(file) + 15);
            snprintf(inf, strlen(file) + 15, "%s : %04o : %04o", file, curr_perm, command);
            stop = clock();
            double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
            pid_t pid = getpid();
            print_str(elapsed_time, pid, "FILE_MODF", inf);
        }
    }

    return 0;
}

void optionV_C_print_success(char *filename, unsigned int octalModePrevious, unsigned int octalModeAfter)
{
    char rwxModePrevious[10], rwxModeAfter[10];
    rwxModePrevious[9] = '\0';
    rwxModeAfter[9] = '\0';
    octal_to_text(octalModePrevious, rwxModePrevious);
    octal_to_text(octalModeAfter, rwxModeAfter);
    if (octalModePrevious != octalModeAfter)
        printf("mode of '%s' changed from %04o (%s) to %04o (%s)\n",
               filename, octalModePrevious, rwxModePrevious, octalModeAfter,
               rwxModeAfter);
    else if (op.v)
        printf("mode of '%s' retained as %04o (%s)\n", filename,
               octalModePrevious, rwxModePrevious);
}

void optionC_print_failure(char *filename)
{
    printf("xmod: cannot access '%s': No such file or directory\n", filename);
}

void optionV_print_failure(char *filename, unsigned int octalModePrevious, unsigned int octalModeAfter)
{
    char rwxModePrevious[10], rwxModeAfter[10];
    rwxModePrevious[9] = '\0';
    rwxModeAfter[9] = '\0';
    octal_to_text(octalModePrevious, rwxModePrevious);
    octal_to_text(octalModeAfter, rwxModeAfter);
    printf("xmod: cannot access '%s': No such file or directory\n", filename);
    printf("failed to change mode of '%s' from %04o (%s) to %04o (%s)\n",
           filename, octalModePrevious, rwxModePrevious, octalModeAfter,
           rwxModeAfter);
}

int main(int argc, char **argv, char **envp)
{
    info.totalFiles = 0;
    info.totalMod = 0;

    envpGlobal = envp;

    start = clock();

    int pidno[64];
    int size;
    int original = isOriginalProcess(pidno, &size);
    if (original)
    {
        if ((f_ptr = fopen(getenv("LOG_FILENAME"), "w")) == NULL)
        {
            printf("No LOG_FILENAME set. No logs will be registered\n");
        }
        else
        {
            fileopen = true;
        }
    }
    else
    {
        if ((f_ptr = fopen(getenv("LOG_FILENAME"), "a")) == NULL)
        {
            printf("No LOG_FILENAME set. No logs will be registered\n");
        }
        else
        {
            fileopen = true;
        }
    }

    char *inf = malloc(strlen(*argv) + 1);
    snprintf(inf, strlen(*argv) + 1, "%s", *argv);
    stop = clock();
    double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    pid_t pid = getpid();
    print_str(elapsed_time, pid, "PROC_CREAT", inf);

    if (argc < 3)
    {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\n");
        printf("xmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "ERROR", 1);
        return -1;
    }

    argvSize = 0;
    for (int i = 0; i < sizeof(argv) / sizeof(char); i++)
    {

        argvSize++;
        if (argv[i] == NULL)
        {
            break;
        }
    }

    // SIGNAL
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);

    size_t mode_idx = 1;

    while (argv[mode_idx][0] == '-')
    {
        switch (argv[mode_idx][1])
        {
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

    if (strlen(argv[mode_idx]) < 3 || strlen(argv[mode_idx]) > 5)
    {
        printf("ERROR: Invalid MODE format. ");
        printf("Should be <u|g|o|a><-|+|=><rwx> or <0><0-7><0-7><0-7>\n");

        stop = clock();
        double elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
        pid_t pid = getpid();
        print_int(elapsed_time, pid, "ERROR", 1);
        return -1;
    }
    fileNamepos = mode_idx + 1;
    info.originalFileDir = argv[mode_idx + 1];
    changePermissionsOfFileDir(argv[mode_idx + 1], argv[mode_idx], argv);

    stop = clock();
    elapsed_time = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    pid = getpid();
    print_int(elapsed_time, pid, "EXIT", 0);

    if (fileopen == true)
        fclose(f_ptr);

    return 0;
}