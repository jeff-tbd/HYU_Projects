#include "types.h"
#include "defs.h"
#include "x86.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

//getlev system call
int
getlev(void)
{
    return proc->priority_lev;
}

///Wrapper function
int
sys_getlev(void)
{
    return getlev();
}
