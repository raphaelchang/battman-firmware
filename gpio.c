#include "ch.h"
#include "hal.h"
#include "hw_conf.h"

void gpio_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // WS2812B
    palSetPadMode(WS2812B_SW_GPIO, WS2812B_SW_PIN,
            PAL_MODE_ALTERNATE(GPIO_AF_TIM3) |
            PAL_STM32_OTYPE_PUSHPULL |
            PAL_STM32_OSPEED_MID1);

    // USB
    palSetPadMode(USB_DM_GPIO, USB_DM_PIN, PAL_MODE_ALTERNATE(10));
    palSetPadMode(USB_DP_GPIO, USB_DP_PIN, PAL_MODE_ALTERNATE(10));
}
