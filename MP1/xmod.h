/*
Necessary includes
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

typedef enum
{
    OWNER,
    GROUP,
    OTHER,
    ALL
} user_type;

typedef enum
{
    REMOVE,
    ADD,
    MODIFY
} permission_mode;

typedef struct
{
    bool read;
    bool write;
    bool execute;
} permission_type;

/*
Responsible for handling the SIGINT signal.
*/
void sigint_handler(int signumber);