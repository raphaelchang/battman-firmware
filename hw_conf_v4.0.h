#ifndef _HW_CONF_V4_0_H_
#define _HW_CONF_V4_0_H_

#include "ch.h"
#include "hal.h"

// ADC
#define CURR_SENSE_GPIO GPIOB
#define CURR_SENSE_PIN 13
#define VBUS_SENSE_GPIO GPIOB
#define VBUS_SENSE_PIN 14
#define TEMP_SENSE_GPIO GPIOB
#define TEMP_SENSE_PIN 15

// SPI
#define LTC6803_CS_GPIO GPIOA
#define LTC6803_CS_PIN 4
#define SCK_GPIO GPIOA
#define SCK_PIN 5
#define MISO_GPIO GPIOA
#define MISO_PIN 6
#define MOSI_GPIO GPIOA
#define MOSI_PIN 7

// I2C
#define SDA_GPIO GPIOA
#define SDA_PIN 14
#define SCL_GPIO GPIOA
#define SCL_PIN 15
#define I2C_DEV I2CD1

// CAN
#define CAN_RX_GPIO GPIOB
#define CAN_RX_PIN 8
#define CAN_TX_GPIO GPIOB
#define CAN_TX_PIN 9

// Accessory Ports
#define ACC1_GPIO GPIOA
#define ACC1_PIN 10
#define ACC2_GPIO GPIOA
#define ACC2_PIN 9
#define ACC_I2C_DEV I2CD2

// RGB LED
#define LED_R_GPIO GPIOB
#define LED_R_PIN 11
#define LED_R_CHANNEL 3
#define LED_G_GPIO GPIOB
#define LED_G_PIN 10
#define LED_G_CHANNEL 2
#define LED_B_GPIO GPIOB
#define LED_B_PIN 3
#define LED_B_CHANNEL 1
#define LED_PWM_DEV PWMD2

// GPIO
#define PWR_SW_GPIO GPIOB
#define PWR_SW_PIN 1
#define PCHG_SW_GPIO GPIOB
#define PCHG_SW_PIN 4
#define DSG_SW_GPIO GPIOB
#define DSG_SW_PIN 5
#define CHG_SW_GPIO GPIOB
#define CHG_SW_PIN 6
#define PWR_BTN_GPIO GPIOB
#define PWR_BTN_PIN 2
#define CHG_SENSE_GPIO GPIOB
#define CHG_SENSE_PIN 0

// USB
#define USB_DM_GPIO GPIOA
#define USB_DM_PIN 11
#define USB_DP_GPIO GPIOA
#define USB_DP_PIN 12

#endif
