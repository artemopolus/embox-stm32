#ifndef APOLLON_ISM330DLC_SPI_GENERATED_H
#define APOLLON_ISM330DLC_SPI_GENERATED_H
#include <stdint.h>

#define ISM330DLC_CS_Pin GPIO_PIN_0
#define ISM330DLC_CS_GPIO_Port GPIOB
#define ISM330DLC_CLC LL_APB2_GRP1_PERIPH_GPIOB
#define ISM330DLC_SPI SPI1

extern uint8_t apollon_ism330dlc_spi_get_option(uint8_t address);
extern uint8_t apollon_ism330dlc_spi_set_option(uint8_t address, uint8_t value);

#endif
