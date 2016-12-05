#ifndef _CURRENT_MONITOR_H_
#define _CURRENT_MONITOR_H_

#include "ch.h"

void current_monitor_init(void);
void current_monitor_update(void);
float current_monitor_get_current(void);
float current_monitor_get_bus_voltage(void);
float current_monitor_get_power(void);
void current_monitor_set_overcurrent(float current_threshold);

#endif /* _CURRENT_MONITOR_H_ */
