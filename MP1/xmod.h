#ifndef MP1_XMOD_H_
#define MP1_XMOD_H_

/*
Necessary includes
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef enum {
    OWNER,
    GROUP,
    OTHER,
    ALL
} user_type;

typedef enum {
    REMOVE,
    ADD,
    MODIFY
} permission_mode;

typedef struct {
    bool read;
    bool write;
    bool execute;
} permission_type;

/*
Responsible for handling the SIGINT signal.
*/
void sigint_handler(int signumber);

#endif  // MP1_XMOD_H_
