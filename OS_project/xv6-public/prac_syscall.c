#include "types.h"
#include "defs.h"
#include "x86.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

// Simple system call
int
getppid(void)
{
    return proc->parent->pid;
}
//Wrapper for my_syscall
int
sys_getppid(void)
{
    return getppid();
}
