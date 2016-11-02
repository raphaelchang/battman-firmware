#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "datatypes.h"

#include "ch.h"

void config_init(void);
Config* config_get_configuration(void);
bool config_write(void);
void config_read(Config *conf);

#endif /* _CONFIG_H_ */
