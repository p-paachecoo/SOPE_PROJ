#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "client.h"



int main(int argc, char *argv[]) {
   // printf() displays the string inside quotation

   if(argc != 3){
      printf("Wrong Arguments!\nUsage: c <-t nsecs> fifoname\n");
      exit(1);
   }
   if(atoi(argv[1]) <= 0){
      printf("<-t nsecs> must be greater than 0\n");
      exit(1);
   }


   
   while ((fd_public = open (argv[2], O_WRONLY)) < 0); // synchronization... will block until fifo opened for reading
   write(fd_public, "HELLO", 6);
   close(fd_public);

   int err;
   pthread_t id;
   if ((err = pthread_create(&id, NULL, createRequests, NULL)) != 0)
   {
      fprintf(stderr, "Main thread: %s!\n", strerror(err));
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
      usleep((rand() % 50 + 10)*1000); //sleep between 10 and 60ms
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

   //Create Private FIFO
   int np;
   if (mkfifo("/tmp/np", 0666) < 0) // TO DO: Make private, change name
      perror("mkfifo");
   while ((np = open("/tmp/np", O_WRONLY)) < 0); // synchronization...
   write(np, "Hi, reader colleague!", 1+strlen("Hi, parent!"));
   close(np);

   pthread_exit(NULL);
}