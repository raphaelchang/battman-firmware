#ifndef _RTCC_H_
#define _RTCC_H_

#include "ch.h"
#include "datatypes.h"

void rtcc_init(void);
void rtcc_update(void);
Time rtcc_get_time(void);
void rtcc_enable_alarm(void);

#endif /* _RTCC_H_ */
