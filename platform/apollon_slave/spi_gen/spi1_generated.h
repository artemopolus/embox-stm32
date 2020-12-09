#ifndef SPI1_GENERATED_H
#define SPI1_GENERATED_H
#include <stdint.h>
typedef enum{
    TRANSMIT = 0,
    RECEIVE
}spi_data_type_t;
typedef struct{
    uint8_t * data;
    uint8_t datalen;
    spi_data_type_t type;
}spi_pack_t;
extern uint8_t SPI1_HALF_BASE_get_option(const uint8_t address);
extern uint8_t SPI1_HALF_BASE_set_option(const uint8_t address, const uint8_t value);
#endif //SPI1_GENERATED_H
