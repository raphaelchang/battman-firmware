#include "power.h"
#include "hw_conf.h"

static volatile bool discharge_enabled = false;
static volatile PowerOnEvent power_on_event;
static volatile bool power_button_released = false;

void power_init(void)
{
    palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
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
        discharge_enabled = true;
}

void power_update(void)
{
    palSetPad(PWR_SW_GPIO, PWR_SW_PIN);
    if (discharge_enabled)
        palSetPad(DSG_SW_GPIO, DSG_SW_PIN);
    else
        palClearPad(DSG_SW_GPIO, DSG_SW_PIN);
    if (palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN))
        power_button_released = true;
    if (!palReadPad(PWR_BTN_GPIO, PWR_BTN_PIN) && power_button_released)
    {
        // Shutdown
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

PowerOnEvent power_get_power_on_event(void)
{
    return power_on_event;
}
