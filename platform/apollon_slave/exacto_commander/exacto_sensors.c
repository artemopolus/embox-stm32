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

#include "spi_gen/spi1_generated.h"
#include "exacto_commander/exacto_sensors.h"
#include <embox/unit.h>

EMBOX_UNIT_INIT(initSensor);
static int initSensors(void)
{
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    return 0;
}

void enableExactoSensor(exacto_sensors_list_t sensor)
{
    switch (sensor)
    {
    case LSM303AH:
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
        break;
    default:
        break;
    }
}
void disableExactoSensor(exacto_sensors_list_t sensor)
{
    switch (sensor)
    {
    case LSM303AH:
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
        break;
    default:
        break;
    }

}
