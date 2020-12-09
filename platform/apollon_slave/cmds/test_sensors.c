#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
  
#include <stdint.h>
#include "exacto_commander/exacto_data_storage.h"
#include "spi_gen/spi1_generated.h"


int main(int argc, char *argv[]) {

    const uint8_t lsm303ah_3wire_addr = 0x21&0x7F;
    const uint8_t lsm303ah_3wire_val = 0x05;
    const uint8_t lsm303ah_whoami_xl_addr = 0x0f;
    const uint8_t lsm303ah_whoami_xl_val = 0x43;


    return 0;
}
