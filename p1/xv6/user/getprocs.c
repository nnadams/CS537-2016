#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "%d\n", getprocs());
  fork();
  printf(1, "%d\n", getprocs());
  exit();
}