#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "spi_gen/spi2_generated.h"
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/lthread/lthread.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <kernel/lthread/sync/mutex.h>

struct lthread WaitingTaskLth;

static int WaitingRun(struct lthread *self) {
    while(! SPI2_FULL_DMA_wait_rx_data())
    {

    }
    printf("Receive data: done\n");
    return 0;
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
    SPI2_FULL_DMA_transmit(data, datacount);


    printf("Transmit: done\n");


    return 0;
}
