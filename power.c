#include "power.h"
#include "hw_conf.h"
#include "config.h"

static volatile bool discharge_enabled = false;
static volatile bool precharged = false;
static volatile systime_t prechargeStartTime;
static volatile PowerOnEvent power_on_event;
static volatile bool power_button_released = false;
static volatile bool shutdownStarted = false;
static volatile bool shutdown = false;
static volatile systime_t shutdownStartTime;
static volatile Config *config;

void power_init(void)
{
    config = config_get_configuration();
    palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
    if (!palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN))
        power_on_event = EVENT_SWITCH;
    else if (palReadPad(CHG_SENSE_GPIO, CHG_SENSE_PIN))
        power_on_event = EVENT_CHARGER;
#ifdef BATTMAN_4_1
    else if (!palReadPad(RTCC_INT_GPIO, RTCC_INT_PIN))
        power_on_event = EVENT_RTCC;
#endif
    else
        power_on_event = EVENT_USB;
    if (power_on_event == EVENT_SWITCH)
    {
        chThdSleepMilliseconds(config->turnOnDelay);
        discharge_enabled = true;
    }
    palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
}

void power_update(void)
{
    if (!shutdown)
        palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
    if (discharge_enabled)
    {
        if (!precharged)
        {
            prechargeStartTime = chVTGetSystemTime();
            precharged = true;
        }
        if (precharged)
        {
            if (ST2MS(chVTTimeElapsedSinceX(prechargeStartTime)) > 1000)
            {
                palSetPad(DSG_SW_GPIO, DSG_SW_PIN);
                palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
            }
            else
            {
                palSetPad(PCHG_SW_GPIO, PCHG_SW_PIN);
                palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
            }
        }
        else
        {
            palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
            palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
        }
    }
    else
    {
        palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
        palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
    }
    if (palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN))
        power_button_released = true;
    if ((!palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN) && power_button_released))
    {
        if (!shutdownStarted)
        {
            shutdownStartTime = chVTGetSystemTime();
            shutdownStarted = true;
        }
        if (ST2MS(chVTTimeElapsedSinceX(shutdownStartTime)) > config->shutdownDelay)
        {
            palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
            palClearPad(PWR_SW_GPIO, PWR_SW_PIN);
            shutdown = true;
        }
    }
    else if (shutdown)
    {
        palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
        palClearPad(PWR_SW_GPIO, PWR_SW_PIN);
    }
    else
    {
        shutdown = false;
        shutdownStarted = false;
    }
    // Check for faults
}

void power_enable_discharge(void)
{
    discharge_enabled = true;
}

void power_disable_discharge(void)
{
    discharge_enabled = false;
    palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
}

bool power_is_shutdown(void)
{
    return shutdown;
}

PowerOnEvent power_get_power_on_event(void)
{
    return power_on_event;
}
