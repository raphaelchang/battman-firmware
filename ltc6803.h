#ifndef _LTC6803_H_
#define _LTC6803_H_

#include "ch.h"
#include "hw_conf.h"

void ltc6803_init(void);
void ltc6803_update(void);
uint16_t* ltc6803_get_cell_voltages(void);

#endif /* _LTC6803_H_ */
