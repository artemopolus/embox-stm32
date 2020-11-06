#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "spi_gen/spi2_generated.h"
#include <kernel/lthread/lthread.h>
// #include <kernel/time/ktime.h>
#include <kernel/lthread/sync/mutex.h>

struct lthread WaitingTaskLth;

static int WaitingRun(struct lthread *self) {
    struct mutex trg_m;
    uint8_t result = 0;
    goto *lthread_resume(self, &&start);
start:
    /* инициализация */
    result = 0;
    trg_m = SPI2_FULL_DMA_wait_rx_data();
mutex_retry:
    if (mutex_trylock_lthread(self, &trg_m ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    if (SPI2_FULL_DMA_is_full())
    {
    	mutex_unlock_lthread(self, &trg_m);
        printf("Receive data: done\n");
        return result;
    }
    else
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    
}

int main(int argc, char *argv[]) {
	lthread_init(&WaitingTaskLth, WaitingRun);

    uint8_t data[] = {1, 17, 22, 4,55};
    size_t datacount = sizeof(data);
    printf("Ready to send spi dma test %d array!", datacount);
    printf("Sending data:\n");
    for (size_t i = 0; i < datacount; i++)
    {
        /* code */
        printf("dec= %d ,hex= %#04x,",data[i], data[i]);
    }
    printf("\n");

    sleep(1);
    // SPI2_FULL_DMA_transmit(data, datacount);


    printf("Transmit: done\n");


    return 0;
}

