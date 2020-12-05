#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "exacto_commander/exacto_data_storage.h"

thread_control_t MainThread;

#define FUN_TEST

uint8_t MarkerThread = 0;

static int waitingRun(struct lthread *self) {
    thread_control_t * _trg_th;
    goto *lthread_resume(self, &&start);
start:
    /* инициализация */
    _trg_th = (thread_control_t *)self;
mutex_retry:
    if (mutex_trylock_lthread(self, &_trg_th->mx ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    if (_trg_th->result == OK)
    {
        MarkerThread = 1;
    }
    // mutex_unlock_lthread(self, &MainThread.mx);
    mutex_unlock_lthread(self, &_trg_th->mx);
    return 0;
}
#ifdef FUN_TEST
uint8_t initLthreadBase( thread_control_t * base )
{
    mutex_init_schedee(&base->mx);
    lthread_init(&base->thread, waitingRun);
    base->result = WAIT;
    return 0;
}
uint8_t launchLthreadBase( thread_control_t * base)
{
    lthread_launch(&base->thread);
    return 0;
}
#endif


int main(int argc, char *argv[]) {
    printf("Start Slave Full Duplex SPI\n");
	#ifdef FUN_TEST
    initLthreadBase(&MainThread);
    #else
    mutex_init_schedee(&MainThread.mx);
    lthread_init(&MainThread.base_thread, waitingRun);
    #endif

    printf("Init main thread\n");
    printf("Launch thread\n");

    printf("Run cycle for checking\n");
    while (!MarkerThread)
    {
        #ifdef FUN_TEST
        launchLthreadBase(&MainThread);
        #else
        lthread_launch(&MainThread.base_thread);
        #endif
        sleep(1);
    }
    

    printf("Programm reach end\n");
    return 0;
}

