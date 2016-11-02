#ifndef _CHARGER_H_
#define _CHARGER_H_

#include "ch.h"

void charger_init(void);
void charger_update(void);
bool charger_is_charging(void);
float charger_get_input_voltage(void);
void charger_enable(void);
void charger_disable(void);

#endif /* _CHARGER_H_ */
