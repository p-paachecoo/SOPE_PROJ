#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


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


   int fd = open (argv[2], O_WRONLY);

   //write(fd, buf, 5);

   printf("Hello, World!\n");
   return 0;
}