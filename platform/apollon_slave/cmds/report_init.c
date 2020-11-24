#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>  
#include <stdint.h>

#include "base_project_defs.h"


int main(int argc, char *argv[]) {
    printf("Hello, I'm SPI SLAVE 0.1: %d\n", DEVICE_ID);
    return 0;
}