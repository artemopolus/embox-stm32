#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "spi/spi2_fullduplex_master.h"
int main(int argc, char *argv[]) {

    uint8_t data[] = {1, 17, 22, 4,55};
    size_t datacount = sizeof(data);
    printf("Send to slave data:\n");
    for (size_t i = 0; i < datacount; i++)
    {
        /* code */
        printf(" %#04x,",data[i]);
    }
    printf("\n");

    spi2_ms_transmit(data, datacount);
    
    printf("Done\n");
    return 0;
}
