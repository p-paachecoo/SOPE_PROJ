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
#include <ctype.h>
#include <math.h>


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


#include <time.h>
#include <unistd.h>
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

typedef struct {
    char* originalFileDir;
    int totalFiles;
    int totalMod;
} sig_info;


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
