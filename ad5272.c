#include "ad5272.h"
#include "hal.h"
#include "hw_conf.h"

static const I2CConfig i2cconfig = {
    STM32_TIMINGR_PRESC(15U) |
        STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
        STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(21U),
    0,
    0
};

void ad5272_init(void)
{
    i2cStart(&I2C_DEV, &i2cconfig);
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x07 << 2;
    tx[1] = 0x01 << 1;
    msg_t msg = i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
}

msg_t ad5272_set_resistance(uint16_t value)
{
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x01 << 2 | ((value >> 8) & 0x03);
    tx[1] = value & 0xFF;
    msg_t msg = i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
    return msg;
}
