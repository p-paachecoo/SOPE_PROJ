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

   if(argc == 5){
      if ((buff_size = atoi(argv[3])) <= 0)
      {
         printf("[-l buffsz] must be greater than 0\n");
         exit(1);
      }
   } else 
      buff_size = 1;


   max_time = atoi(argv[2]);
   initial_time = time(0);

   //Create public FIFO
   client_fifo_public = argv[4];
      if (mkfifo(client_fifo_public, 0666) < 0)
   {
      perror("mkfifo");
   }

   printf("Waiting for client to connect ...\n");
   do
   {
      fd_client_public = open(argv[3], O_RDONLY);
      if (fd_client_public == -1)
      {
         sleep(1);
      }
   } while (fd_client_public == -1 && difftime(time(0), initial_time) < max_time);

   if (pthread_mutex_init(&lock1, NULL) != 0)
   {
      printf("\n mutex init lock1 failed\n");
      return 1;
   }


   //Launch Cn producer threads
   time_t t;
   unsigned int seed = (unsigned)time(&t);
   srand(seed);

   while (difftime(time(0), initial_time) < max_time)
   {
      char buf_msg[sizeof(message)];

      ssize_t num_bytes_read = read(fd_client_public, buf_msg, sizeof(message));

      //TO DO process message
      
      usleep((rand_r(&seed) % 50 + 30) * 1000); //sleep between 30 and 80ms

   }


   close(fd_client_public);

   return 0;
}