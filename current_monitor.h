#ifndef _CURRENT_MONITOR_H_
#define _CURRENT_MONITOR_H_

#include "ch.h"

void current_monitor_init(void);
void current_monitor_update(void);
float current_monitor_get_current(void);
float current_monitor_get_coulomb_count(void);
float current_monitor_get_voltage(void);
float current_monitor_get_power(void);

#endif /* _CURRENT_MONITOR_H_ */
