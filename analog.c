#include "analog.h"
#include "hal.h"
#include "hw_conf.h"
#include "faults.h"
#include <math.h>

static const ADCConversionGroup adc3 = {
    FALSE,
    2,
    NULL,
    NULL,
    0,                        /* CFGR    */
    ADC_TR(0, 4095),          /* TR1     */
    {                         /* SMPR[2] */
        0,
        0
    },
    {                         /* SQR[4]  */
        ADC_SQR1_SQ1_N(CHG_SENSE_CHANNEL) | ADC_SQR1_SQ2_N(DSG_SENSE_CHANNEL),
        0,
        0,
        0
    }
};

static const ADCConversionGroup adc4 = {
    FALSE,
    1,
    NULL,
    NULL,
    0,                        /* CFGR    */
    ADC_TR(0, 4095),          /* TR1     */
    {                         /* SMPR[2] */
        0,
        0
    },
    {                         /* SQR[4]  */
        ADC_SQR1_SQ1_N(TEMP_SENSE_CHANNEL),
        0,
        0,
        0
    }
};

static volatile uint16_t charger_input_voltage;
static volatile uint16_t thermistor;
static volatile uint16_t discharge_voltage;

void analog_init(void)
{
    adcStart(&ADCD3, NULL);
    adcStart(&ADCD4, NULL);
    adcsample_t samples[2];
    adcConvert(&ADCD3, &adc3, samples, 1);
    charger_input_voltage = samples[0];
    discharge_voltage = samples[1];
}

void analog_update(void)
{
    adcsample_t samples3[2];
    adcConvert(&ADCD3, &adc3, samples3, 1);
    charger_input_voltage = samples3[0];
    discharge_voltage = samples3[1];
    adcsample_t samples4[1];
    adcConvert(&ADCD4, &adc4, samples4, 1);
    thermistor = samples4[0];
    if (analog_temperature() > 100)
        faults_set_fault(FAULT_BOARD_TEMP);
}

float analog_charger_input_voltage(void)
{
    return (float)charger_input_voltage / 4095.0 * 3.3 * (51000.0 + 18000.0 + 4700.0) / 4700.0;
}

float analog_temperature(void)
{
    return (1.0 / ((logf(((4095.0 * 10000.0) / thermistor - 10000.0) / 10000.0) / 3434.0) + (1.0 / 298.15)) - 273.15);
}

float analog_discharge_voltage(void)
{
    return (float)discharge_voltage * (3.3 / 4095.0) * (200000.0 + 100 + 2500 + 10000.0) / 10000.0;

}
