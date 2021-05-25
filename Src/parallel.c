#include "parallel.h"
#include "gpio.h"
#include "buffer.h"
#include "mem.h"

extern uint8_t DeviceMode;
extern BUFFER dataTx_buf;
extern BUFFER dataRx_buf;
uint8_t ParTranferFlag;
uint8_t ParReadyFlag;
uint8_t ParEnableFlag;

void ParallelIn_init()
{
	GPIOE_ParallelIn_init();
	ParEnableFlag = PAR_NOT_ENABLE;
	BufferRx_init(&dataRx_buf);
}

void ParallelOut_init()
{
	GPIOE_ParallelOut_init();
	ParTranferFlag = PAR_STOP;
	ParReadyFlag = PAR_READY;	// Pretend that last byte is received
	BufferTx_init(&dataTx_buf);
}

void Parallel_appendData()
{
	dataRx_buf.data[dataRx_buf.ptr] = GPIOE->IDR & 0xFF;
	++dataRx_buf.ptr;
}

void ParallelTranceive()
{
	if (DeviceMode == MASTER_MODE)
	{
		if (Buffer_Eof(&dataRx_buf))
		{
			Parallel_stopReception();
			return;
		}
		if (ParEnableFlag == PAR_ENABLE)
		{
			ParEnableFlag = PAR_NOT_ENABLE;
			Parallel_appendData();
			GPIOE_RESET_PIN(PAR_READY_PIN);
		}
	}
	else // DeviceMode == SLAVE_MODE
	{
		if (ParTranferFlag == PAR_START)
		{
			if (ParReadyFlag == PAR_READY)	// Receiver is ready to receive next byte
			{
				if (Buffer_Eof(&dataTx_buf))
				{
					Parallel_stopTranfer();
					return;
				}
				ParReadyFlag = PAR_NOT_READY;
				GPIOE_setByte(dataTx_buf.data[dataTx_buf.ptr]);
				++dataTx_buf.ptr;
				GPIOE_SET_PIN(PAR_ENABLE_PIN);	// Byte set complete
			}
		}
	}
}

void Parallel_stopTranfer()
{
//	ParReadyFlag = PAR_READY;
	ParTranferFlag = PAR_STOP;
	GPIOE_setByte(0x00); // reset bus pins
//	Buffer_reset(&dataTx_buf);
}

void Parallel_stopReception()
{
	ParReadyFlag = PAR_READY;
	ParTranferFlag = PAR_STOP;
//	DeviceMode = SLAVE_MODE; // UNLESS PARALLERL IS FASTER THAN SERIAL
	GPIOE_ParallelOut_init();
}
