#ifndef _FW_UPDATER_H_
#define _FW_UPDATER_H_

#include "ch.h"

uint16_t fw_updater_write_firmware(uint32_t offset, uint8_t *data, uint32_t len);
uint16_t fw_updater_erase_new_firmware(void);
void fw_updater_jump_bootloader(void);

#endif /* _FW_UPDATER_H_ */
