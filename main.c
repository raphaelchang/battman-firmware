#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "gpio.h"
#include "usbcfg.h"
#include "hw_conf.h"
#include "chprintf.h"
#include "led_rgb.h"

#define usb_lld_connect_bus(usbp)
#define usb_lld_disconnect_bus(usbp)

int main(void) {

    halInit();
    chSysInit();
    gpio_init();
    led_rgb_init();
    
    //start USB
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp)

    while(1)
    {
        led_rgb_set(0x00FF00);
        chThdSleepMilliseconds(100);
        led_rgb_set(0x000000);
        chThdSleepMilliseconds(100);
        chprintf((BaseSequentialStream *)&SDU1, "%s\n", "I am vengeance, I am the night, I am Battman.");
    }

}
