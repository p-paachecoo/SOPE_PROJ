#ifndef CLIENT_H_
#define CLIENT_H_

#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int fd_server;
int identifier_c = 1;
char *server_path;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
time_t initial_time;
int max_time;

struct message
{
    int rid;       // request id
    pid_t pid;     // process id
    pthread_t tid; // thread id
    int tskload;   // task load
    int tskres;    // task result
};

void createRequests();

void *makeRequest();

void log_msg(int rid, pid_t pid, pthread_t tid, int tskload, int tskres, char *operation);

#endif // CLIENT_H_
