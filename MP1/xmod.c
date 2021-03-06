#include "xmod.h"

permission_type get_permissions(int oct)
{
    permission_type perm;
    switch (oct)
    {
    case 00:
        perm.read = false;
        perm.write = false;
        perm.execute = false;
        break;

    case 01:
        perm.read = false;
        perm.write = false;
        perm.execute = true;
        break;

    case 02:
        perm.read = false;
        perm.write = true;
        perm.execute = false;
        break;

    case 03:
        perm.read = false;
        perm.write = true;
        perm.execute = true;
        break;

    case 04:
        perm.read = true;
        perm.write = false;
        perm.execute = false;
        break;

    case 05:
        perm.read = true;
        perm.write = false;
        perm.execute = true;
        break;

    case 06:
        perm.read = true;
        perm.write = true;
        perm.execute = false;
        break;

    case 07:
        perm.read = true;
        perm.write = true;
        perm.execute = true;
        break;

    default:
        break;
    }

    return perm;
}

int main(int argc, char **argv, char **envp)
{
    if (argc < 3)
    {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\nxmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return 1;
    }

    if (strlen(argv[1]) < 3 || strlen(argv[1]) > 5)
    {
        printf("ERROR: Invalid MODE format. Should be <u|g|o|a><-|+|=><rwx>\n");
        return 1;
    }

    struct stat buffer;
    if (stat(argv[2], &buffer) != 0)
    {
        perror("ERROR");
        return 1;
    }

    // current permissions
    unsigned int curr_mode = buffer.st_mode & 0777;
    permission_type owner_perms = get_permissions((curr_mode & 0700) >> 6);
    permission_type group_perms = get_permissions((curr_mode & 0070) >> 3);
    permission_type other_perms = get_permissions(curr_mode & 0007);

    printf("OWNER - r: %d w: %d x: %d\n", owner_perms.read, owner_perms.write, owner_perms.execute);
    printf("GROUP - r: %d w: %d x: %d\n", group_perms.read, group_perms.write, group_perms.execute);
    printf("OTHER - r: %d w: %d x: %d\n", other_perms.read, other_perms.write, other_perms.execute);

    unsigned int command = 0;

    user_type user_t;
    permission_mode perm_mode;
    permission_type perm_type = {false, false, false};

    // MODE: user type
    switch (argv[1][0])
    {
    case 'u':
        user_t = OWNER;
        printf("Owner\n");
        break;

    case 'g':
        user_t = GROUP;
        printf("Group\n");
        break;

    case 'o':
        user_t = OTHER;
        printf("Other\n");
        break;

    case 'a':
        user_t = ALL;
        printf("All\n");
        break;

    default:
        printf("ERROR: Invalid user type. Should be one of <u|g|o|a>.\n");
        return 1;
        break;
    }

    // MODE: permission mode
    switch (argv[1][1])
    {
    case '-':
        perm_mode = REMOVE;
        printf("Remove\n");
        break;

    case '+':
        perm_mode = ADD;
        printf("Add\n");
        break;

    case '=':
        perm_mode = MODIFY;
        printf("Modify\n");
        break;

    default:
        printf("ERROR: Invalid permission mode. Should be one of <-|+|=>.\n");
        return 1;
        break;
    }

    // MODE: permission type
    for (int i = 2; i < strlen(argv[1]); i++)
    {
        switch (argv[1][i])
        {
        case 'r':
            perm_type.read = true;
            printf("Read\n");
            break;

        case 'w':
            perm_type.write = true;
            printf("Write\n");
            break;

        case 'x':
            perm_type.execute = true;
            printf("Execute\n");
            break;

        default:
            printf("ERROR: Invalid permission type. Should be at least one of <rwx>.\n");
            return 1;
            break;
        }
    }

    if (perm_mode != MODIFY)
        return 1;

    switch (user_t)
    {
    case OWNER:
        if (perm_type.read)
            command |= S_IRUSR;
        if (perm_type.write)
            command |= S_IWUSR;
        if (perm_type.execute)
            command |= S_IXUSR;
        break;

    case GROUP:
        if (perm_type.read)
            command |= S_IRGRP;
        if (perm_type.write)
            command |= S_IWGRP;
        if (perm_type.execute)
            command |= S_IXGRP;
        break;

    case OTHER:
        if (perm_type.read)
            command |= S_IROTH;
        if (perm_type.write)
            command |= S_IWOTH;
        if (perm_type.execute)
            command |= S_IXOTH;
        break;

    case ALL:
        if (perm_type.read)
            command |= S_IRUSR | S_IRGRP | S_IROTH;
        if (perm_type.write)
            command |= S_IWUSR | S_IWGRP | S_IWOTH;
        if (perm_type.execute)
            command |= S_IXUSR | S_IXGRP | S_IXOTH;
        break;

    default:
        break;
    }

    if (chmod(argv[2], command) != 0)
    {
        perror("chmod() error");
        return 1;
    }

    return 0;
}