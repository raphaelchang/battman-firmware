#ifndef _LTC6803_H_
#define _LTC6803_H_

#include "ch.h"
#include "hw_conf.h"

void ltc6803_init(void);
void ltc6803_wrcfg(uint8_t config[][6]);
void ltc6803_stcvad(void);
void ltc6803_rdcv(uint16_t cells[][12]);

#endif /* _LTC6803_H_ */
