#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();}

void spin()
{
	int i = 0, j = 0, k = 0;
	for(i = 0; i < 50; ++i)
	{
		for(j = 0; j < 10000000; ++j)
		{
			k = j % 10;
		}
	}
}

void print(struct pstat *st)
{
   int i;
   for(i = 0; i < NPROC; i++) {
      if (st->inuse[i]) {
         printf(1, "pid: %d hticks: %d lticks: %d\n", st->pid[i], st->hticks[i], st->lticks[i]);
      }
   }

}

void compare(struct pstat *before, struct pstat *after)
{
	int i, processesrun = 0;
	for(i = 0; i < NPROC; i++)
	{
		check(before->inuse[i] == after->inuse[i], "number of processes inuse viewed at different times should be same");
		check(before->pid[i] == after->pid[i], "the pid of processes at a particular index taken viewed at different times should be same");
		check(before->lticks[i] == after->lticks[i], "no low priority process should have been run when there is a runnable high priority process");
		if(before->hticks[i] < after->hticks[i])
			processesrun++;
	}
	check(processesrun == 1, "Expecetd that only the high prirority process is run when there is one");
}

int
main(int argc, char *argv[])
{
   if(fork() == 0)
   {
   	struct pstat st_before, st_after;
	check(setpri(2) == 0, "setpri");
	check(getpinfo(&st_before) == 0, "getpinfo");
	printf(1, "\n ****PInfo before**** \n");
	print(&st_before);
	spin();
	spin();
	check(getpinfo(&st_after) == 0, "getpinfo");
	printf(1, "\n ****PInfo after**** \n");
	print(&st_after);
	compare(&st_before, &st_after);
	printf(1, "Should print 1"); 
	exit();
   }
   spin();
   printf(1, " then 2");
   exit();
}
/*#include "types.h"
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
}*/
