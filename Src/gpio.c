#include "gpio.h"
#include "tim.h"
#include "serial.h"
#include "parallel.h"

extern uint8_t DeviceMode;
extern uint8_t SerialStartReceiveFlag;
extern uint8_t SerialSetBitFlag;

extern uint8_t ParTranferFlag;
extern uint8_t ParReadyFlag;
extern uint8_t ParEnableFlag;

// EXTI on button press //
void EXTI0_IRQHandler()
{
	EXTI->PR |= EXTI_PR_PR0;
	// Starts TIM6 in one pulse mode to protect from contact bounce //
	START_TIMER(TIM6);
}

void EXTI9_5_IRQHandler()
{
	if (EXTI->PR & EXTI_PR_PR8)	// EXTI on ENABLE line
	{
		EXTI->PR |= EXTI_PR_PR8;
		GPIOE_SET_PIN(PAR_READY_PIN);
		ParEnableFlag = PAR_ENABLE;
	}
	if (EXTI->PR & EXTI_PR_PR9) // EXTI on READY line
	{
		EXTI->PR |= EXTI_PR_PR9;
		ParReadyFlag = PAR_READY;
		GPIOE_RESET_PIN(PAR_ENABLE_PIN);	// Byte set not complete
	}
}

void EXTI15_10_IRQHandler ()
{
	if (EXTI->PR & EXTI_PR_PR10)	// EXTI on data read channel in serial transfer <-> Master starts transfer
	{
		EXTI->PR |= EXTI_PR_PR10;
		ParTranferFlag = PAR_START; // Start parallel transmission

		if(TIM3_FrequencySet(GPIOE_GetFrequency()) == TIM_FERQ_ERROR)	// Try to get reception rate
			return;
		SerialStartReceiveFlag = RECEIVE_START;
	}
	if (EXTI->PR & EXTI_PR_PR11)	// EXTI on byte synchronization
	{
		EXTI->PR |= EXTI_PR_PR11;
		EXTI->IMR &= ~EXTI_IMR_MR11;
		EXTI->EMR &= ~EXTI_EMR_MR11;
		NVIC_DisableIRQ(EXTI15_10_IRQn);
	}
	TIMER_CNT_RESET(TIM3);
	NVIC_EnableIRQ(TIM3_IRQn);
}

// Mode & transfer start switch initialization //
void GPIOA_Button_init()
{
	// Button is located on PA0 pin //
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER0;

	EXTI->IMR|= EXTI_IMR_IM0;
	EXTI->EMR|= EXTI_EMR_EM0;

	EXTI->RTSR|= EXTI_RTSR_TR0;

	SYSCFG->EXTICR[0] &= ~0xF;

	NVIC_SetPriority(EXTI0_IRQn, 3);
	NVIC_EnableIRQ(EXTI0_IRQn);
}

void GPIOD_init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	// Output mode and pull-down on pins 12, 13, 14, 15 //
	GPIOD->MODER |= 0x55 << 24;
	GPIOD->PUPDR |= 0xAA << 24;
}

void GPIOE_ParallelOut_init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER &= ~(0xFFFFF);
	GPIOE->MODER |= 0x15555;

	EXTI->IMR &= ~EXTI_IMR_IM8;
	EXTI->EMR &= ~EXTI_EMR_EM8;

	EXTI->IMR |= EXTI_IMR_IM9;
	EXTI->EMR |= EXTI_EMR_EM9;

	EXTI->FTSR |= EXTI_FTSR_TR9;

	SYSCFG->EXTICR[2] |= 0x4 << 4;

	NVIC_SetPriority(EXTI9_5_IRQn, 3);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void GPIOE_ParallelIn_init()
{
	GPIOE->MODER &= ~(0xFFFFF);
	GPIOE->MODER |= 0x1 << (PAR_READY_PIN*2);

	EXTI->IMR &= ~EXTI_IMR_IM9;
	EXTI->EMR &= ~EXTI_EMR_EM9;

	EXTI->IMR |= EXTI_IMR_IM8;
	EXTI->EMR |= EXTI_EMR_EM8;

	EXTI->RTSR|= EXTI_RTSR_TR8;
	SYSCFG->EXTICR[2] |= 0x4;
}

