#include "xmod.h"

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
        printf("ERROR: Invalid user type. Should be one of <u|g|o|a>.\n");
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
        printf("ERROR: Invalid permission mode. Should be one of <-|+|=>.\n");
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
            printf("ERROR: Invalid permission type. Should be at least one of <rwx>.\n");
            return -1;
            break;
        }
    }

    switch (user_t)
    {
    case OWNER:
        //remove current permissions for OWNER
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
        //remove current permissions for GROUP
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
        //remove current permissions for OTHER
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
        //remove current permissions for ALL
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

int main(int argc, char **argv, char **envp)
{
    if (argc < 3)
    {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\nxmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return -1;
    }

    if (strlen(argv[1]) < 3 || strlen(argv[1]) > 5)
    {
        printf("ERROR: Invalid MODE format. Should be <u|g|o|a><-|+|=><rwx>\n");
        return -1;
    }

    struct stat buffer;
    if (stat(argv[2], &buffer) != 0)
    {
        perror("ERROR");
        return -1;
    }

    // current permissions
    unsigned int curr_perm = buffer.st_mode & 0777;

    unsigned int command = curr_perm;

    if (make_command_from_text_mode(argv[1], &command) != 0)
    {
        printf("Error making new command.\n");
        return -1;
    }

    if (chmod(argv[2], command) != 0)
    {
        perror("chmod() error");
        return -1;
    }

    return 0;
}