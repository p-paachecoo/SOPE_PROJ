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

   max_time = atoi(argv[2]);
   initial_time = time(0);

   server_path = argv[3];
   do
   {
      fd_server = open(server_path, O_WRONLY);
      if (fd_server == -1)
      {
         printf("Connecting to server ...\n");
         sleep(1);
      }
   } while (fd_server == -1 && difftime(time(0), initial_time) < max_time);

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

   //Launch Cn request threads
   time_t t;
   unsigned int seed = (unsigned)time(&t);
   srand(seed);

   while (difftime(time(0), initial_time) < max_time)
   {
      if (!server_closed)
         createRequests();

      usleep((rand_r(&seed) % 50 + 30) * 1000); //sleep between 30 and 80ms
   }

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
}

//Request Threads -> Cn
void *makeRequest()
{

   //Create Private FIFO
   char client_fifo[256];
   snprintf(client_fifo, sizeof(client_fifo), "/tmp/%d.%ld", getpid(), pthread_self());

   if (mkfifo(client_fifo, 0666) < 0)
   {
      perror("mkfifo");
   }

   int fd_client;
   fd_client = open(client_fifo, O_RDONLY | O_NONBLOCK);

   time_t t;

   unsigned int seed = (unsigned)(time(&t) + pthread_self() % 100);
   srand(seed);
   int task = rand_r(&seed) % 8 + 1; //1-9 inclusive

   struct message *msg = &(struct message){
       .rid = identifier_c,
       .pid = getpid(),
       .tid = pthread_self(),
       .tskload = task,
       .tskres = -1};

   pthread_mutex_lock(&lock1);
   identifier_c += 1;
   pthread_mutex_unlock(&lock1);

   if (difftime(time(0), initial_time) >= max_time)
   {
      close(fd_client);
      unlink(client_fifo);

      pthread_exit(NULL);
   }

   pthread_mutex_lock(&lock2);

   write(fd_server, msg, sizeof(*msg));

   pthread_mutex_unlock(&lock2);

   log_msg(msg->rid, msg->pid, msg->tid, msg->tskload, msg->tskres, "IWANT");

   signal(SIGPIPE, SIG_IGN);

   struct message msg_received;
   int timeout = 0;
   while (read(fd_client, &msg_received, sizeof(msg_received)) <= 0)
   {
      if (difftime(time(0), initial_time) >= max_time)
      {
         timeout = 1;
         break;
      }
      usleep(10000);
   }
   if (timeout == 0)
   { //received msg
      if (msg_received.tskres == -1)
      {
         log_msg(msg->rid, getpid(), pthread_self(), msg_received.tskload, msg_received.tskres, "CLOSD");
         server_closed = 1;
      }
      else
         log_msg(msg->rid, getpid(), pthread_self(), msg_received.tskload, msg_received.tskres, "GOTRS");
   }
   else
      log_msg(msg->rid, getpid(), pthread_self(), msg->tskload, msg->tskres, "GAVUP");

   close(fd_client);
   unlink(client_fifo);

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