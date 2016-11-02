#ifndef _AD5272_H_
#define _AD5272_H_

#include "ch.h"

void ad5272_init(void);
msg_t ad5272_set_resistance(uint16_t value);

#endif /* _AD5272_H_ */
