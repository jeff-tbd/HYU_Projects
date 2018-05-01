#include "types.h"
#include "stat.h"
#include "user.h"

int 
main(int argc, char *argv[])
{
    int i = 0;
    int pid =fork();
    while((i++)<100)
    {
        if(pid == 0)
        {
            write(1, "child\n", 6);
            yield();
        }
        else if(pid > 0)
        {
            write(1, "parent\n", 7);
            yield();
        }
        else
        {
            printf(1,"fork error!\n");
            exit();
        }
    }
    if(pid>0)
    {
        wait();
    }
    exit();
}
    
