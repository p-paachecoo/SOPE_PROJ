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

   while (((fd_public = open(argv[3], O_WRONLY)) < 0) && (difftime(time(0), start) < timeInt))
      ; // synchronization... will block until fifo opened for reading


   //Launch Cn request threads
   time_t t;
   srand((unsigned)time(&t));

   while (difftime(time(0), start) < timeInt)
   {
      printf("Creating Requests\n");
      createRequests();
      usleep((rand() % 50 + 30) * 1000); //sleep between 10 and 60ms
   }

   printf("END\n");
   close(fd_public);

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
   pthread_detach(id);

}

//Request Threads -> Cn
void *makeRequest()
{
   printf("Making Request\n");

   int pid = getpid();
   int sizePid = (int)((ceil(log10(pid)) + 1) * sizeof(char));
   char pidString[sizePid + 1];
   sprintf(pidString, "%d.", pid);

   int tid = pthread_self();
   int sizeTid = (int)((ceil(log10(tid)) + 1) * sizeof(char));
   char tidString[sizeTid];
   sprintf(tidString, "%d", tid);

   strcat(pidString, tidString);

   char filePath[] = "/tmp/";
   strcat(filePath, pidString);

   //Create Message
   char msg[1024];
   createPublicMessage(msg);

   //IWANT with private FIFO name
   printf("Sending private fifoname %s\n", msg);
   write(fd_public, msg, sizeof(msg));
   printf("Sent\n");

   signal(SIGPIPE, SIG_IGN);

   //Create Private FIFO
   int fd_private;
   char msg_received[1024];

   if (mkfifo(filePath, 0666) < 0)
      perror("mkfifo");
   while ((fd_private = open(filePath, O_RDONLY)) < 0)
      ; // synchronization... will block until fifo opened for reading
   printf("reading\n");
   if (read(fd_private, msg_received, 256) < 0)
      printf("Error Reading Request\n");
   else
      printf("Message: %s\n", msg_received);
   close(fd_private);
   unlink(filePath);

   pthread_exit(NULL);
}

/* Fills msg with according values */
void createPublicMessage(char* msg)
{

   int pid = getpid();
   unsigned long int tid = pthread_self();
   int res = -1; //Client
   int size_res = (int)((ceil(log10(res*-1)) + 1) * sizeof(char));
   char res_string[size_res+1];
   if(sizeof(res_string) < 0 )
      printf("Error\n");

   int size_i = (int)((ceil(log10(identifier_c)) + 1) * sizeof(char));
   char i_string[size_i + 1];
   sprintf(i_string, "%d ", identifier_c);
   identifier_c += 1;

   srand(identifier_c*time(NULL));
   int task = rand() % 8 + 1; //1-9 inclusive

   int size_task = (int)((ceil(log10(task)) + 1) * sizeof(char));
   char task_string[size_task + 1];
   sprintf(task_string, "%d ", task);

   int size_pid = (int)((ceil(log10(pid)) + 1) * sizeof(char));
   char pid_string[size_pid + 1];
   sprintf(pid_string, "%d ", pid);

   unsigned long int size_tid = (unsigned long int)((ceil(log10(tid)) + 1) * sizeof(char));
   char tid_string[size_tid + 1];
   sprintf(tid_string, "%lu ", tid);

   strcat(i_string, task_string);
   strcat(i_string, pid_string);
   strcat(i_string, tid_string);

   if(res > 0){
      int size_res2 = (int)((ceil(log10(res)) + 1) * sizeof(char));
      char res_string2[size_res2];
      sprintf(res_string2, "%d", res);
      strcat(i_string, res_string2);
   } else{
      int size_res2 = (int)((ceil(log10(res*-1)) + 1) * sizeof(char));
      char res_string2[size_res2+1];
      sprintf(res_string2, "%d", res);
      strcat(i_string, res_string2);
   }
   
   printf("Message: %s\n", i_string);
   strcpy(msg, i_string);
   
}