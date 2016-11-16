#include "accessory.h"
#include "hw_conf.h"
#include "power.h"

void accessory_init(void)
{
    palSetPadMode(ACC1_GPIO, ACC1_PIN, PAL_MODE_OUTPUT_OPENDRAIN | PAL_STM32_OSPEED_MID1);
    palSetPad(ACC1_GPIO, ACC1_PIN);
    palSetPadMode(ACC2_GPIO, ACC2_PIN, PAL_MODE_OUTPUT_OPENDRAIN | PAL_STM32_OSPEED_MID1);
    palSetPad(ACC2_GPIO, ACC2_PIN);
}

void accessory_update(void)
{
    if (!power_is_shutdown())
    {
        palClearPad(ACC1_GPIO, ACC1_PIN);
    }
    else
    {
        palSetPad(ACC1_GPIO, ACC1_PIN);
    }
}
