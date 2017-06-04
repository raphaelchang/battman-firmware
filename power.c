#include "power.h"
#include "hw_conf.h"
#include "analog.h"
#include "current_monitor.h"
#include "config.h"
#include "charger.h"
#include "rtcc.h"
#include "faults.h"

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
    palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
    if (!palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN))
        power_on_event = EVENT_SWITCH;
    else if (analog_charger_input_voltage() > 6.0)
        power_on_event = EVENT_CHARGER;
#if defined(BATTMAN_4_1) || defined(BATTMAN_4_2)
    else if (!palReadPad(RTCC_INT_GPIO, RTCC_INT_PIN))
        power_on_event = EVENT_RTCC;
#endif
#if defined(BATTMAN_4_2)
    else if (palReadPad(USB_DETECT_GPIO, USB_DETECT_PIN))
    {
        power_on_event = EVENT_USB;
    }
#endif
    if (power_on_event == EVENT_SWITCH)
    {
        chThdSleepMilliseconds(config->turnOnDelay);
        discharge_enabled = true;
    }
    if (power_on_event != EVENT_USB)
        palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
}

void power_update(void)
{
    if (!shutdown && power_on_event != EVENT_USB)
        palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
    if (discharge_enabled && faults_get_faults() == FAULT_NONE)
    {
        if (!precharged)
        {
            prechargeStartTime = chVTGetSystemTime();
            precharged = true;
        }
        if (precharged)
        {
            if (ST2MS(chVTTimeElapsedSinceX(prechargeStartTime)) > 8)
            {
                if (analog_discharge_voltage() < 0.5) // Turned on into short
                {
                    palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
                    palClearPad(PCHG_SW_GPIO, PCHG_SW_PIN);
                    faults_set_fault(FAULT_TURN_ON_SHORT);
                    return;
                }
            }
            if (analog_discharge_voltage() >= current_monitor_get_bus_voltage() - 5.0 || ST2MS(chVTTimeElapsedSinceX(prechargeStartTime)) > config->prechargeTimeout)
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
        precharged = false;
    }
    if (palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN))
        power_button_released = true;
    if (shutdown || (config->chargerDisconnectShutdown && power_on_event == EVENT_CHARGER && analog_charger_input_voltage() < 6.0))
    {
        shutdown = true;
        rtcc_enable_alarm();
        palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
        palClearPad(PWR_SW_GPIO, PWR_SW_PIN);
    }
    else if (((!palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN) && power_button_released) && analog_charger_input_voltage() < 6.0 && !palReadPad(USB_DETECT_GPIO, USB_DETECT_PIN)) || power_on_event == EVENT_RTCC)
    {
        if (!shutdownStarted)
        {
            shutdownStartTime = chVTGetSystemTime();
            shutdownStarted = true;
        }
        if (ST2MS(chVTTimeElapsedSinceX(shutdownStartTime)) > config->shutdownDelay)
        {
            rtcc_enable_alarm();
            palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
            palClearPad(PWR_SW_GPIO, PWR_SW_PIN);
            shutdown = true;
        }
    }
    else
    {
        shutdown = false;
        shutdownStarted = false;
    }
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
