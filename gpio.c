#include "ch.h"
#include "hal.h"
#include "hw_conf.h"

void gpio_init(void)
{
    // ENABLE GPIO MODULES
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

    //CHARGING CIRCUIT
    palSetPadMode(CHRG_STAT_1_GPIO, CHRG_STAT_1_PIN, PAL_MODE_INPUT_PULLUP);

    // WS2812B
    //palSetPadMode(WS2812_SW_GPIO, WS2812_SW_PIN,
    //        PAL_MODE_ALTERNATE(GPIO_AF_TIM3) |
    //        PAL_STM32_OTYPE_PUSHPULL |
    //        PAL_STM32_OSPEED_MID1);

    // USB
    palSetPadMode(USB_DM_GPIO, USB_DM_PIN, PAL_MODE_ALTERNATE(10));
    palSetPadMode(USB_DP_GPIO, USB_DP_PIN, PAL_MODE_ALTERNATE(10));
}
