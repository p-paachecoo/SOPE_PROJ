#include "xmod.h"
/* CELEBC11

   This example changes the permission from the file owner to the file's
   group.

 */
#define _POSIX_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>

void main() {
  char fn[]="temp.txt";
  FILE *stream;
  struct stat info;

  if ((stream = fopen(fn, "w+")) == NULL)
    perror("fopen() error");
  else {
    fprintf(stream, "Testing...");
    fclose(stream);
    stat(fn, &info);
    printf("original permissions were: %08x\n", info.st_mode);
    if (chmod(fn, S_IRWXU|S_IRWXG) != 0)
      perror("chmod() error");
    else {
      stat(fn, &info);
      printf("after chmod(), permissions are: %08x\n", info.st_mode);
    }
    //unlink(fn);
  }
}