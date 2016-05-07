#include "fcntl.h"
#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];
int n;

void
cat(int fd)
{
  while((n = read(fd, buf, sizeof(buf))) > 0)
    write(1, buf, n);
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd2;
  char buff = 'a';

  int fd = open("sm.txt", O_CREATE | O_SMALLFILE | O_RDWR);
  printf(1, "Creating small file (%d)... ", fd);
  printf(1, "%d\n", write(fd, &buff, 1));
  close(fd);

  fd = open("sm.txt", NULL);
  printf(1, "*** FILE DATA START***\n");
  cat(fd);
  printf(1, "*** FILE DATA END***\n");
  close(fd);

  fd = open("sm.txt",O_SMALLFILE | O_WRONLY);
  fd2 = open("README", NULL);
  while((n = read(fd2, buf, sizeof(buf))) > 0)
    write(fd, buf, n);
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
  close(fd);
  close(fd2);

  fd = open("sm.txt", O_SMALLFILE);
  cat(fd);
  exit();
}
