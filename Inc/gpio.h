#ifndef _GPIO_H_
#define _GPIO_H_

#include "global.h"

#define GPIOE_FERQ_ERROR 	1
#define GPIOE_FERQ_SET		0

#define GPIOD_TOGGLE_PIN(pin_msk)	GPIOD->ODR ^= 0x1 << pin_msk
#define GPIOD_SET_PIN(pin_msk)		GPIOD->BSRR = 0x1 << pin_msk
#define GPIOD_RESET_PIN(pin_msk)	GPIOD->BSRR = 0x1 << (pin_msk + 16)

#define GPIOE_SET_PIN(pin_msk)		GPIOE->BSRR = 0x1 << pin_msk
#define GPIOE_RESET_PIN(pin_msk)	GPIOE->BSRR = 0x1 << (pin_msk + 16)
#define GPIOE_RESET_MSK(bus_msk)	GPIOE->BSRR = bus_msk << 16

void GPIOD_init();
void GPIOA_Button_init();

void GPIOE_SerialIn_init();
void GPIOE_SerialOut_init();

uint8_t GPIOE_FrequencySet();
uint8_t GPIOE_GetFrequency();

void GPIOE_ParallelIn_init();
void GPIOE_ParallelOut_init();
void GPIOE_setByte(uint8_t data_byte);

void MasterMode_init();
void SlaveMode_init();

enum GPIOD_PIN_MSK
{
	GPIOD_PIN_12 = 12,
	GPIOD_PIN_13 = 13,
	GPIOD_PIN_14 = 14,
	GPIOD_PIN_15 = 15,
};

#endif
