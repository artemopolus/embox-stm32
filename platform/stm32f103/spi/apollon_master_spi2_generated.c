#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

#include "apollo_master_spi2_generated.h"

#define CS_PORT GPIOB
#define CS_PIN LL_GPIO_PIN_12

#include <embox/unit.h>

static int apollon_master_spi2_enable(void)
{
    LL_GPIO_ResetOutputPin(CS_PORT, CS_PIN);
    return 0;
}
static int apollon_master_spi2_disable(void)
{
    LL_GPIO_SetOutputPin(CS_PORT, CS_PIN);
    return 0;
}
EMBOX_UNIT_INIT(apollon_master_spi2_init);
static int apollon_master_spi2_init(void)
{
    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    /**SPI2 GPIO Configuration
     * PB13   ------> SPI2_SCK
     * PB14   ------> SPI2_MISO
     * PB15   ------> SPI2_MOSI
     * */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /** CS pin */
    LL_GPIO_SetPinMode(CS_PORT, CS_PIN, LL_GPIO_MODE_OUTPUT);

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI2, &SPI_InitStruct);
    LL_SPI_Enable(SPI2);
    apollon_master_spi2_disable();
}
uint8_t apollon_master_spi2_transmit_receive(uint8_t address)
{
    return 0;
}
uint8_t apollon_master_spi2_transmit_array(uint8_t *data, uint8_t datacount)
{
    apollon_master_spi2_enable();
    while (!LL_SPI_IsActiveFlag_TXE(SPI2));
    for (uint8_t i = 0; i < datacount; i++)
    {
        /* последовательно отправляем данные */
        LL_SPI_TransmitData8(SPI1, data[i]);
        while (!LL_SPI_IsActiveFlag_TXE(SPI2));
    }
    while (LL_SPI_IsActiveFlag_BSY(SPI2));
    apollon_master_spi2_disable();
    return 0;
}
