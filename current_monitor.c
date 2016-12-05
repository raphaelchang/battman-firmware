#include "current_monitor.h"
#include "hal.h"
#include "hw_conf.h"
#include "config.h"
#include "power.h"
#include "faults.h"
#include <math.h>

#define I2C_ADDRESS 0x40

static void curr_alert(EXTDriver *extp, expchannel_t channel);
static const EXTConfig extcfg = {
    {
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, curr_alert},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL},
        {EXT_CH_MODE_DISABLED, NULL}
    }
};

static volatile Config *config;
static volatile float current;
static volatile float coulomb_count;
static volatile float voltage;
static volatile float power;

void current_monitor_init(void)
{
    config = config_get_configuration();
    extStart(&EXTD1, &extcfg);
    extChannelEnable(&EXTD1, 12);
    uint8_t tx[3];
    uint8_t rx[2];
    uint16_t current_cal = 4183;
    tx[0] = 0x05;
    tx[1] = (uint8_t)(current_cal >> 7);
    tx[2] = (uint8_t)((current_cal & 0xFF) << 1);
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 3, rx, 0, MS2ST(10));
    current_monitor_set_overcurrent(config->maxCurrentCutoff);
    tx[0] = 0x09;
    tx[1] = 0x00;
    tx[2] = 0x80;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 3, rx, 0, MS2ST(10));
}

void current_monitor_update(void)
{
    i2cAcquireBus(&I2C_DEV);
    uint8_t tx[1];
    uint8_t rx[2];
    /*tx[0] = 0x04;*/
    /*i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 2, MS2ST(10));*/
    /*int16_t current_value = (rx[0] << 8) | rx[1];*/
    /*current = current_value * 0.01958504192;*/
    tx[0] = 0x01;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 2, MS2ST(10));
    int16_t shunt_value = (rx[0] << 8) | rx[1];
    float shunt_voltage = shunt_value * 1.0e-5;
    current = shunt_voltage / 0.0005;
    tx[0] = 0x02;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 1, rx, 2, MS2ST(10));
    int16_t voltage_value = (rx[0] << 6) | (rx[1] >> 2);
    voltage = voltage_value * 0.004;
    i2cReleaseBus(&I2C_DEV);
    if (current > config->maxCurrentCutoff || !palReadPad(CURR_ALERT_GPIO, CURR_ALERT_PIN))
    {
        power_disable_discharge();
        faults_set_fault(FAULT_OVERCURRENT);
    }
}

float current_monitor_get_current(void)
{
    return current;
}

float current_monitor_get_bus_voltage(void)
{
    return voltage;
}

float current_monitor_get_power(void)
{
    return power;
}

static void curr_alert(EXTDriver *extp, expchannel_t channel) {
    (void)extp;
    (void)channel;

    chSysLockFromISR();

    power_disable_discharge();
    faults_set_fault(FAULT_OVERCURRENT);

    chSysUnlockFromISR();
}

void current_monitor_set_overcurrent(float current_threshold)
{
    i2cAcquireBus(&I2C_DEV);
    int8_t value_max = ceil(current_threshold * 0.0005 / 0.00256);
    int8_t value_min = -value_max;
    uint8_t tx[3];
    uint8_t rx[2];
    tx[0] = 0x06;
    tx[1] = *(uint8_t*)&value_max;
    tx[2] = *(uint8_t*)&value_min;
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 3, rx, 0, MS2ST(10));
    i2cReleaseBus(&I2C_DEV);
}