void GPIOE_SerialOut_init()
{
	GPIOE_SET_PIN(DATA_SEND);
	GPIOE_SET_PIN(DATA_CH);
	GPIOE->MODER |= 0x155 << 20;	// f0...f2 & CH & SEND output mode select
	GPIOE->PUPDR &= ~(0xF << 20);

	EXTI->IMR &= ~EXTI_IMR_MR10;	// Interrupt Mask on line 10
	EXTI->EMR &= ~EXTI_EMR_MR10;	// Event Mask on line 10
	NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void GPIOE_SerialIn_init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->PUPDR |= 0x5 << 20;
	GPIOE->MODER &= ~(0x3FF << 20);	// Input mode on all serial bus pins

	SYSCFG->EXTICR[2] |= 1 << 10;	// PE10 interrupt line to EXTI connect
	EXTI->IMR |= EXTI_IMR_MR10;		// Interrupt Mask on line 10
	EXTI->EMR |= EXTI_EMR_MR10;		// Event Mask on line 10
	EXTI->FTSR |= EXTI_FTSR_TR10; 	// Falling trigger event configuration bit of line 10
	NVIC_SetPriority(EXTI15_10_IRQn, 5);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	SYSCFG->EXTICR[2] |= 1 << 14;	// PE11 interrupt line to EXTI connect
	EXTI->IMR &= ~EXTI_IMR_MR11;	// Interrupt Mask on line 11
	EXTI->EMR &= ~EXTI_EMR_MR11;	// Event Mask on line 11
	EXTI->FTSR |= EXTI_FTSR_TR11; 	// Falling trigger event configuration bit of line 11
}

uint8_t GPIOE_GetFrequency()
{
	return ((GPIOE->IDR & FREQ_MASK) >> FREQ_POS) & 0x7;
}

uint8_t GPIOE_FrequencySet()
{
	// Frequency of transfer is defined in global.h settings file //
	switch(SER_TRANFER_RATE)
	{
		case FREQ_100:
			// Sets 3 bit combination of chosen frequency to f0..f2 pins //
			GPIOE->BSRR = FREQ_100 << FREQ_POS;
			// Prescaler and auto-reload registers values corresponding to the chosen frequency //
			TIM3->ARR = 80-1;
			TIM3->PSC = 1000-1;
		break;
		case FREQ_500:
			GPIOE->BSRR = FREQ_500 << FREQ_POS;
			TIM3->ARR = 80-1;
			TIM3->PSC = 200-1;
		break;
		case FREQ_1K:
			GPIOE->BSRR = FREQ_1K << FREQ_POS;
			TIM3->ARR = 80-1;
			TIM3->PSC = 100-1;
		break;
		case FREQ_10K:
			GPIOE->BSRR = FREQ_10K << FREQ_POS;
			TIM3->ARR = 80-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_20K:
			GPIOE->BSRR = FREQ_20K << FREQ_POS;
			TIM3->ARR = 40-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_50K:
			GPIOE->BSRR = FREQ_50K << FREQ_POS;
			TIM3->ARR = 16-1;
			TIM3->PSC = 10-1;
		break;
		case FREQ_100K:
			GPIOE->BSRR = FREQ_100K << FREQ_POS;
			TIM3->ARR = 8-1;
			TIM3->PSC = 10-1;
		break;
		default:
			// In case of undefined frequency of serial transfer //
			GPIOE->BSRR = FREQ_0 << FREQ_POS; // Reset f0..f2 ports to 0
		return GPIOE_FERQ_ERROR;
	}
	return GPIOE_FERQ_SET;	// OK
}

void MasterMode_init()
{
	if (GPIOE_FrequencySet() == GPIOE_FERQ_ERROR)	// Try to set transmission rate
		return;
	SerialOut_init();
	ParallelIn_init();
	GPIOE_RESET_PIN(DATA_SEND);
	DeviceMode = MASTER_MODE;
	TIMER_CNT_RESET(TIM3);
	NVIC_EnableIRQ(TIM3_IRQn);
}

void SlaveMode_init()
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	DeviceMode = SLAVE_MODE;
	SerialIn_init();
	ParallelOut_init();
	TIM3_init();
}

void GPIOE_setByte(uint8_t data_byte)
{
	GPIOE->BSRR = 0xFF << 16;	//Reset bus pins
	GPIOE->BSRR = data_byte; // Set new data to pins
}
