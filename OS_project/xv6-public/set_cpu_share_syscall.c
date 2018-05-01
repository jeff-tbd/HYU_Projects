#include "types.h"
#include "defs.h"

//set cpu share system call
int
set_cpu_share(int cpu_share)
{
    return 0;
}

//Wrapper function
int
sys_set_cpu_share(void)
{
    int num;
    if(argint(0, &num) < 0){
        return -1;
    }
    return set_cpu_share(num);
}

