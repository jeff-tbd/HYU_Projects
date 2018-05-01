#include "types.h"
#include "defs.h"

// Simple system call
int
yieldcall(void)
{
    yield();
    return 1;
}
//Wrapper for my_syscall
int
sys_yield(void)
{
    return yieldcall();
}
