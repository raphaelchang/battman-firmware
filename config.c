#include "config.h"
#include "ch.h"
#include "hal.h"
#include "eeprom.h"
#include "stm32f30x_conf.h"
#include <string.h>
#include "utils.h"

#define EEPROM_BASE              1000

// Global variables
uint16_t VirtAddVarTab[NB_OF_VAR];

static volatile Config config;

void config_init(void)
{
    config.CANDeviceID = 0x01;
    config.numCells = 6;
    config.lowVoltageCutoff = 3.2;
    config.maxCurrentCutoff = 150.0;
    config.chargeVoltage = 23.0;
    config.chargeCurrent = 3.0;
    config.turnOnDelay = 250;
    config.shutdownDelay = 500;

    memset(VirtAddVarTab, 0, sizeof(VirtAddVarTab));

    int ind = 0;
    for (unsigned int i = 0; i < (sizeof(Config) / 2); i++) {
	VirtAddVarTab[ind++] = EEPROM_BASE + i;
    }

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);
    EE_Init();
}

Config* config_get_configuration(void)
{
    return &config;
}

bool config_write(void)
{
    utils_sys_lock_cnt();

    bool is_ok = true;
    uint8_t *conf_addr = (uint8_t*)&config;
    uint16_t var;

    FLASH_ClearFlag(FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

    for (unsigned int i = 0;i < (sizeof(Config) / 2);i++) {
	var = (conf_addr[2 * i] << 8) & 0xFF00;
	var |= conf_addr[2 * i + 1] & 0xFF;

	if (EE_WriteVariable(EEPROM_BASE + i, var) != FLASH_COMPLETE) {
	    is_ok = false;
	    break;
	}
    }

    utils_sys_unlock_cnt();
    return is_ok;
}

void config_read(Config *conf)
{
    bool is_ok = true;
    uint8_t *conf_addr = (uint8_t*)conf;
    uint16_t var;

    for (unsigned int i = 0;i < (sizeof(Config) / 2);i++) {
	if (EE_ReadVariable(EEPROM_BASE + i, &var) == 0) {
	    conf_addr[2 * i] = (var >> 8) & 0xFF;
	    conf_addr[2 * i + 1] = var & 0xFF;
	} else {
	    is_ok = false;
	    break;
	}
    }

    /*if (!is_ok) {*/
    /*}*/
}
