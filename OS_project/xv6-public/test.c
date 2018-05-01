#include "types.h"
#include "stat.h"
#include "user.h"

void*
test(void *arg)
{
    int tid = (int) arg;
    int i;
    for(i = 0; i < 10000; i++)
    {
        if(i%2000 == 0)
        {
            printf(1, "%d", tid);
        }
    }
    thread_exit((void *)(tid + 1));
}

int
main(void)
{
    thread_t threads[10];
    int i;
    void *retval;

    for(i=0; i<10; i++)
    {
        if(thread_create(&threads[i], test, (void*)i) != 0)
        {
            printf(1, "panic at thread_create\n");
            return -1;
        }
    }
    for(i=0; i<10; i++)
    {
        if(thread_join(threads[i], &retval)!=0)
        {

            printf(1, "panic at thread_join\n");
            return -1;
        }
    }
    printf(1,"asdfsdfsadfsadfadsfsd\n");
    exit();
}
