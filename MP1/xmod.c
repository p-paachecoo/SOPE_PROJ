#include "xmod.h"
/* CELEBC11

   This example changes the permission from the file owner to the file's
   group.

 */

void sigint_handler(int signumber)
{
  fprintf(stderr, "\nReceived signal %d!\n", signumber);

  char answer;
  int cicle = 0;
  printf("Should the program terminate? (y/n)\n");
  while (cicle == 0)
  {
    answer = getchar();
    if (answer == 'y')
    {
      printf("Terminated\n");
      cicle = 1;
      exit(0);
    }
    else if (answer == 'n')
    {
      printf("Execution resumed\n");
      cicle = 1;
      return;
    }
  }
}

int main()
{
  signal(SIGINT, sigint_handler);
  struct sigaction new, old;
  sigset_t smask;                // defines signals to block while func() is running            // prepare struct sigaction
  if (sigemptyset(&smask) == -1) // block no signal
    perror("sigsetfunctions");

  new.sa_handler = sigint_handler;
  new.sa_mask = smask;
  new.sa_flags = 0; // usually works            if(sigaction(SIGUSR1, &new, &old) == -1)
  if (sigaction(SIGUSR1, &new, &old) == -1)
    perror("sigaction");
  printf("\nSIGUSR1 handler is ready...");
  printf("\n\t Try: \"sh> kill -SIGUSR1 %d\".\n", getpid());

  pause(); // wait
  printf("World!\n");

  char fn[] = "temp.txt";
  FILE *stream;
  struct stat info;

  if ((stream = fopen(fn, "w+")) == NULL)
    perror("fopen() error");
  else
  {
    fprintf(stream, "Testing...");
    fclose(stream);
    stat(fn, &info);
    printf("original permissions were: %08x\n", info.st_mode);
    if (chmod(fn, S_IRWXU | S_IRWXG) != 0)
      perror("chmod() error");
    else
    {
      stat(fn, &info);
      printf("after chmod(), permissions are: %08x\n", info.st_mode);
    }
    return 0;
    //unlink(fn);
  }
}