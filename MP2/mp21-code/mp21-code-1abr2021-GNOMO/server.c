#include "server.h"
#include "lib.h"

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

   if (argc == 6)
   {
      if ((buff_size = atoi(argv[4])) <= 0)
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

   buffer = malloc(buff_size * sizeof(messages));

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

   if (pthread_mutex_init(&lock1, NULL) != 0)
   {
      printf("\n mutex init failed\n");
      return 1;
   }

   if (pthread_mutex_init(&lock2, NULL) != 0)
   {
      printf("\n mutex init failed\n");
      return 1;
   }

   if (pthread_mutex_init(&lock3, NULL) != 0)
   {
      printf("\n mutex init failed\n");
      return 1;
   }

   // Create consumer thread
   createConsumer();

   // Launch Cn producer threads
   while (difftime(time(0), initial_time) < max_time)
   {
      struct message *msg_received = malloc(sizeof(message));

      if (read(fd_client_public, msg_received, sizeof(message)) > 0)
      {
         log_msg(msg_received->rid, getpid(), pthread_self(), msg_received->tskload, msg_received->tskres, "RECVD");
         createProducer(msg_received);
      }
   }

   free(buffer);

   close(fd_client_public);
   unlink(client_fifo_public);

   return 0;
}

void createProducer(struct message *msg)
{
   int err;
   pthread_t id;

   if ((err = pthread_create(&id, NULL, handleRequest, (void *)msg)) != 0)
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
   struct message *msg = (struct message *)arg;

   int task_res = task(msg->tskload);

   struct message *response = &(struct message){
       .rid = msg->rid,
       .pid = getpid(),
       .tid = pthread_self(),
       .tskload = msg->tskload,
       .tskres = task_res};

   struct messages *client_server = &(struct messages){
       .client = *msg,
       .server = *response};

   pthread_mutex_lock(&lock2);

   while (buff_num_elems >= buff_size)
      pthread_cond_wait(&buff_full, &lock2);

   buffer[buff_num_elems] = *client_server;
   buff_num_elems++;

   pthread_cond_signal(&buff_empty);

   pthread_mutex_unlock(&lock2);

   log_msg(response->rid, getpid(), pthread_self(), response->tskload, response->tskres, "TSKEX");

   free(msg);

   pthread_exit(NULL);
}

//Consumer Thread -> Sc
void *sendResponse()
{
   while (1)
   {
      pthread_mutex_lock(&lock3);

      while (buff_num_elems <= 0)
         pthread_cond_wait(&buff_empty, &lock3);

      struct message response = buffer[0].server;
      int client_pid = buffer[0].client.pid;
      long int client_tid = buffer[0].client.tid;

      for (int i = 0; i < buff_num_elems - 1; i++)
      {
         buffer[i] = buffer[i + 1];
      }
      buff_num_elems--;

      pthread_cond_signal(&buff_full);

      pthread_mutex_unlock(&lock3);

      char server_fifo[256];
      snprintf(server_fifo, sizeof(server_fifo), "/tmp/%d.%ld", client_pid, client_tid);

      int fd_server_private;
      fd_server_private = open(server_fifo, O_WRONLY | O_NONBLOCK);

      if (fd_server_private == -1)
      {
         log_msg(response.rid, response.pid, response.tid, response.tskload, response.tskres, "FAILD");
         close(fd_server_private);
         pthread_exit(NULL);
      }

      if (difftime(time(0), initial_time) >= max_time)
      {
         response.tskres = -1;

         pthread_mutex_lock(&lock1);
         write(fd_server_private, &response, sizeof(response));
         pthread_mutex_unlock(&lock1);

         log_msg(response.rid, response.pid, response.tid, response.tskload, response.tskres, "2LATE");

         signal(SIGPIPE, SIG_IGN);

         close(fd_server_private);
         pthread_exit(NULL);
      }
      else
      {
         pthread_mutex_lock(&lock1);
         int bytes = write(fd_server_private, &response, sizeof(response));
         pthread_mutex_unlock(&lock1);

         log_msg(response.rid, response.pid, response.tid, response.tskload, response.tskres, "TSKDN");

         signal(SIGPIPE, SIG_IGN);

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