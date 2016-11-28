#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "datatypes.h"

#include "ch.h"

void config_init(void);
Config* config_get_configuration(void);
void config_load_default_configuration(void);
bool config_write_all(void);
bool config_write_field(uint16_t addr, uint8_t *data, uint8_t size);
void config_read_all(void);

#endif /* _CONFIG_H_ */
