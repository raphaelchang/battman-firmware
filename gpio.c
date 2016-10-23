#include "ch.h"
#include "hal.h"
#include "hw_conf.h"

void gpio_init(void)
{
    //GPIO
    palSetPadMode(PCHG_SW_GPIO, PCHG_SW_PIN, PAL_MODE_OUTPUT_PUSHPULL |
                        PAL_STM32_OSPEED_HIGHEST);
    palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
    
    palSetPadMode(PWR_BTN_GPIO, PWR_BTN_PIN, PAL_MODE_INPUT);
    
    palSetPadMode(PWR_SW_GPIO, PWR_SW_PIN, PAL_MODE_OUTPUT_PUSHPULL |
                        PAL_STM32_OSPEED_HIGHEST);
    palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
    
    palSetPadMode(DSG_SW_GPIO, DSG_SW_PIN, PAL_MODE_OUTPUT_PUSHPULL |
                        PAL_STM32_OSPEED_HIGHEST);
    palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
    
    palSetPadMode(CHG_DETECT_GPIO, CHG_DETECT_PIN, PAL_MODE_INPUT);

    // SPI
    palSetPadMode(LTC6803_CS_GPIO, LTC6803_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL |
                        PAL_STM32_OSPEED_HIGHEST);

    palClearPad(LTC6803_CS_GPIO, LTC6803_CS_PIN);
    palSetPadMode(SCK_GPIO, SCK_PIN, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(MISO_GPIO, MISO_PIN, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(MOSI_GPIO, MOSI_PIN, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

    // RGB LED
    palSetPadMode(LED_R_GPIO, LED_R_PIN, PAL_MODE_ALTERNATE(1));
    palSetPadMode(LED_G_GPIO, LED_G_PIN, PAL_MODE_ALTERNATE(1));
    palSetPadMode(LED_B_GPIO, LED_B_PIN, PAL_MODE_ALTERNATE(1));
    
    // USB
    palSetPadMode(USB_DM_GPIO, USB_DM_PIN, PAL_MODE_ALTERNATE(14));
    palSetPadMode(USB_DP_GPIO, USB_DP_PIN, PAL_MODE_ALTERNATE(14));
}
