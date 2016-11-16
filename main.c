#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "gpio.h"
#include "comm_usb.h"
#include "comm_can.h"
#include "hw_conf.h"
#include "led_rgb.h"
#include "ltc6803.h"
#include "charger.h"
#include "power.h"
#include "config.h"
#include "current_monitor.h"
#include "adc.h"
#include "rtcc.h"
#include "buzzer.h"
#include "packet.h"
#include "console.h"
#include "accessory.h"

static const I2CConfig i2cconfig = {
    STM32_TIMINGR_PRESC(15U) |
        STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
        STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(21U),
    0,
    0
};

static THD_WORKING_AREA(led_update_wa, 1024);
static THD_FUNCTION(led_update, arg) {
    (void)arg;

    chRegSetThreadName("LED update");

    for(;;)
    {
        if (power_is_shutdown())
        {
            led_rgb_set(0);
            break;
        }
        if (packet_connect_event())
        {
            led_rgb_set(0x00FFFF);
            chThdSleepMilliseconds(100);
            led_rgb_set(0);
            chThdSleepMilliseconds(100);
            led_rgb_set(0x00FFFF);
            chThdSleepMilliseconds(100);
            led_rgb_set(0);
            chThdSleepMilliseconds(100);
            led_rgb_set(0x00FFFF);
            chThdSleepMilliseconds(100);
            led_rgb_set(0);
            chThdSleepMilliseconds(100);
        }
        else if (comm_usb_is_active())
        {
            led_rgb_set(0x00FF00);
            chThdSleepMilliseconds(250);
            led_rgb_set(0);
            chThdSleepMilliseconds(250);
        }
        else
        {
            led_rgb_set(0x00FF00);
            chThdSleepMilliseconds(500);
            led_rgb_set(0);
            chThdSleepMilliseconds(500);
        }
    }
}
static THD_WORKING_AREA(buzzer_update_wa, 1024);
static THD_FUNCTION(buzzer_update, arg) {
    (void)arg;

    chRegSetThreadName("Buzzer update");

    for(;;)
    {
        for (int i = 100; i < 4000; i+=5)
        {
            buzzer_set_frequency(i);
            chThdSleepMilliseconds(1);
        }
        /*buzzer_play_rest(200);*/
        /*buzzer_play_note(NOTE_G4, 290);*/
        /*buzzer_play_rest(10);*/
        /*buzzer_play_note(NOTE_G4, 100);*/
        /*buzzer_play_note(NOTE_A4, 400);*/
        /*buzzer_play_note(NOTE_G4, 400);*/
        /*buzzer_play_note(NOTE_C5, 400);*/
        /*buzzer_play_note(NOTE_B4, 800);*/

        /*buzzer_play_note(NOTE_G4, 290);*/
        /*buzzer_play_rest(10);*/
        /*buzzer_play_note(NOTE_G4, 100);*/
        /*buzzer_play_note(NOTE_A4, 400);*/
        /*buzzer_play_note(NOTE_G4, 400);*/
        /*buzzer_play_note(NOTE_D5, 400);*/
        /*buzzer_play_note(NOTE_C5, 800);*/

        /*buzzer_play_note(NOTE_G4, 290);*/
        /*buzzer_play_rest(10);*/
        /*buzzer_play_note(NOTE_G4, 100);*/
        /*buzzer_play_note(NOTE_G5, 400);*/
        /*buzzer_play_note(NOTE_E5, 400);*/
        /*buzzer_play_note(NOTE_C5, 400);*/
        /*buzzer_play_note(NOTE_B4, 400);*/
        /*buzzer_play_note(NOTE_A4, 800);*/

        /*buzzer_play_note(NOTE_F5, 290);*/
        /*buzzer_play_rest(10);*/
        /*buzzer_play_note(NOTE_F5, 100);*/
        /*buzzer_play_note(NOTE_E5, 400);*/
        /*buzzer_play_note(NOTE_C5, 400);*/
        /*buzzer_play_note(NOTE_D5, 400);*/
        /*buzzer_play_note(NOTE_C5, 800);*/
    }
}

int main(void) {

    halInit();
    chSysInit();
    gpio_init();
    chThdSleepMilliseconds(1);
    config_init();
    power_init();
    adc_init();
    i2cStart(&I2C_DEV, &i2cconfig);
    ltc6803_init();
    charger_init();
    current_monitor_init();
    rtcc_init();
    accessory_init();
    comm_can_init();
    buzzer_init();
    chThdCreateStatic(buzzer_update_wa, sizeof(buzzer_update_wa), NORMALPRIO, buzzer_update, NULL);
    led_rgb_init();
    chThdCreateStatic(led_update_wa, sizeof(led_update_wa), NORMALPRIO, led_update, NULL);
    comm_usb_init();

    while(1)
    {
        adc_update();
        ltc6803_update();
        charger_update();
        current_monitor_update();
        power_update();
        rtcc_update();
        accessory_update();
        comm_can_update();
        if (power_is_shutdown())
        {
            led_rgb_set(0);
            break;
        }
    }
}
