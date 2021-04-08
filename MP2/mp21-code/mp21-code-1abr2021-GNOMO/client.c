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
   if ((err = pthread_create(&id, NULL, startRequests, NULL)) != 0)
   {
      fprintf(stderr, "Main thread: %s!\n", strerror(err));
      exit(-1);
   }

   printf("Hello, World!\n");



   if ((err = pthread_join(id, NULL)) != 0)
   fprintf(stderr, "Main thread: %s!\n", strerror(err));

   return 0;
}

//Main Thread -> C0
void* startRequests(){
   printf("Child thread!\n");
   pthread_exit(NULL);
}