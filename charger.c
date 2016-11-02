#include "charger.h"
#include "hal.h"
#include "hw_conf.h"
#include "config.h"
#include "adc.h"

static volatile Config *config;
static volatile float charge_voltage;
static volatile bool is_charging;
static volatile float input_voltage;
static volatile bool charge_enabled = true;

static void set_voltage(float voltage);

void charger_init(void)
{
    config = config_get_configuration();
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x07 << 2;
    tx[1] = 0x01 << 1;
    msg_t msg = i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
    palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
    set_voltage(config->chargeVoltage);
}

void charger_update(void)
{
    set_voltage(config->chargeVoltage);
    is_charging = palReadPad(CHG_SENSE_GPIO, CHG_SENSE_PIN);
    if (charge_enabled && is_charging)
        palSetPad(CHG_SW_GPIO, CHG_SW_PIN);
    else
        palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
    input_voltage = adc_charger_input_voltage() / 4096.0 * 3.3 * (47000.0 + 28000.0) / 28.000;
}

bool charger_is_charging(void)
{
    return is_charging;
}

float charger_get_input_voltage(void)
{
    return input_voltage;
}

void charger_enable(void)
{
    charge_enabled = true;
}

void charger_disable(void)
{
    charge_enabled = false;
    palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
}

static void set_voltage(float voltage)
{
    float resistance = (5100 * voltage - 1.26 * (5100 + 200000)) / (1.26 - voltage);
    uint16_t value = (uint16_t)((resistance / 20000) * 1024);
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x01 << 2 | ((value >> 8) & 0x03);
    tx[1] = value & 0xFF;
    i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
}
