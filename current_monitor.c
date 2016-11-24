#include "current_monitor.h"
#include "hal.h"
#include "hw_conf.h"
#include "config.h"

#define I2C_ADDRESS 0x40

static volatile Config *config;
static volatile float current;
static volatile float coulomb_count;
static volatile float voltage;
static volatile float power;

void current_monitor_init(void)
{
    config = config_get_configuration();
    uint8_t tx[3];
    uint8_t rx[2];
    uint16_t current_cal = 4183;
    tx[0] = 0x05;
    tx[1] = (uint8_t)(current_cal >> 7);
    tx[2] = (uint8_t)((current_cal & 0xFF) << 1);
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 3, rx, 0, MS2ST(10));
}

void current_monitor_update(void)
{
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
}

float current_monitor_get_current(void)
{
    return current;
}

float current_monitor_get_coulomb_count(void)
{
    return coulomb_count;
}

float current_monitor_get_bus_voltage(void)
{
    return voltage;
}

float current_monitor_get_power(void)
{
    return power;
}
