#include "rtcc.h"
#include "hal.h"
#include "hw_conf.h"

#define I2C_ADDRESS 0x51
#define HEX_TO_BCD(x) ((x / 10) << 4) | (x % 10)

static Time time;

void rtcc_init(void)
{
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x25;
    tx[1] = 0x01;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x26;
    tx[1] = 0x06;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x27;
    tx[1] = 0x02;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x29;
    tx[1] = 0x90;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x2B;
    tx[1] = 0x00;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x10;
    tx[1] = 0x00;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
}

void rtcc_update(void)
{
    i2cAcquireBus(&I2C_DEV);
    uint8_t tx[1];
    uint8_t rx[1];
    tx[0] = 0x01;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.second = ((rx[0] >> 4) & 0x07) * 10 + (rx[0] & 0x0F);
    tx[0] = 0x02;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.minute = ((rx[0] >> 4) & 0x07) * 10 + (rx[0] & 0x0F);
    tx[0] = 0x03;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.hour = ((rx[0] >> 4) & 0x03) * 10 + (rx[0] & 0x0F);
    tx[0] = 0x04;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.day = ((rx[0] >> 4) & 0x03) * 10 + (rx[0] & 0x0F);
    tx[0] = 0x06;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.month = ((rx[0] >> 4) & 0x01) * 10 + (rx[0] & 0x0F);
    tx[0] = 0x07;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 1, MS2ST(10));
    time.year = (rx[0] >> 4) * 10 + (rx[0] & 0x0F);
    i2cReleaseBus(&I2C_DEV);
}

Time rtcc_get_time(void)
{
    return time;
}

void rtcc_enable_alarm(void)
{
    uint8_t numDays;
    uint8_t month = time.day == 1 ? time.month : (time.month + 1) % 12;
    if (month == 1 || month == 3 || month == 5 || month == 8 || month == 10 || month == 12)
        numDays = 31;
    else if (month == 2)
        numDays = time.year % 4 == 0 ? 29 : 28;
    else
        numDays = 30;
    uint8_t day = time.day == 1 ? numDays : (time.day - 1 > numDays ? numDays : (time.day - 1));
    uint8_t tx[1];
    uint8_t rx[1];
    tx[0] = 0x0B;
    tx[1] = HEX_TO_BCD(day);
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
    tx[0] = 0x10;
    tx[1] = 0x08;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 2, rx, 0, MS2ST(10));
}
