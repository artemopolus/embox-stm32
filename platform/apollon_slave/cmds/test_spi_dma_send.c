#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "exacto_commander/exacto_data_storage.h"

static thread_control_t MainThread;

uint8_t MarkerThread = 0;


static int waitingRun(struct lthread *self) {
    thread_control_t * _trg_th;
    goto *lthread_resume(self, &&start);
start:
    /* инициализация */
    _trg_th = (thread_control_t *)&self;
mutex_retry:
    if (mutex_trylock_lthread(self, &_trg_th->mx ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    if (_trg_th->result == OK)
    {
        MarkerThread = 1;
        mutex_unlock_lthread(self, &_trg_th->mx);
        return 0;
    }
    else
    {
        mutex_unlock_lthread(self, &_trg_th->mx);
        return lthread_yield(&&start, &&mutex_retry);
    }
    
   
}

int main(int argc, char *argv[]) {
    printf("Start Slave Full Duplex SPI\n");
    
    initThreadExactoDataStorage(&MainThread);

    lthread_init(&MainThread.base_thread, waitingRun);
    lthread_launch(&MainThread.base_thread);

    while (!MarkerThread)
    {
        checkExactoDataStorage(&MainThread);
        sleep(1);
    }
    

    printf("Programm reach end\n");
    return 0;
}

