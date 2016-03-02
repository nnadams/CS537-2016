// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "%d\n", shmem_access(0));
  printf(1, "%d\n", shmem_count(0));
  exit();
}
