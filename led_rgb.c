#include "led_rgb.h"
#include "ch.h"
#include "hal.h"
#include "hw_conf.h"

static PWMConfig pwmcfg = {
    512000,
    256,
    NULL,
    {
        {PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_ACTIVE_LOW, NULL},
        {PWM_OUTPUT_ACTIVE_LOW, NULL},
        {PWM_OUTPUT_ACTIVE_LOW, NULL}
    },
    0,
    0
};

void led_rgb_init(void)
{
    pwmStart(&LED_PWM_DEV, &pwmcfg);
    pwmEnableChannel(&LED_PWM_DEV, LED_R_CHANNEL, 0);
    pwmEnableChannel(&LED_PWM_DEV, LED_G_CHANNEL, 0);
    pwmEnableChannel(&LED_PWM_DEV, LED_B_CHANNEL, 0);
}
void led_rgb_set(uint32_t color)
{
    pwmEnableChannel(&LED_PWM_DEV, LED_B_CHANNEL, color & 0xFF);
    color = color >> 8;
    pwmEnableChannel(&LED_PWM_DEV, LED_G_CHANNEL, color & 0xFF);
    color = color >> 8;
    pwmEnableChannel(&LED_PWM_DEV, LED_R_CHANNEL, color & 0xFF);
}
