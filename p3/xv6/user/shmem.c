// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char *pg0, *pg1, *pg2, *pg3;
  int write;

  if (argc <= 1)
    write = 0;
  else
    write = atoi(argv[1]);

  pg0 = shmem_access(0);
  printf(1, "\n%d pg0= %d\n", getpid(), (int)pg0);
  printf(1, "%d\t%d %d %d %d\n", getpid(), shmem_count(0), shmem_count(1), shmem_count(2), shmem_count(3));

  pg1 = shmem_access(1);
  printf(1, "\n%d pg1= %d\n", getpid(), (int)pg1);
  printf(1, "%d\t%d %d %d %d\n", getpid(), shmem_count(0), shmem_count(1), shmem_count(2), shmem_count(3));

  pg2 = shmem_access(2);
  printf(1, "\n%d pg2= %d\n", getpid(), (int)pg2);
  printf(1, "%d\t%d %d %d %d\n", getpid(), shmem_count(0), shmem_count(1), shmem_count(2), shmem_count(3));

  pg3 = shmem_access(3);
  printf(1, "\n%d pg3= %d\n", getpid(), (int)pg3);
  printf(1, "%d\t%d %d %d %d\n", getpid(), shmem_count(0), shmem_count(1), shmem_count(2), shmem_count(3));
  printf(1, "%d\t%d\n", getpid(), shmem_count(4));

  if (write)
  {
    *pg0 = 11;
    *pg1 = 22;
    *pg2 = 33;
    *pg3 = 44;
    if (fork() != 0) {
      argv[1] = "0";
      exec("shmem", argv);
    }
  }

  int i;
  char *ptr;

  for (i=0; i < 4; i++)
  {
      switch (i) {
        case 0: ptr = pg0; break;
        case 1: ptr = pg1; break;
        case 2: ptr = pg2; break;
        case 3: ptr = pg3; break;
      }
      printf(1, "%d pg%d= %d\n", getpid(), i, *ptr);
  }
  printf(1, "%d My args: %s %s\n", getpid(), argv[0], argv[1]);

  exit();
}
