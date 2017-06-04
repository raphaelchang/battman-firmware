#ifndef _LTC6803_H_
#define _LTC6803_H_

#include "ch.h"
#include "hw_conf.h"

void ltc6803_init(void);
void ltc6803_update(void);
float* ltc6803_get_cell_voltages(void);
void ltc6803_enable_balance(uint8_t cell);
void ltc6803_disable_balance(uint8_t cell);
void ltc6803_disable_balance_all(void);
void ltc6803_lock(void);
void ltc6803_unlock(void);

#endif /* _LTC6803_H_ */
