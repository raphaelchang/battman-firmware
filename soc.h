#ifndef _SOC_H_
#define _SOC_H_

#include "ch.h"

void soc_init(void);
void soc_update(void);
float soc_get_coulomb_count(void);
float soc_get_relative_soc(void);

#endif /* _SOC_H_ */
