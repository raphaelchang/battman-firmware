#ifndef _ANALOG_H_
#define _ANALOG_H_

#include "ch.h"

void analog_init(void);
void analog_update(void);
float analog_charger_input_voltage(void);
float analog_temperature(void);
float analog_discharge_voltage(void);

#endif /* _ANALOG_H_ */
