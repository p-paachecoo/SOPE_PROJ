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

int main(int argc, char *argv[])
{
   //Check Args
   if (argc != 4 || strcmp(argv[1], "-t") != 0)
   {
      printf("Usage: c <-t nsecs> fifoname\n");
      exit(1);
   }

   if (atoi(argv[2]) <= 0)
   {
      printf("<-t nsecs> must be greater than 0\n");
      exit(1);
   }

   int timeInt = atoi(argv[2]);
   time_t start = time(0);

   server_path = argv[3];
   do {
      fd_server = open(argv[3], O_WRONLY);
      if (fd_server == -1) {
         printf("Connecting to server ...\n");
         sleep(1);
      }
   } while(fd_server == -1 && difftime(time(0), start) < timeInt);


   //Launch Cn request threads
   time_t t;
   srand((unsigned)time(&t));

   while (difftime(time(0), start) < timeInt)
   {
      printf("Creating Requests\n");
      createRequests();
      usleep((rand() % 50 + 30) * 1000); //sleep between 30 and 80ms
   } 

   printf("END\n");
   close(fd_server);

   return 0;
}

//Create Requests Thread -> C0
void createRequests()
{
   int err;
   pthread_t id;

   if ((err = pthread_create(&id, NULL, makeRequest, NULL)) != 0)
   {
      fprintf(stderr, "C0 thread: %s!\n", strerror(err));
      exit(-1);
   }
   //pthread_detach(id);

}

//Request Threads -> Cn
void *makeRequest()
{
   printf("Making Request\n");

   //Create Private FIFO
   char client_fifo[256];
   sprintf(client_fifo, "/tmp/%d.%ld", getpid(), pthread_self());

   printf("PATH %s\n", client_fifo);

   if (mkfifo(client_fifo, 0666) < 0)
      perror("mkfifo");
   
   int fd_client;
   fd_client = open(client_fifo, O_RDONLY | O_NONBLOCK);
   
   time_t t;
   srand((unsigned)time(&t));
   int task = rand() % 8 + 1; //1-9 inclusive

   struct message * msg = &(struct message) {
    .rid = identifier_c,
    .pid=getpid(),
    .tid=pthread_self(),
    .tskload = task,
    .tskres = -1
   };
   identifier_c += 1;

   //IWANT with private FIFO name
   printf("Sending private fifoname\n");
   write(fd_server, msg, sizeof(*msg));

   signal(SIGPIPE, SIG_IGN);

   //RCVD
   if (access(server_path, F_OK) != -1) {
      printf("Reading\n");

      struct message msg_received;
        int counter = 0;
        while (read(fd_client, &msg_received, sizeof(msg_received)) <= 0 && counter < 6) {
            usleep(10000);
            counter++;
        }
        if (counter < 6) //received msg or Server is closed
            printf("Message: %d %d %ld %d %d\n", msg_received.rid, msg_received.pid, msg_received.tid, msg_received.tskload, msg_received.tskres);
        else
            printf("Error Reading Request\n");


   } else
      printf("Cannot access Server\n");

   close(fd_client);
   unlink(client_fifo);

   pthread_exit(NULL);
}