#ifndef _HW_CONF_V3_0_H_
#define _HW_CONF_V3_0_H_

#include "ch.h"
#include "hal.h"

// CHARGING CIRCUIT
#define CHRG_STAT_1_GPIO GPIOA
#define CHRG_STAT_1_PIN 2
#define CHRG_STAT_2_GPIO GPIOA
#define CHRG_STAT_2_PIN 3
#define CHRG_ENABLE_GPIO GPIOA
#define CHRG_ENABLE_PIN 8

// LTC6803
#define LTC6803_CS_GPIO GPIOA
#define LTC6803_CS_PIN 4

// ADC
#define ADC_CS_GPIO GPIOA
#define ADC_CS_PIN 15

// SPI
#define SCK_GPIO GPIOA
#define SCK_PIN 5
#define MISO_GPIO GPIOA
#define MISO_PIN 6
#define MOSI_GPIO GPIOA
#define MOSI_PIN 7

// CAN
#define CAN_RX_GPIO GPIOB
#define CAN_RX_PIN 8
#define CAN_TX_GPIO GPIOB
#define CAN_TX_PIN 9

// GPIO
#define PRECHRG_GPIO GPIOB
#define PRECHRG_PIN 0
#define POWER_LED_GPIO GPIOB
#define POWER_LED_PIN 1
#define PWR_BTN_NC_GPIO GPIOB
#define PWR_BTN_NC_PIN 3
#define PWR_SW_GPIO GPIOB
#define PWR_SW_PIN 4
#define LOAD_SW_GPIO GPIOB
#define LOAD_SW_PIN 5
#define CHRG_DETECT_GPIO GPIOB
#define CHRG_DETECT_PIN 6

// WS2812
#define WS2812_SW_GPIO GPIOB
#define WS2812_SW_PIN 14

// USB
#define USB_DM_GPIO GPIOA
#define USB_DM_PIN 11
#define USB_DP_GPIO GPIOA
#define USB_DP_PIN 12


#endif
