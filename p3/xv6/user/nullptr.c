#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int *ptr = NULL;

  // Deref NULL ptr; 4 procs should get killed
  printf(1, "%d\n", ptr);
  fork();
  fork();
  printf(1, "%d\n\n", *ptr);

  exit();
}
