
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"

#include "apollon_gpio_generated.h"
#include <embox/unit.h>
#include <kernel/printk.h>

#define GPIO_PORTx      GPIOA
#define GPIO_PINx       LL_GPIO_PIN_10
#define GPIO_CLOCKx     LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)

EMBOX_UNIT_INIT(apollon_gpio_init);
static int apollon_gpio_init(void)
{
    GPIO_CLOCKx;
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
}
void toggle_gpio_pin(void)
{
    LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_10);
}
