#include "fw_updater.h"
#include "stm32f30x_conf.h"
#include "utils.h"
#include "comm_usb.h"
#include "power.h"

#define BOOTLOADER_ADDR             0x08030000
#define FIRMWARE_ADDR               0x08000000
#define NEW_FW_ADDR                 0x08018000
#define FW_NUM_PAGES                48	

uint16_t fw_updater_write_firmware(uint32_t offset, uint8_t *data, uint32_t len)
{
    FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

    utils_sys_lock_cnt();

    for (uint32_t i = 0; i < len; i += 2) {
	uint16_t res = FLASH_ProgramHalfWord(NEW_FW_ADDR + offset + i, (uint16_t)(data[i + 1] << 8) | data[i]);
	if (res != FLASH_COMPLETE) {
	    return res;
	}
    }

    utils_sys_unlock_cnt();

    return FLASH_COMPLETE;
}

uint16_t fw_updater_erase_new_firmware(void) {
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

    utils_sys_lock_cnt();

    for (int i = 0; i < FW_NUM_PAGES; i++) {
	uint16_t res = FLASH_ErasePage(NEW_FW_ADDR + i * 2048);
	if (res != FLASH_COMPLETE) {
	    return res;
	}
    }

    utils_sys_unlock_cnt();

    return FLASH_COMPLETE;
}

void fw_updater_jump_bootloader(void) {
    typedef void (*pFunction)(void);

    power_disable_discharge();
    comm_usb_deinit();

    chSysDisable();

    pFunction jump_to_bootloader;

    // Variable that will be loaded with the start address of the application
    volatile uint32_t* jump_address;
    const volatile uint32_t* bootloader_address = (volatile uint32_t*)BOOTLOADER_ADDR;

    // Get jump address from application vector table
    jump_address = (volatile uint32_t*) bootloader_address[1];

    // Load this address into function pointer
    jump_to_bootloader = (pFunction) jump_address;

    // Clear pending interrupts
    SCB->ICSR = SCB_ICSR_PENDSVCLR_Msk;

    // Disable all interrupts
    for (int i = 0; i < 8; i++) {
	NVIC->ICER[i] = NVIC->IABR[i];
    }

    // Set stack pointer
    __set_MSP((uint32_t) (bootloader_address[0]));

    // Jump to the bootloader
    jump_to_bootloader();
}
