#include "types.h"
#include "param.h"
#include "pstat.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{
  struct pstat ps;
  int i;

  if (argc > 1)
    if ( (i = setpri(atoi(argv[1]))) != 0)
      printf(1, "setpri failed %d\n", i);

  if (getpinfo(&ps) != 0) {
    printf(1, "getpinfo failed\n");
    exit();
  }

  for (i = 0; i < NPROC; i++) {
    if (ps.inuse[i])
      printf(1, "%d 1:%d 2:%d\n", ps.pid[i], ps.hticks[i], ps.lticks[i]);
  }

  exit();
}
