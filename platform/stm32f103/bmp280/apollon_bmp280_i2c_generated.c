// #include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

#include "apollon_bmp280_i2c_generated.h"
#include <embox/unit.h>
#include <kernel/printk.h>


#define BMP280_I2C_ADDR_PRIM                 UINT8_C(0x76)
#define BMP280_I2C_ADDR_SEC                  UINT8_C(0x77)
#define BMP280_I2c_ADDR     BMP280_I2C_ADDR_SEC
#define I2Cx I2C2
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01

EMBOX_UNIT_INIT(apollon_bmp280_init);
static int apollon_bmp280_init(void)
{
  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  /**I2C2 GPIO Configuration
  PB10   ------> I2C2_SCL
  PB11   ------> I2C2_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 100000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C2, 0);

    LL_I2C_Enable(I2C2);

  return 0;
}
uint8_t apollon_bmp280_spi_get_option(const uint8_t address)
{
    	/* ST ; SAD + W */
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_GenerateStartCondition(I2Cx);
	while (!LL_I2C_IsActiveFlag_SB(I2Cx));
	uint8_t trg = (BMP280_I2c_ADDR<<1);
	LL_I2C_TransmitData8(I2Cx, trg  | I2C_REQUEST_WRITE);
	/* wait SAK */
	while(!LL_I2C_IsActiveFlag_ADDR(I2Cx));
	/* SUB */
	LL_I2C_ClearFlag_ADDR(I2Cx);
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	LL_I2C_TransmitData8(I2Cx, address);
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
	/* wait SAK */
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	// while(!LL_I2C_IsActiveFlag_ADDR(I2Cx));
	/* SR ; SAD + R */
	LL_I2C_GenerateStartCondition(I2Cx);
	while (!LL_I2C_IsActiveFlag_SB(I2Cx));
	LL_I2C_TransmitData8(I2Cx, trg  | I2C_REQUEST_READ);
	/* wait SAK + DATA*/
	// while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	while(!LL_I2C_IsActiveFlag_ADDR(I2Cx));
	LL_I2C_ClearFlag_ADDR(I2Cx);
	while (!LL_I2C_IsActiveFlag_RXNE(I2Cx));
	uint8_t res = LL_I2C_ReceiveData8(I2Cx);
	while (!LL_I2C_IsActiveFlag_RXNE(I2Cx));
	/* NMAK + SP */
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
	LL_I2C_GenerateStopCondition(I2Cx);
	return res;
}
uint8_t apollon_bmp280_spi_set_option(const uint8_t address, const uint8_t value)
{
    // MODIFY_REG(I2C2->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	// SET_BIT(I2C2->CR1, I2C_CR1_START);
	LL_I2C_GenerateStartCondition(I2Cx);
	// check_I2C_SB_bmp280();
	while (!LL_I2C_IsActiveFlag_SB(I2Cx));
	uint8_t trg = (BMP280_I2c_ADDR<<1);
	// MODIFY_REG(I2C2->DR, I2C_DR_DR, (trg  | I2C_REQUEST_WRITE));
	LL_I2C_TransmitData8(I2Cx, trg  | I2C_REQUEST_WRITE);
	// check_I2C_ADDR_bmp280();
	while(!LL_I2C_IsActiveFlag_ADDR(I2Cx));
	LL_I2C_ClearFlag_ADDR(I2Cx);
    
	// clearFlag_I2C_ADDR_bmp280();
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
	// check_I2C_TXE_bmp280();
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
	// MODIFY_REG(I2C2->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
	LL_I2C_TransmitData8(I2Cx, address);
	// MODIFY_REG(I2C2->DR, I2C_DR_DR, reg);
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
	// check_I2C_TXE_bmp280();
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_ACK);
    // MODIFY_REG(I2C2->CR1, I2C_CR1_ACK, I2C_CR1_ACK);
	LL_I2C_TransmitData8(I2Cx, value);
	// MODIFY_REG(I2C2->DR, I2C_DR_DR, value);
	while (!LL_I2C_IsActiveFlag_TXE(I2Cx));
    // check_I2C_TXE_bmp280();
    
	// MODIFY_REG(I2C2->CR1, I2C_CR1_ACK, 0x00000000U);
	LL_I2C_AcknowledgeNextData(I2Cx, LL_I2C_NACK);
	// SET_BIT(I2C2->CR1, I2C_CR1_STOP);
	LL_I2C_GenerateStopCondition(I2Cx);
    return 0;
}


