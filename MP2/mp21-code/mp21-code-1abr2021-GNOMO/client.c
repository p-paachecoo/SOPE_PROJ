#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include "client.h"



int main(int argc, char *argv[]) {
   
   //Check Args
   if(argc != 4 || strcmp(argv[1],"-t") != 0){
      printf("Usage: c <-t nsecs> fifoname\n");
      exit(1);
   }


   if(atoi(argv[2]) <= 0){
      printf("<-t nsecs> must be greater than 0\n");
      exit(1);
   }

   //Start program life time countdown
   int err;
   pthread_t idTime;
   if ((err = pthread_create(&idTime, NULL, timeCountdown ,argv[2])) != 0)
   {
      fprintf(stderr, "Main thread Countdown: %s!\n", strerror(err));
      exit(-1);
   }


   while ((fd_public = open (argv[3], O_WRONLY)) < 0); // synchronization... will block until fifo opened for reading
   write(fd_public, "HELLO", 6);
   close(fd_public);

   pthread_t id;
   if ((err = pthread_create(&id, NULL, createRequests, NULL)) != 0)
   {
      fprintf(stderr, "Main thread Create Request: %s!\n", strerror(err));
      exit(-1);
   }

   printf("Hello, World!\n");



   if ((err = pthread_join(id, NULL)) != 0)
   fprintf(stderr, "Main thread: %s!\n", strerror(err));

   return 0;
}

//Create Requests Thread -> C0
void* createRequests(){

   printf("Creating Requests\n");
   time_t t;
   srand((unsigned) time(&t));
   int err;
   pthread_t id;

   //Launch Cn request threads
   while(1){
      usleep((rand() % 50 + 30)*1000); //sleep between 10 and 60ms
      if ((err = pthread_create(&id, NULL, makeRequest, NULL)) != 0)
      {
         fprintf(stderr, "C0 thread: %s!\n", strerror(err));
         exit(-1);
      }   
   
   }

   pthread_exit(NULL);
}

//Request Threads -> Cn
void* makeRequest(){
   printf("Making Request\n");
   time_t t;
   srand((unsigned) time(&t));
   int taskWeight = rand() % 8 + 1; //1-9 inclusive



   int pid = getpid();
   int sizePid = (int)((ceil(log10(pid))+1)*sizeof(char));
   char pidString[sizePid];
   sprintf(pidString, "%d.", pid);

   int tid = pthread_self();
   int sizeTid = (int)((ceil(log10(tid))+1)*sizeof(char));
   char tidString[sizeTid];
   sprintf(tidString, "%d", tid);

   strcat(pidString,tidString);

   char filePath[] = "/tmp/";
   strcat(filePath,pidString);

   //Create Private FIFO
   int fd_private;
   int msg[1024];

   if (mkfifo(filePath, 0666) < 0)
      perror("mkfifo");
   while((fd_private = open(filePath, O_RDONLY)) < 0); // synchronization... will block until fifo opened for reading
   printf("reading\n");
   read(fd_private, msg, 256);
   close(fd_private);

   pthread_exit(NULL);
} 

void* timeCountdown(void* time){
   char* timeStr = time;
   int timeInt = atoi(timeStr);
   sleep(timeInt);
   printf("KILL\n");
   exit(0);
   pthread_exit(NULL);
}