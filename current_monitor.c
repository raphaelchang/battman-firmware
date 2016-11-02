#include "current_monitor.h"
#include "config.h"

static volatile Config *config;
static volatile float current;
static volatile float coulomb_count;
static volatile float voltage;
static volatile float power;

void current_monitor_init(void)
{
    config = config_get_configuration();
}

void current_monitor_update(void)
{
}

float current_monitor_get_current(void)
{
    return current;
}

float current_monitor_get_coulomb_count(void)
{
    return coulomb_count;
}

float current_monitor_get_voltage(void)
{
    return voltage;
}

float current_monitor_get_power(void)
{
    return power;
}
