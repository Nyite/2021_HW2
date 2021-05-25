#include "tim.h"
#include "gpio.h"
#include "buffer.h"
#include "serial.h"

extern uint8_t SerialSetBitFlag;
extern uint8_t SerialReceiveBitFlag;
extern uint8_t DeviceMode;
uint8_t TIM_indx;

// Main serial receive / transmit clock //
void TIM3_IRQHandler()
{
	TIM3->SR &= ~0x1;
	// Bits transfer / reception //
	if (TIM_indx) // Every second interrupt
	{
		SerialReceiveBitFlag = PENDING_BIT_RECEIVE;
	}
	else
	{
		SerialSetBitFlag = PENDING_BIT_SET; // Require bit set on serial data line
	}
	TIM_indx ^= 0x1;
}

// Button contact bounce protection 100ms one-pulse mode timer //
void TIM6_DAC_IRQHandler()
{
	TIM6->SR &= ~0x1;
	if (GPIOA->IDR & 0x1)
	{
		// Button press confirm //
		MasterMode_init();
	}
}

// Main serial clock initialization //
void TIM3_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3_FrequencySet(FREQ_20K);	// Default value to be changed
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_SetPriority(TIM3_IRQn, 7);
	NVIC_DisableIRQ(TIM3_IRQn);

	START_TIMER(TIM3);
}

// Button contact bounce protection one pulse mode timer initialization //
void TIM6_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->CR1 |= TIM_CR1_ARPE;
	TIM6->CR1 |= TIM_CR1_OPM;	// One pulse mode

	TIM6->PSC = 1600-1;			// 100ms delay
	TIM6->ARR = 1000-1;

	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_SetPriority(TIM6_DAC_IRQn, 7);
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

uint8_t TIM3_FrequencySet(uint8_t req_freq)
{
	// Frequency of transfer is defined in global.h settings file //
	switch (req_freq)
	{
		case FREQ_100:
			// Prescaler and auto-reload registers values corresponding to the chosen frequency //
			TIM3->ARR = 80-1;
			TIM3->PSC = 1000-1;
		break;
		case FREQ_500:
			TIM3->ARR = 80-1;
			TIM3->PSC = 200-1;
		break;
		case FREQ_1K:
			TIM3->ARR = 80-1;
			TIM3->PSC = 100-1;
		break;
		case FREQ_10K:
			TIM3->ARR = 80-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_20K:
			TIM3->ARR = 40-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_50K:
			TIM3->ARR = 16-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_100K:
			TIM3->ARR = 8-1;
			TIM3->PSC = 10-1;
		break;
		default:
			// In case of undefined frequency of serial transfer //
			GPIOE->BSRR = FREQ_0 << FREQ_POS; // Reset f0..f2 ports to 0
		return TIM_FERQ_ERROR;
	}
	return TIM_FERQ_SET;	// OK
}
