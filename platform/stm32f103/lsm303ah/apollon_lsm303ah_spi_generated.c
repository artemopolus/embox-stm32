#include "apollon_lsm303ah_spi_generated.h"
#include <embox/unit.h>
#include <kernel/printk.h>
struct apollon_lsm303ah_spi_dev {
	int spi_bus;
	struct spi_device *spi_dev;
};
struct apollon_lsm303ah_spi_dev apollon_lsm303ah_spi_dev0 = {
  .spi_bus = 0,
};
EMBOX_UNIT_INIT(apollon_lsm303ah_init);
static int apollon_lsm303ah_spi_init( struct apollon_lsm303ah_spi_dev *dev )
{
  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA7   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI1, &SPI_InitStruct);

  return 0;

}
static int apollon_lsm303ah_init(void)
{
  struct apollon_lsm303ah_spi_dev *dev = &apollon_lsm303ah_spi_dev0;
  apollon_lsm303ah_spi_init(dev);
  return 0;
}

uint8_t apollon_lsm303ah_spi_get_option(uint8_t address)
{
  uint8_t value = address | 0x80;
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	LL_SPI_TransmitData8(SPI1, value);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	LL_SPI_SetTransferDirection(SPI1,LL_SPI_HALF_DUPLEX_RX);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
	uint8_t result = 0;
	result = LL_SPI_ReceiveData8(SPI1);
	LL_SPI_SetTransferDirection(SPI1,LL_SPI_HALF_DUPLEX_TX);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
  return result;

}
uint8_t apollon_lsm303ah_spi_set_option(uint8_t address, uint8_t value)
{
  uint8_t mask = 0x7F ;//01111111b
	mask &= address;
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
  while(!LL_SPI_IsActiveFlag_TXE(SPI1));
  LL_SPI_TransmitData8(SPI1, mask);
  while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	LL_SPI_TransmitData8(SPI1, value);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	LL_GPIO_SetOutputPin(GPIOA,PinMask);
  return 0;

}
