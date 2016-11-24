#ifndef _STM32F30X_CONF_H_
#define _STM32F30X_CONF_H_

#define USE_RTOS	0

#include "stm32f30x_flash.h"
#include "stm32f30x_tim.h"
#include "stm32f30x_dma.h"
#include "stm32f30x_rcc.h"

#ifdef  USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif

#endif /* _STM32F30X_CONF_H_ */
