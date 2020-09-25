#ifndef SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_
#define SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_usart.h"
#include "stm32f1xx_hal_uart.h"

#include <hal/reg.h>

#include <assert.h>
#include <framework/mod/options.h>
#include <module/embox/driver/serial/stm_usart_f1.h>

#define MODOPS_USARTX OPTION_GET(NUMBER, usartx)
#define USART1_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f1, NUMBER, usart1_irq)
static_assert(USART1_IRQ == USART1_IRQn);
#define USART2_IRQ    \
	OPTION_MODULE_GET(embox__driver__serial__stm_usart_f1, NUMBER, usart2_irq)
static_assert(USART2_IRQ == USART2_IRQn);


#if MODOPS_USARTX == 1

#define USARTx                           USART1
/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQ

#elif MODOPS_USARTX == 2
#define USARTx                           USART2
#define USARTx_IRQn                      USART2_IRQ
#else
#error Unsupported USARTx
#endif

#define STM32_USART1_ENABLED             1
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART1_TX_PIN                    GPIO_PIN_6
#define USART1_TX_GPIO_PORT              GPIOB
#define USART1_TX_AF                     GPIO_AF7_USART2
#define USART1_RX_PIN                    GPIO_PIN_7
#define USART1_RX_GPIO_PORT              GPIOB
#define USART1_RX_AF                     GPIO_AF7_USART2

#define STM32_USART2_ENABLED             1
#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_AF                     GPIO_AF7_USART2
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_AF                     GPIO_AF7_USART2
// this is just for stm32f103 	 |
//								 |
//							 	 V
#define STM32_USART_FLAGS(uart)   (REG_LOAD(&uart->SR))
#define STM32_USART_RXDATA(uart)  uart->DR
#define STM32_USART_TXDATA(uart)  uart->DR
// #define STM32_USART_ICR(uart)     uart->ICR




#endif //SRC_DRIVERS_SERIAL_STM32_USART_STM32_USART_CONF_F1_H_