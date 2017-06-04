#include "charger.h"
#include "hal.h"
#include "hw_conf.h"
#include "config.h"
#include "current_monitor.h"
#include "power.h"
#include "ltc6803.h"
#include "analog.h"
#include <math.h>

#define I2C_ADDRESS 0x1F

static volatile Config *config;
static volatile float charge_voltage;
static volatile bool is_charging;
static volatile float input_voltage;
static volatile float output_voltage;
static volatile bool charge_enabled = true;
static volatile float current_control_integral = 0.0;
static volatile systime_t lastTime;
static volatile bool balancing = false;
static volatile systime_t balanceUpdateTime;
static volatile bool chargeComplete = false;
static volatile uint8_t chargeCompleteCounter = 0;

static void set_voltage(float voltage);

void charger_init(void)
{
    config = config_get_configuration();
    palSetPad(CHG_SW_GPIO, CHG_SW_PIN);
#ifdef BATTMAN_4_0
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x07 << 2;
    tx[1] = 0x01 << 1;
    i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
#endif
    set_voltage(config->chargeVoltage);
    lastTime = chVTGetSystemTime();
    balanceUpdateTime = chVTGetSystemTime();
}

void charger_update(void)
{
    input_voltage = analog_charger_input_voltage();
    is_charging = input_voltage > 6.0;
    if (charge_enabled && is_charging && !chargeComplete)
    {
        /*power_disable_discharge();*/
        float currentErr;
        float chargeVoltage;
        float dt = ST2US(chVTTimeElapsedSinceX(lastTime)) / 1e6;
        float* cells;
        cells = ltc6803_get_cell_voltages();
        float highestCellV = 0.0;
        float lowestCellV = 999.0;
        uint8_t lowestCellNum;
        if (!balancing)
        {
            for (uint8_t i = 0; i < config->numCells; i++)
            {
                if (cells[i] > highestCellV)
                {
                    highestCellV = cells[i];
                }
                if (cells[i] < lowestCellV)
                {
                    lowestCellV = cells[i];
                    lowestCellNum = i;
                }
            }
            if ((highestCellV >= config->balanceStartVoltage && highestCellV - lowestCellV > config->balanceDifferenceThreshold) && ST2MS(chVTTimeElapsedSinceX(balanceUpdateTime)) > 10000 && !chargeComplete)
            {
                balancing = true;
                balanceUpdateTime = chVTGetSystemTime();
            }
        }
        if (balancing)
        {
            palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
            balancing = true;
            if (ST2MS(chVTTimeElapsedSinceX(balanceUpdateTime)) > 500)
            {
                bool continueBalance = false;
                for (uint8_t i = 0; i < config->numCells; i++)
                {
                    if (cells[i] > cells[lowestCellNum])
                    {
                        ltc6803_enable_balance(i + 1);
                        continueBalance = true;
                    }
                    else
                    {
                        ltc6803_disable_balance(i + 1);
                    }
                }
                balancing = continueBalance;
                balanceUpdateTime = chVTGetSystemTime();
            }
        }
        else if (highestCellV < config->highVoltageCutoff)
        {
            palSetPad(CHG_SW_GPIO, CHG_SW_PIN);
            switch(config->chargeMode)
            {
                case CURRENT_CONTROL:

                    currentErr = config->chargeCurrent - (-current_monitor_get_current());
                    current_control_integral += currentErr * dt;
                    // Windup protection
                    if (current_control_integral > 0)
                        current_control_integral = 0;
                    else if (current_control_integral < -config->chargeVoltage)
                        current_control_integral = -config->chargeVoltage;
                    chargeVoltage = config->chargeVoltage + currentErr * config->chargeCurrentGain_P + current_control_integral * config->chargeCurrentGain_I;
                    if (chargeVoltage > config->chargeVoltage)
                        chargeVoltage = config->chargeVoltage;
                    set_voltage(chargeVoltage);
                    break;
                case FULL_CURRENT:
                    set_voltage(config->chargeVoltage);
                    break;
                case BYPASS_CC:
                    set_voltage(0);
                    break;
                case BYPASS_CCCV:
                    set_voltage(0);
                    break;
            }
            if (current_monitor_get_bus_voltage() >= config->chargeVoltage && fabs(current_monitor_get_current()) < config->chargeCurrent / 10.0)
            {
                chargeCompleteCounter++;
                if (chargeCompleteCounter > 100)
                    chargeComplete = true;
            }
            else
            {
                chargeCompleteCounter = 0;
            }
        }
        else
        {
            palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
        }
    }
    else
    {
        if (!is_charging)
        {
            palSetPad(CHG_SW_GPIO, CHG_SW_PIN);
            chargeComplete = false;
            chargeCompleteCounter = 0;
        }
        else
            palClearPad(CHG_SW_GPIO, CHG_SW_PIN);
        current_control_integral = 0.0;
        balancing = false;
        ltc6803_disable_balance_all();
    }
    lastTime = chVTGetSystemTime();
}

bool charger_is_charging(void)
{
    return is_charging && !chargeComplete;
}

bool charger_is_balancing(void)
{
    return balancing;
}

float charger_get_input_voltage(void)
{
    return input_voltage;
}

float charger_get_output_voltage(void)
{
    return output_voltage;
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
    i2cAcquireBus(&I2C_DEV);
#ifdef BATTMAN_4_0
    float resistance = (5100 * voltage - 1.26 * (5100 + 200000)) / (1.26 - voltage);
    uint16_t value = (uint16_t)((resistance / 20000) * 1024);
    uint8_t tx[2];
    uint8_t rx[2];
    tx[0] = 0x01 << 2 | ((value >> 8) & 0x03);
    tx[1] = value & 0xFF;
    i2cMasterTransmitTimeout(&I2C_DEV, 0x2E, tx, 2, rx, 0, MS2ST(10));
#elif defined(BATTMAN_4_1) || defined(BATTMAN_4_2)
#if defined(BATTMAN_4_1)
    float dac_voltage = 0.075 * (51.66 - voltage);
#else
    float dac_voltage = 0.075 * (51.584 - voltage);
#endif
    if (dac_voltage < 0)
        dac_voltage = 0;
    else if (dac_voltage > 3.3)
        dac_voltage = 3.3;
    uint16_t value = (uint16_t)((dac_voltage / 3.3) * 16383);
    uint8_t tx[3];
    uint8_t rx[2];
    tx[0] = 0x01;
    tx[1] = (uint8_t)(value >> 6);
    tx[2] = (uint8_t)((value & 0xFF) << 2);
    i2cMasterTransmitTimeout(&I2C_DEV, I2C_ADDRESS, tx, 3, rx, 0, MS2ST(10));
#endif
    output_voltage = voltage;
    i2cReleaseBus(&I2C_DEV);
}
