#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
    int inuse[NPROC];  // slot in use in process table (1 or 0)
    int pid[NPROC];    // pid
    int hticks[NPROC]; // ticks in priority 2
    int lticks[NPROC]; // ticks in priority 1 
};

#endif
