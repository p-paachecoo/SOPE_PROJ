#include "xmod.h"

int main(int argc, char **argv, char **envp)
{
    if (argc < 3)
    {
        printf("Usage:\nxmod [OPTIONS] MODE FILE/DIR\nxmod [OPTIONS] OCTAL-MODE FILE/DIR\n");
        return 1;
    }

    if (strlen(argv[1]) < 3)
    {
        printf("ERROR: Invalid MODE format. Should be <u|g|o|a><-|+|=><rwx>\n");
        return 1;
    }

    // MODE: user type
    switch (argv[1][0])
    {
    case 'u':
        printf("User\n");
        break;

    case 'g':
        printf("Group\n");
        break;

    case 'o':
        printf("Other\n");
        break;

    case 'a':
        printf("All\n");
        break;

    default:
        printf("ERROR: Invalid user type. Should be one of <u|g|o|a>.");
        return 1;
        break;
    }

    // MODE: permission mode
    switch (argv[1][1])
    {
    case '-':
        printf("Remove\n");
        break;

    case '+':
        printf("Add\n");
        break;

    case '=':
        printf("Modify\n");
        break;

    default:
        printf("ERROR: Invalid permission mode. Should be one of <-|+|=>.");
        return 1;
        break;
    }

    // MODE: permission type
    char perm[3];
    strncpy(perm, argv[1] + 2, 3);
    if (strlen(argv[1]) > 5 || (strchr(perm, 'r') == NULL && strchr(perm, 'w') == NULL && strchr(perm, 'x') == NULL))
    {
        printf("ERROR: Invalid permission type. Should be at least one of <rwx>.\n");
        return 1;
    }
    if (strchr(perm, 'r') != NULL)
    {
        printf("Read\n");
    }
    if (strchr(perm, 'w') != NULL)
    {
        printf("Write\n");
    }
    if (strchr(perm, 'x') != NULL)
    {
        printf("Execute\n");
    }

    //TODO: Check for invalid chars outside of rwx

    return 0;
}