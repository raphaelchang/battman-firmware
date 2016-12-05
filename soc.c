#include "soc.h"
#include "current_monitor.h"
#include "config.h"

static volatile Config *config;
static float coulomb_count;
static systime_t prevTime;

void soc_init(void)
{
    config = config_get_configuration();
    prevTime = chVTGetSystemTime();
}

void soc_update(void)
{
    float dt = ST2US(chVTTimeElapsedSinceX(prevTime)) / 1.0e6;
    prevTime = chVTGetSystemTime();
    float current = current_monitor_get_current();
    coulomb_count -= current * dt;
}

float soc_get_coulomb_count(void)
{
    return coulomb_count;
}

float soc_get_relative_soc(void)
{
    return coulomb_count * 3600 * 1000 / config->packCapacity;
}
