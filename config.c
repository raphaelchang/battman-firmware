#include "config.h"
#include "ch.h"
#include "hal.h"
#include "eeprom.h"
#include "stm32f30x_conf.h"
#include <string.h>
#include "utils.h"
#include <stddef.h>
#include "current_monitor.h"

#define EEPROM_BASE              1000

// Global variables
uint16_t VirtAddVarTab[NB_OF_VAR];

static volatile Config config;

void config_init(void)
{
    memset(VirtAddVarTab, 0, sizeof(VirtAddVarTab));

    int ind = 0;
    for (unsigned int i = 0; i < (sizeof(Config) / 2); i++) {
	VirtAddVarTab[ind++] = EEPROM_BASE + i;
    }

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);
    EE_Init();
    config_read_all();
}

void config_load_default_configuration(void)
{
    config.CANDeviceID = 0x01;
    config.numCells = 12;
    config.fullCellVoltage = 4.2;
    config.emptyCellVoltage = 3.4;
    config.packCapacity = 2500;
    config.lowVoltageCutoff = 3.2;
    config.lowVoltageWarning = 3.4;
    config.highVoltageCutoff = 4.25;
    config.highVoltageWarning = 4.1;
    config.maxCurrentCutoff = 120.0;
    config.maxContinuousCurrent = 100.0;
    config.continuousCurrentCutoffTime = 30;
    config.continuousCurrentCutoffWarning = 10;
    config.maxChargeCurrent = 20.0;
    config.chargeVoltage = 25.2;
    config.chargeCurrent = 2.0;
    config.turnOnDelay = 200;
    config.shutdownDelay = 500;
    config.chargeMode = FULL_CURRENT;
    config.chargeCurrentGain_P = 0.1;
    config.chargeCurrentGain_I = 1.0;
    config.prechargeTimeout = 500;
    config.balanceStartVoltage = 3.5;
    config.balanceDifferenceThreshold = 0.01;
    config.chargerDisconnectShutdown = true;
}

Config* config_get_configuration(void)
{
    return &config;
}

bool config_write_all(void)
{
    utils_sys_lock_cnt();

    bool is_ok = true;
    uint8_t *conf_addr = (uint8_t*)&config;
    uint16_t var;

    FLASH_ClearFlag(FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

    for (unsigned int i = 0; i < (sizeof(Config) / 2); i++) {
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

bool config_write_field(uint16_t addr, uint8_t *data, uint8_t size)
{
    if (addr == offsetof(Config, CANDeviceID))
    {

    }
    else if (addr == offsetof(Config, numCells))
    {
        if (*data > 12)
        {
            *data = 12;
        }
    }
    else if (addr == offsetof(Config, lowVoltageCutoff))
    {
    }
    else if (addr == offsetof(Config, highVoltageCutoff))
    {
        if (*((float*)data) > 5.0)
        {
            *((float*)data) = 5.0;
        }
        else if (*((float*)data) < 0.0)
        {
            *((float*)data) = 0.0;
        }
    }
    else if (addr == offsetof(Config, chargeVoltage))
    {
        if (*((float*)data) > config.numCells * config.highVoltageCutoff)
        {
            *((float*)data) = config.numCells * config.highVoltageCutoff;
        }
        else if (*((float*)data) < 0.0)
        {
            *((float*)data) = 0.0;
        }
    }
    else if (addr == offsetof(Config, maxCurrentCutoff))
    {
        if (*((float*)data) > 150.0)
        {
            *((float*)data) = 150.0;
        }
        else if (*((float*)data) < 1.0)
        {
            *((float*)data) = 1.0;
        }
        current_monitor_set_overcurrent(*((float*)data));
    }
    else
    {
        /*return false;*/
    }
    memcpy((void*)&config + addr, (void*)data, size);

    utils_sys_lock_cnt();

    bool is_ok = true;
    uint8_t *conf_addr = (uint8_t*)&config;
    uint16_t var;

    FLASH_ClearFlag(FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

    for (unsigned int i = (addr % 2 == 0 ? addr : addr - 1); i < addr + size; i += 2) {
        var = (conf_addr[i] << 8) & 0xFF00;
        var |= conf_addr[i + 1] & 0xFF;

        if (EE_WriteVariable(EEPROM_BASE + i / 2, var) != FLASH_COMPLETE) {
            is_ok = false;
            break;
        }
    }

    utils_sys_unlock_cnt();
    return is_ok;
}

void config_read_all(void)
{
    bool is_ok = true;
    uint8_t *conf_addr = (uint8_t*)&config;
    uint16_t var;

    for (unsigned int i = 0; i < (sizeof(Config) / 2); i++) {
	if (EE_ReadVariable(EEPROM_BASE + i, &var) == 0) {
	    conf_addr[2 * i] = (var >> 8) & 0xFF;
	    conf_addr[2 * i + 1] = var & 0xFF;
	} else {
	    is_ok = false;
	    break;
	}
    }

    if (!is_ok) {
        config_load_default_configuration();
        config_write_all();
    }
}
