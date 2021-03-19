#ifndef MP1_XMOD_H_
#define MP1_XMOD_H_

/*
Necessary includes
*/
#define __USE_XOPEN_EXTENDED
#define _POSIX_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#define __USE_POSIX2
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define SIGHUP 1   /* Hangup the process */
#define SIGINT 2   /* Interrupt the process */
#define SIGQUIT 3  /* Quit the process */
#define SIGILL 4   /* Illegal instruction. */
#define SIGTRAP 5  /* Trace trap. */
#define SIGABRT 6  /* Abort. */
#define SIGKILL 9  /* Kill */
#define SIGCHLD 17 /* Child */
#define SIGCONT 18 /* Continue */
#define SIGSTOP 19 /* Stop */

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
    char *originalFileDir;
    int totalFiles;
    int totalMod;
} sig_info;

typedef struct {
    bool R;
    bool v;
    bool c;
} options;

int isOriginalProcess();
extern int usleep(__useconds_t __useconds);

char *concat(const char *s1, const char *s2);

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
void end_sig_print(double instant, pid_t pid, char event[], char info[]);

void optionV_C_print_success(char *filename, unsigned int octalModePrevious,
                             unsigned int octalModeAfter);

void optionC_print_failure(char *filename);

void optionV_print_failure(char *filename, unsigned int octalModePrevious,
                           unsigned int octalModeAfter);

int isDirectory(const char *path);

int changePermissionsOfFileDir(char *fileDir, char *permissions, char **argv);

void changePermissionsOfWholeDir(char *Dir, char **argv);

int changePermissionsOfFile(char *file, char *permissions);

void sigint_handler(int signumber);

void sigchild_handler(int signumber);

int make_command_from_text_mode(char *mode, unsigned int *command);

int make_command_from_octal_mode(char *mode, unsigned int *command);

void octal_to_text(unsigned int oct, char *text);

#endif  // MP1_XMOD_H_
