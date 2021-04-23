#ifndef SERVER_H
#define SERVER_H

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

int client_fifo_public;
int fd_client_public;
int buff_size;

time_t initial_time;
int max_time;
pthread_mutex_t lock1;

struct message
{
    int rid;       // request id
    pid_t pid;     // process id
    pthread_t tid; // thread id
    int tskload;   // task load
    int tskres;    // task result
} message;


#endif // SERVER_H