#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

#include "apollon_ism330dlc_spi_generated.h"
#include <embox/unit.h>
#include <kernel/printk.h>

#include <drivers/gpio/gpio.h>

#define ISM330DLC_CS_Pin LL_GPIO_PIN_0
#define ISM330DLC_CS_GPIO_Port GPIOB
#define ISM330DLC_CLC LL_APB2_GRP1_PERIPH_GPIOB
#define ISM330DLC_SPI SPI1

struct apollon_ism330dlc_spi_dev {
	int spi_bus;
	struct spi_device *spi_dev;
};

struct apollon_ism330dlc_spi_dev apollon_ism330dlc_spi_dev0 = {
  .spi_bus = 0,
};

EMBOX_UNIT_INIT(apollon_ism330dlc_init);
static int apollon_ism330dlc_spi_init( struct apollon_ism330dlc_spi_dev *dev )
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
  	LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_ResetOutputPin(ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);
  	LL_GPIO_SetPinMode(       ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);


    return 0;
}
static int apollon_ism330dlc_init(void)
{
  struct apollon_ism330dlc_spi_dev *dev = &apollon_ism330dlc_spi_dev0;
  apollon_ism330dlc_spi_init(dev);
  return 0;
}
uint8_t apollon_ism330dlc_spi_get_option(uint8_t address)
{
    uint8_t value = address | 0x80; //10000000b
	LL_GPIO_ResetOutputPin(		ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);
	while(!LL_SPI_IsActiveFlag_TXE( ISM330DLC_SPI));
	LL_SPI_TransmitData8(           ISM330DLC_SPI, value);
	while(!LL_SPI_IsActiveFlag_TXE( ISM330DLC_SPI));
	while(LL_SPI_IsActiveFlag_BSY(  ISM330DLC_SPI));
	LL_SPI_SetTransferDirection(    ISM330DLC_SPI,LL_SPI_HALF_DUPLEX_RX);
	while(!LL_SPI_IsActiveFlag_RXNE(ISM330DLC_SPI));
	uint8_t result = 0;
	result = LL_SPI_ReceiveData8(   ISM330DLC_SPI);
	LL_SPI_SetTransferDirection(    ISM330DLC_SPI,LL_SPI_HALF_DUPLEX_TX);
	LL_GPIO_SetOutputPin(ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);
    return result;
}
uint8_t apollon_ism330dlc_spi_set_option(uint8_t address, uint8_t value)
{
    uint8_t mask = 0x7F ;//01111111b
	mask &= address;
    LL_GPIO_ResetOutputPin(ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);
    while(!LL_SPI_IsActiveFlag_TXE( ISM330DLC_SPI));
    LL_SPI_TransmitData8(           ISM330DLC_SPI, mask);
    while(!LL_SPI_IsActiveFlag_TXE( ISM330DLC_SPI));
	LL_SPI_TransmitData8(           ISM330DLC_SPI, value);
	while(!LL_SPI_IsActiveFlag_TXE( ISM330DLC_SPI));
	while(LL_SPI_IsActiveFlag_BSY(  ISM330DLC_SPI));
	LL_GPIO_SetOutputPin(ISM330DLC_CS_GPIO_Port, ISM330DLC_CS_Pin);
    return 0;
}
