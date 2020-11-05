#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "spi/spi2_generated.h"

int main(int argc, char *argv[]) {

    uint8_t data[] = {1, 17, 22, 4,55};
    size_t datacount = sizeof(data);
    printf('Ready to send spi dma test array!');
    return 0;
}
