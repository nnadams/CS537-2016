#include "types.h"
#include "param.h"
#include "pstat.h"
#include "stat.h"
#include "user.h"

#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();} 

int
main(int argc, char **argv)
{
  /*struct pstat ps;
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
  }*/
   struct pstat st;

   check(getpinfo(&st) == 0, "getpinfo");
   printf(1, "\n **** PInfo **** \n");
   int i;
   for(i = 0; i < NPROC; i++) {
      if (st.inuse[i]) {
         printf(1, "pid: %d hticks: %d lticks: %d\n", st.pid[i], st.hticks[i], st.lticks[i]);
      }
      else printf(1, "%d not in use\n", i);
   }

  check(getpinfo(NULL) == -1, "getpinfo with bad pointer");
  printf(1, "Should print 1 then 2");

  exit();
}
