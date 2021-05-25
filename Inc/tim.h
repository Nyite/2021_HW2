#ifndef _TIM_H_
#define _TIM_H_

#include "global.h"

#define TIM_FERQ_ERROR 	1
#define TIM_FERQ_SET	0

#define START_TIMER(source) (source->CR1 |= TIM_CR1_CEN)
#define STOP_TIMER(source) (source->CR1 &= ~TIM_CR1_CEN)
#define TIMER_CNT_RESET(source) (source->CNT = 0)

void TIM3_init();
void TIM6_init();

uint8_t TIM3_FrequencySet(uint8_t req_freq);

#endif
