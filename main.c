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
#include "soc.h"
#include "analog.h"
#include "rtcc.h"
#include "buzzer.h"
#include "accessory.h"
#include "faults.h"
#include "packet.h"
#include "console.h"

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
        uint16_t blinkTime = 500;
        if (comm_usb_is_active())
            blinkTime = 250;
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
        else if (power_get_power_on_event() == EVENT_RTCC)
        {
            led_rgb_set(0xFF0055);
            chThdSleepMilliseconds(50);
            led_rgb_set(0);
            chThdSleepMilliseconds(150);
        }
        else if (faults_get_faults() != FAULT_NONE)
        {
            led_rgb_set(0xFF0000);
            chThdSleepMilliseconds(200);
            led_rgb_set(0);
            chThdSleepMilliseconds(200);
        }
        else if (charger_is_balancing())
        {
            led_rgb_set(0x0000FF);
            chThdSleepMilliseconds(blinkTime);
            led_rgb_set(0);
            chThdSleepMilliseconds(blinkTime);
        }
        else if (charger_is_charging())
        {
            led_rgb_set(0xFF2200);
            chThdSleepMilliseconds(blinkTime);
            led_rgb_set(0);
            chThdSleepMilliseconds(blinkTime);
        }
        else
        {
            led_rgb_set(0x00FF00);
            chThdSleepMilliseconds(blinkTime);
            led_rgb_set(0);
            chThdSleepMilliseconds(blinkTime);
        }
    }
}
static THD_WORKING_AREA(buzzer_update_wa, 1024);
static THD_FUNCTION(buzzer_update, arg) {
    (void)arg;

    chRegSetThreadName("Buzzer update");

    /*for (int i = 100; i < 4000; i+=5)*/
    /*{*/
        /*buzzer_set_frequency(i);*/
        /*chThdSleepMilliseconds(1);*/
    /*}*/
    for(;;)
    {
        if (power_is_shutdown())
        {
            buzzer_set_frequency(0);
            break;
        }
        buzzer_set_frequency(0);
        chThdSleepMilliseconds(100);
        continue;
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
    analog_init();
    power_init();
    i2cStart(&I2C_DEV, &i2cconfig);
    ltc6803_init();
    charger_init();
    current_monitor_init();
    soc_init();
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
        analog_update();
        ltc6803_update();
        current_monitor_update();
        soc_update();
        charger_update();
        power_update();
        rtcc_update();
        accessory_update();
        comm_can_update();
        if (power_is_shutdown())
        {
            comm_usb_deinit();
            led_rgb_set(0);
            buzzer_set_frequency(0);
            break;
        }
    }
}
