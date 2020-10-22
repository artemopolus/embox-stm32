#include <errno.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"

#include "apollon_spi2_generated.h"

#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>

// uint8_t aRxBuffer[sizeof(aTxBuffer)];

#define RXTX_BUFFER_SIZE 5

typedef struct{
    uint8_t dt_buffer[RXTX_BUFFER_SIZE];
    uint8_t dt_count;
    struct mutex dt_mutex;
    struct lthread dt_lth;
}spi_buffer;

static spi_buffer spi_rx_buffer = {
    .dt_count = RXTX_BUFFER_SIZE,
};
static spi_buffer spi_tx_buffer = {
    .dt_count = RXTX_BUFFER_SIZE,
};

static irq_return_t dma_tx_irq_handler(unsigned int irq_nr, void *data);
static irq_return_t dma_rx_irq_handler(unsigned int irq_nr, void *data);

static int spi2_receive_handler(struct lthread *self)
{
    /* обработка полученных данных */
    return 0;
}
static int spi2_transmit_handler(struct lthread *self)
{
    /* действия после отправки данных */
    return 0;
}
EMBOX_UNIT_INIT(apollon_spi2_init);
static int apollon_spi2_init(void)
{
    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI
  */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI2 DMA Init */

    /* SPI2_RX Init */
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_LOW);

    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_NORMAL);

    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);

    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);

    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_BYTE);

    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_BYTE);
    
    LL_DMA_ConfigAddresses(DMA1,
                         LL_DMA_CHANNEL_4,
                         LL_SPI_DMA_GetRegAddr(SPI2), (uint32_t)spi_rx_buffer.dt_buffer,
                         LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, spi_rx_buffer.dt_count);

    /* SPI2_TX Init */
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PRIORITY_LOW);

    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MODE_NORMAL);

    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PERIPH_NOINCREMENT);

    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_INCREMENT);

    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PDATAALIGN_BYTE);

    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MDATAALIGN_BYTE);

    LL_DMA_ConfigAddresses(DMA1, 
                        LL_DMA_CHANNEL_5, (uint32_t)spi_tx_buffer.dt_buffer, 
                        LL_SPI_DMA_GetRegAddr(SPI2),
                        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, spi_tx_buffer.dt_count);
    /* SPI2 interrupt Init */
    // NVIC_SetPriority(SPI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    // NVIC_EnableIRQ(SPI2_IRQn);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI2, &SPI_InitStruct);

    int res = 0;
    //    DMA1_Channel4_IRQ  DMA1_Channel4_IRQn          = 14,     /*!< DMA1 Channel 4 global Interrupt                      */
    //  DMA1_Channel5_IRQn          = 15,     /*!< DMA1 Channel 5 global Interrupt  n

    res = irq_attach(15, dma_tx_irq_handler, 0, NULL, "tim_irq_handler");
    res = irq_attach(14, dma_rx_irq_handler, 0, NULL, "tim_irq_handler");

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_INPUT);
    lthread_init(&spi_tx_buffer.dt_lth, &spi2_transmit_handler);
    lthread_init(&spi_rx_buffer.dt_lth, &spi2_receive_handler);

    LL_SPI_EnableDMAReq_RX(SPI2);
    LL_SPI_EnableDMAReq_TX(SPI2);

    LL_SPI_Enable(SPI2);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    res = 0;
    return res;
}
static irq_return_t dma_tx_irq_handler(unsigned int irq_nr, void *data)
{
    if (LL_DMA_IsActiveFlag_TC5(DMA1) != RESET)
    {
        LL_DMA_ClearFlag_GI5(DMA1);
        //
        lthread_launch(&spi_tx_buffer.dt_lth);
    }
    return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(15, dma_tx_irq_handler, NULL);
static irq_return_t dma_rx_irq_handler(unsigned int irq_nr, void *data)
{
    if (LL_DMA_IsActiveFlag_TC4(DMA1) != RESET)
    {
        LL_DMA_ClearFlag_GI4(DMA1);
        // spi2MLine_TransmissionEndHandle();
        lthread_launch(&spi_rx_buffer.dt_lth);
    }
    return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(14, dma_rx_irq_handler, NULL);
