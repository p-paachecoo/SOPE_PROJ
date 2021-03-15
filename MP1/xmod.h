#ifndef MP1_XMOD_H_
#define MP1_XMOD_H_

/*
Necessary includes
*/
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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
* Info printing for integers
* ie: PROC_EXIT and SIGNAL_RECV
*/
void print_int(double instant, pid_t pid, char event[], int info);

/*
* Info printing for sentences
* ie: PROC_CREAT , SIGNAL_SENT and FILE_MODF
*/
void print_str(double instant, pid_t pid, char event[], char info[]);

/*
* Print function when SIGINT is received
*/
void end_sig_print(pid_t pid, char file_dir[], int nftot, int nfmod);

#endif  // MP1_XMOD_H_
