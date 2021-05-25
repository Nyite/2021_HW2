#include "global.h"
#include "gpio.h"
#include "tim.h"
#include "serial.h"
#include "buffer.h"
#include "parallel.h"

BUFFER dataTx_buf;
BUFFER dataRx_buf;
uint8_t DeviceMode;

void Button_init()
{
	TIM6_init();
	GPIOA_Button_init();
}

int main(void)
{
	GPIOD_init();
	SlaveMode_init();
	Button_init();
	while(1)
	{
		Serial_trancieveData();
		ParallelTranceive();
	}
}
