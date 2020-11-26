#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "exacto_commander/exacto_data_storage.h"

// #include "spi_gen/spi2_generated.h"
// #include <kernel/lthread/lthread.h>
// // #include <kernel/time/ktime.h>
// #include <kernel/lthread/sync/mutex.h>

// struct lthread WaitingTaskLth;

// static int WaitingRun(struct lthread *self) {
//     struct mutex trg_m;
//     uint8_t result = 0;
//     goto *lthread_resume(self, &&start);
// start:
//     /* инициализация */
//     result = 0;
//     trg_m = SPI2_FULL_DMA_wait_rx_data();
// mutex_retry:
//     if (mutex_trylock_lthread(self, &trg_m ) == -EAGAIN)
//     {
//         return lthread_yield(&&start, &&mutex_retry);
//     }
//     if (SPI2_FULL_DMA_is_full())
//     {
//     	mutex_unlock_lthread(self, &trg_m);
//         printf("Receive data: done\n");
//         return result;
//     }
//     else
//     {
//         return lthread_yield(&&start, &&mutex_retry);
//     }
    
// }

int main(int argc, char *argv[]) {
    printf("Start Master Full Duplex SPI\n");
    printf("Ready to receive data!\n");

    while(!checkExactoDataStorage())
    {
        sleep(1);
    }
    printf("Receive data: done\n");

    return 0;
}

