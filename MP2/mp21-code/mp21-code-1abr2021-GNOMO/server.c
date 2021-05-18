#include "server.h"

int main(int argc, char *argv[])
{
   //Check Args
   if ((argc != 5 && argc != 4) || strcmp(argv[1], "-t") != 0)
   {
      printf("Usage: c <-t nsecs> [-l buffsz] fifoname\n");
      exit(1);
   }

   if (atoi(argv[2]) <= 0)
   {
      printf("<-t nsecs> must be greater than 0\n");
      exit(1);
   }

   if (argc == 5)
   {
      if ((buff_size = atoi(argv[3])) <= 0)
      {
         printf("[-l buffsz] must be greater than 0\n");
         exit(1);
      }
   }
   else
      buff_size = 1;

   max_time = atoi(argv[2]);
   initial_time = time(0);

   //Create public FIFO
   client_fifo_public = argv[3];

   if (mkfifo(client_fifo_public, 0666) < 0)
   {
      perror("mkfifo");
   }

   printf("Waiting for client to connect ...\n");
   do
   {
      fd_client_public = open(client_fifo_public, O_RDONLY);
      if (fd_client_public == -1)
      {
         sleep(1);
      }
   } while (fd_client_public == -1 && difftime(time(0), initial_time) < max_time);

   if (pthread_mutex_lock(&lock1) != 0)
   {
      printf("\n mutex init lock1 failed\n");
      return 1;
   }
   else{
      identifier_c += 1;
      pthread_mutex_unlock(&lock1);
   }

   //Launch Cn producer threads
   unsigned int seed = (unsigned)(time(&t) + pthread_self() % 100);
   srand(seed);
   int task = rand_r(&seed) % 8 + 1; //1-9 inclusive

   while (difftime(time(0), initial_time) < max_time)
   {
      if (!client_closed)
         getRequests();
         log_msg(request.id, getpid(), pthread_self(), task, -1, "RECVD");

      usleep((rand_r(&seed) % 50 + 30) * 1000); //sleep between 30 and 80ms
   }

   close(fd_client_public);
   unlink(client_fifo_public);

   return 0;
}

//Main Thread -> S0
void getRequests()
{
   int err;
   pthread_t id;
   message request;



   if ((err = pthread_create(&id, NULL, handleRequest, NULL)) != 0)
   {
      fprintf(stderr, "S0 thread: %s!\n", strerror(err));
      log_msg(request.id, getpid(), pthread_self(), task, -1, "RECVD");
      exit(-1);
   }

   log_msg(request.id, getpid(), pthread_self(), request.dur, request.pl, "RECVD");
}

//Producer Threads -> Sn
void *handleRequest()
{
   struct message msg_received;

   /*ssize_t num_bytes_read = */ read(fd_client_public, &msg_received, sizeof(message));

   buffer[sizeof(buffer)] = msg_received;

   log_msg(msg_received.rid, getpid(), pthread_self(), msg_received.tskload, msg_received.tskres, "RECVD");

   char server_fifo[256];
   snprintf(server_fifo, sizeof(server_fifo), "/tmp/%d.%ld", msg_received.pid, msg_received.tid);

   int fd_server_private;
   fd_server_private = open(server_fifo, O_RDONLY | O_NONBLOCK);

   time_t t;

   unsigned int seed = (unsigned)(time(&t) + pthread_self() % 100);
   srand(seed);
   int task = rand_r(&seed) % 8 + 1; //1-9 inclusive

   struct message *msg = &(struct message){
       .rid = msg_received.rid,
       .pid = getpid(),
       .tid = pthread_self(),
       .tskload = task,
       .tskres = -1};

   if (difftime(time(0), initial_time) >= max_time)
   {
      close(fd_server_private);
      pthread_exit(NULL);
   }

   pthread_mutex_lock(&lock1);

   write(fd_server_private, msg, sizeof(*msg));

   pthread_mutex_unlock(&lock1);

   log_msg(msg->rid, msg->pid, msg->tid, msg->tskload, msg->tskres, "TSKDN");

   close(fd_server_private);

   pthread_exit(NULL);
}

void log_msg(int rid, pid_t pid, pthread_t tid, int tskload, int tskres, char *operation)
{
   char msg[128];
   time_t inst = time(NULL);

   snprintf(msg, sizeof(msg), "%ld ; %d ; %d ; %d ; %ld ; %d ; %s\n", inst, rid, tskload, pid, tid, tskres, operation);

   if (msg != NULL)
      write(STDOUT_FILENO, msg, strlen(msg));
}