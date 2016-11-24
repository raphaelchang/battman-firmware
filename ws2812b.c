#include "ch.h"
#include "hal.h"
#include <math.h>
#include "ws2812b.h"
#include "stm32f30x_conf.h"

// Settings
#define WS2812B_CLK_HZ 800000
#define WS2812B_LED_NUM 1
#define TIM_PERIOD			(((72000000 / WS2812B_CLK_HZ) - 1))
#define LED_BUFFER_LEN		(WS2812B_LED_NUM + 1)
#define BITBUFFER_PAD		50
#define BITBUFFER_LEN		(24 * LED_BUFFER_LEN + BITBUFFER_PAD)
#define WS2812B_ZERO		(TIM_PERIOD * 0.32)
#define WS2812B_ONE			(TIM_PERIOD * 0.64)

// Private variables
static uint16_t bitbuffer[BITBUFFER_LEN];
static uint32_t RGBdata[LED_BUFFER_LEN];
static uint8_t gamma_table[256];

// Private function prototypes
static uint32_t rgb_to_local(uint32_t color);

void ws2812b_init(void) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	// Default LED values
	int i, bit;

	for (i = 0; i < LED_BUFFER_LEN; i++) {
		RGBdata[i] = 0;
	}

	for (i = 0; i < LED_BUFFER_LEN; i++) {
		uint32_t tmp_color = rgb_to_local(RGBdata[i]);

		for (bit = 0;bit < 24;bit++) {
			if(tmp_color & (1 << 23)) {
				bitbuffer[bit + i * 24] = WS2812B_ONE;
			} else {
				bitbuffer[bit + i * 24] = WS2812B_ZERO;
			}
			tmp_color <<= 1;
		}
	}

	// Fill the rest of the buffer with zeros to give the LEDs a chance to update
	// after sending all bits
	for (i = 0; i < BITBUFFER_PAD; i++) {
		bitbuffer[BITBUFFER_LEN - BITBUFFER_PAD - 1 + i] = 0;
	}

	// Generate gamma correction table
	for (i = 0; i < 256; i++) {
		gamma_table[i] = (int)roundf(powf((float)i / 255.0, 1.0 / 0.45) * 255.0);
	}

	// DMA clock enable
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        rccEnableTIM1(false);

	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM1->CCR2;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)bitbuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = BITBUFFER_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	// Time Base configuration
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// Channel 2 Configuration in PWM mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = bitbuffer[0];
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	// TIM1 Update DMA Request enable
	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

	// TIM1 counter enable
	TIM_Cmd(TIM1, ENABLE);

        DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE);

	// Main Output Enable
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void ws2812b_set_led_color(int led, uint32_t color) {
	if (led < WS2812B_LED_NUM) {
		RGBdata[led] = color;

		color = rgb_to_local(color);

		int bit;
		for (bit = 0;bit < 24;bit++) {
			if(color & (1 << 23)) {
				bitbuffer[bit + led * 24] = WS2812B_ONE;
			} else {
				bitbuffer[bit + led * 24] = WS2812B_ZERO;
			}
			color <<= 1;
		}
	}
}

uint32_t ws2812b_get_led_color(int led) {
	if (led < WS2812B_LED_NUM) {
		return RGBdata[led];
	}

	return 0;
}

void ws2812b_all_off(void) {
	int i;

	for (i = 0; i < WS2812B_LED_NUM; i++) {
		RGBdata[i] = 0;
	}

	for (i = 0; i < (WS2812B_LED_NUM * 24); i++) {
		bitbuffer[i] = WS2812B_ZERO;
	}
}

void ws2812b_set_all(uint32_t color) {
	int i, bit;

	for (i = 0; i < WS2812B_LED_NUM; i++) {
		RGBdata[i] = color;

		uint32_t tmp_color = rgb_to_local(color);

		for (bit = 0; bit < 24; bit++) {
			if(tmp_color & (1 << 23)) {
				bitbuffer[bit + i * 24] = WS2812B_ONE;
			} else {
				bitbuffer[bit + i * 24] = WS2812B_ZERO;
			}
			tmp_color <<= 1;
		}
	}
}

static uint32_t rgb_to_local(uint32_t color) {
	uint32_t r = (color >> 16) & 0xFF;
	uint32_t g = (color >> 8) & 0xFF;
	uint32_t b = color & 0xFF;

	r = gamma_table[r];
	g = gamma_table[g];
	b = gamma_table[b];

	return (g << 16) | (r << 8) | b;
}
