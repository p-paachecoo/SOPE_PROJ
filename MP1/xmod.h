#ifndef MP1_XMOD_H_
#define MP1_XMOD_H_

/*
Necessary includes
*/

#define _POSIX_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define SIGHUP  1   /* Hangup the process */ 
#define SIGINT  2   /* Interrupt the process */ 
#define SIGQUIT 3   /* Quit the process */ 
#define SIGILL  4   /* Illegal instruction. */ 
#define SIGTRAP 5   /* Trace trap. */ 
#define SIGABRT 6   /* Abort. */

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
