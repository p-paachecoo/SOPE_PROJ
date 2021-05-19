#include "server.h"

//Main Thread -> S0
int main(int argc, char *argv[])
{
   //Check Args
   if ((argc != 6 && argc != 4) || strcmp(argv[1], "-t") != 0)
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
      client_fifo_public = argv[5];
   }
   else
   {
      buff_size = 1;
      client_fifo_public = argv[3];
   }

   buffer = malloc(buff_size * sizeof(message));

   max_time = atoi(argv[2]);
   initial_time = time(0);

   //Create public FIFO
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
   else
   {
      identifier_c += 1;
      pthread_mutex_unlock(&lock1);
   }

   // Create consumer thread
   createConsumer();

   // Launch Cn producer threads
   time_t t;
   unsigned int seed = (unsigned)(time(&t) + pthread_self() % 100);
   srand(seed);

   while (difftime(time(0), initial_time) < max_time)
   {
      struct message msg_received;

      if (read(fd_client_public, &msg_received, sizeof(message)) > 0 && !client_closed)
      {
         createProducer(msg_received);
      }
      else if (client_closed)
      {
         log_msg(msg_received.rid, getpid(), pthread_self(), msg_received.tskload, msg_received.tskres, "FAILD");
      }
   }

   close(fd_client_public);
   unlink(client_fifo_public);

   return 0;
}

void createProducer(struct message msg)
{
   int err;
   pthread_t id;

   if ((err = pthread_create(&id, NULL, handleRequest, (void *)&msg)) != 0)
   {
      fprintf(stderr, "S0 thread: %s!\n", strerror(err));
      exit(-1);
   }
}

void createConsumer()
{
   int err;
   pthread_t id;

   if ((err = pthread_create(&id, NULL, sendResponse, NULL)) != 0)
   {
      fprintf(stderr, "S0 thread: %s!\n", strerror(err));
      exit(-1);
   }
}

//Producer Threads -> Sn
void *handleRequest(void *arg)
{
   struct message *msg = arg;

   log_msg(msg->rid, getpid(), pthread_self(), msg->tskload, msg->tskres, "RECVD");

   time_t t;

   unsigned int seed = (unsigned)(time(&t) + pthread_self() % 100);
   srand(seed);
   int task = rand_r(&seed) % 8 + 1; //1-9 inclusive

   struct message *response = &(struct message){
       .rid = msg->rid,
       .pid = getpid(),
       .tid = pthread_self(),
       .tskload = task,
       .tskres = -1};

   buffer[buff_num_elems] = *response;
   buff_num_elems++;

   pthread_exit(NULL);
}

//Consumer Thread -> Sc
void *sendResponse()
{
   while (difftime(time(0), initial_time) < max_time)
   {
      if (buff_num_elems > 0)
      {
         struct message response = buffer[0];

         for (int i = 0; i < buff_num_elems - 1; i++)
         {
            buffer[i] = buffer[i + 1];
         }
         buff_num_elems--;

         char server_fifo[256];
         snprintf(server_fifo, sizeof(server_fifo), "/tmp/%d.%ld", response.pid, response.tid);

         int fd_server_private;
         fd_server_private = open(server_fifo, O_RDONLY | O_NONBLOCK);

         if (difftime(time(0), initial_time) >= max_time)
         {
            log_msg(response.rid, response.pid, response.tid, response.tskload, response.tskres, "2LATE");
            close(fd_server_private);
            pthread_exit(NULL);
         }

         pthread_mutex_lock(&lock1);

         write(fd_server_private, &response, sizeof(response));

         pthread_mutex_unlock(&lock1);

         log_msg(response.rid, response.pid, response.tid, response.tskload, response.tskres, "TSKDN");

         close(fd_server_private);
      }
   }

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