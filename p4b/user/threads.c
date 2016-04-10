/* clone and play with the argument */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
volatile int arg = 55;
volatile int global = 1;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();
   void *stack = malloc(PGSIZE*2);
   assert(stack != NULL);
   if((uint)stack % PGSIZE)
     stack = stack + (4096 - (uint)stack % PGSIZE);

   int clone_pid = clone(worker, (void*)&arg, stack);
   assert(clone_pid > 0);

   /*int size = PGSIZE*2;
   for (int i = 0; i < size; i++) {
       if (i == 4096)
          printf(1, "\n---\n");

       printf(1, "%d ", ((unsigned char *) stack) [i]);
       if (i % 35 == 0)
          printf(1, "\n");
       if (i == 4079)
          printf(1, "\n---\n");
       if (i == 4083)
          printf(1, "\n---\n");
   }*/

   while(global != 55);
   assert(arg == 1);
   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   if (arg_ptr == NULL)
       printf(1, "\nNULL ptr!\n");
   printf(1, "\nArg*=%d  Global*=%d\n", (int)arg_ptr, (int)&global);

   int tmp = *(int*)arg_ptr;
   printf(1, "\nArg=%d\n", tmp);
   *(int*)arg_ptr = 1;
   assert(global == 1);
   global = tmp;
   exit();
}
