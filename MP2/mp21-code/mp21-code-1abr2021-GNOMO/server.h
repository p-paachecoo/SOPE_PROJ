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

int fd_client_public;
int buff_size;
int identifier_c = 1;

time_t initial_time;
int max_time;
pthread_mutex_t lock1;
pthread_cond_t buff_full;
pthread_cond_t buff_empty;

char *client_fifo_public;

struct message
{
    int rid;       // request id
    pid_t pid;     // process id
    pthread_t tid; // thread id
    int tskload;   // task load
    int tskres;    // task result
} message;

struct message *buffer;
int buff_num_elems = 0;

void createProducer(struct message msg);

void createConsumer();

void *handleRequest(void *arg);

void *sendResponse();

void log_msg(int rid, pid_t pid, pthread_t tid, int tskload, int tskres, char *operation);

#endif // SERVER_H