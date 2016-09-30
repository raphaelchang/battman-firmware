#include "ch.h"
#include "hal.h"
#include "gpio.h"
#include "usbcfg.h"
#include "hw_conf.h"

#define usb_lld_connect_bus(usbp)
#define usb_lld_disconnect_bus(usbp)

int main(void) {

    halInit();
    chSysInit();
    gpio_init();

    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp)

    while(1)
    {
    palClearPad(WS2812_SW_GPIO, WS2812_SW_PIN);
    chThdSleepMilliseconds(100);
    palSetPad(WS2812_SW_GPIO, WS2812_SW_PIN);
    chThdSleepMilliseconds(100);
    }

}
