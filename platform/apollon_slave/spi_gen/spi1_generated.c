#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_i2c.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
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
#include <errno.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/sync/mutex.h>
#include "spi1_generated.h"
#include "exacto_commander/exacto_data_storage.h"

#define SPI1_HALF_BUFFER_SIZE 5
typedef struct
{
    struct lthread thread;
    uint8_t data[SPI1_HALF_BUFFER_SIZE];
    uint8_t datalen;
    // struct mutex mutex;
    uint8_t datapt;
    uint8_t overflow;
    uint8_t buffer[SPI1_HALF_BUFFER_SIZE];
    uint8_t bufferlen;
} SPI1_HALF_buffer_t;

static SPI1_HALF_buffer_t TxSPI1HalfBuffer = {
    .datalen = SPI1_HALF_BUFFER_SIZE,
    .datapt = 0,
    .overflow = 0,
};
static SPI1_HALF_buffer_t RxSPI1HalfBuffer = {
    .datalen = SPI1_HALF_BUFFER_SIZE,
    .datapt = 0,
    .overflow = 0,
};

struct lthread RxSpi1HalfIrqThread;
struct lthread TxSpi1HalfIrqThread;
struct lthread RxSpi1HalfRunThread;
struct lthread TxSpi1HalfRunThread;

static irq_return_t Spi1HalfIrqHandler(unsigned int irq_nr, void *data);
static int txSpi1HalfRun(struct lthread *self);
static int rxSpi1HalfRun(struct lthread *self);
EMBOX_UNIT_INIT(SPI1_HALF_BASE_init);
static int SPI1_HALF_BASE_init(void)
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
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 10;
    LL_SPI_Init(SPI1, &SPI_InitStruct);

    lthread_init(&TxSpi1HalfIrqThread, txSpi1HalfRun);
    lthread_init(&RxSpi1HalfIrqThread, rxSpi1HalfRun);
    lthread_init(&TxSpi1HalfRunThread, updateTxRun);
    lthread_init(&RxSPI1HalfBuffer.thread, syncRxTsSpi1HalfRun);
    

//   SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt    

    irq_attach(35, Spi1HalfIrqHandler, 0, NULL, "Spi1HalfDmaIrqHandler");
    // LL_SPI_EnableIT_RXNE(SPI1);
    LL_SPI_EnableIT_TXE(SPI1);
    //enable spi and set transfer direction
    LL_SPI_Enable(SPI1);
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_HALF_DUPLEX_TX);
    return 0;
}
static irq_return_t Spi1HalfIrqHandler(unsigned int irq_nr, void *data)
{
    if(LL_SPI_IsActiveFlag_TXE(SPI1))
    {
        lthread_launch(&TxSpi1HalfIrqThread);
    }
    if(LL_SPI_IsActiveFlag_RXNE(SPI1))
    {
        lthread_launch(&RxSpi1HalfIrqThread);

    }
    return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(35, Spi1HalfIrqHandler, NULL);
uint8_t SPI1_HALF_BASE_get_option(const uint8_t address)
{
    uint8_t value = address | 0x80;
	// remember to reset CS --> LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	LL_SPI_TransmitData8(SPI1, value);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	LL_SPI_SetTransferDirection(SPI1,LL_SPI_HALF_DUPLEX_RX);
	while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
	uint8_t result = 0;
	result = LL_SPI_ReceiveData8(SPI1);
	LL_SPI_SetTransferDirection(SPI1,LL_SPI_HALF_DUPLEX_TX);
	// remember to set CS --> LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
    return result;
}
uint8_t SPI1_HALF_BASE_set_option(const uint8_t address, const uint8_t value)
{
    uint8_t mask = 0x7F ;//01111111b
	mask &= address;
    // remember to reset CS -->LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    while(!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, mask);
    while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	LL_SPI_TransmitData8(SPI1, value);
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	// remember to set CS -->LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_4);
    return 0;
}
static int txSpi1HalfRun(struct lthread *self)
{
    if (TxSPI1HalfBuffer.datapt < TxSPI1HalfBuffer.datalen)
    {
        LL_SPI_TransmitData8(SPI1, TxSPI1HalfBuffer.data[TxSPI1HalfBuffer.datapt++]);
    }
    else
    {
        LL_SPI_DisableIT_TXE(SPI1);
        LL_SPI_SetTransferDirection(SPI1, LL_SPI_HALF_DUPLEX_RX);
        LL_SPI_EnableIT_RXNE(SPI1);
    }
    return 0;
}
static int rxSpi1HalfRun(struct lthread *self)
{
    if(RxSPI1HalfBuffer.datapt < RxSPI1HalfBuffer.datalen)
    {
        RxSPI1HalfBuffer.data[RxSPI1HalfBuffer.datapt++] = LL_SPI_ReceiveData8(SPI1);
    }
    else
    {
        //nothing
    }
    
    return 0;
}
static int syncRxTsSpi1HalfRun(struct lthread * self)
{
    SPI1_HALF_buffer_t * _trg;
    _trg = (SPI1_HALF_buffer_t * ) self;
    goto *lthread_resume(self, &&start);
start:
    /* инициализация */
mutex_retry:
    if (mutex_trylock_lthread(self, &ExDtStorage.dtmutex ) == -EAGAIN)
    {
        return lthread_yield(&&start, &&mutex_retry);
    }
    if (_trg->datapt != 0)
    {
        ExDtStorage.isEmpty = 0;
    }
    mutex_unlock_lthread(self, &ExDtStorage.dtmutex);

    return 0;

}
static int updateTxRun(struct lthread *self)
{
    if(LL_SPI_IsEnabledIT_TXE(SPI1))
        LL_SPI_DisableIT_TXE(SPI1);
    if(LL_SPI_IsEnabledIT_RXNE(SPI1))
        LL_SPI_DisableIT_RXNE(SPI1);
    TxSPI1HalfBuffer.datalen = TxSPI1HalfBuffer.bufferlen;
    for (uint8_t i = 0; i < _trg->bufferlen; i++)
    {
        TxSPI1HalfBuffer.data[i] = TxSPI1HalfBuffer.buffer[i];
    }
    TxSPI1HalfBuffer.datapt = 0;
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_HALF_DUPLEX_TX);
    LL_SPI_EnableIT_TXE(SPI1);
    return 0;
}
uint8_t sendSpi1Half(uint8_t * data, uint8_t datalen)
{
    if (datalen > SPI1_HALF_BUFFER_SIZE)
        return 1;
    TxSPI1HalfBuffer.bufferlen = datalen;
    for (uint8_t i = 0; i < datalen; i++)
    {
        TxSPI1HalfBuffer.buffer[i] = data[i];
    }
    lthread_launch(&TxSPI1HalfBuffer.thread);
    return 0;
}
