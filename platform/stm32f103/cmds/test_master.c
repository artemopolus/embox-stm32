#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "spi/apollon_master_spi2_generated.h"

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

    apollon_master_spi2_transmit_array(data, datacount);
    
    printf("Done\n");
    return 0;
}