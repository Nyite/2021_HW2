#include "serial.h"
#include "gpio.h"
#include "tim.h"
#include "buffer.h"
#include "mem.h"

uint8_t SerialSetBitFlag;
uint8_t SerialReceiveBitFlag;
uint8_t SerialStartReceiveFlag;

extern BUFFER dataTx_buf;
extern BUFFER dataRx_buf;
extern uint8_t DeviceMode;
extern uint8_t TIM_indx;

void SerialOut_init()
{
	SerialSetBitFlag = BIT_SET_DONE;
	GPIOE_SerialOut_init();
	BufferTx_init(&dataTx_buf); // Data to send
}

void SerialIn_init()
{
	TIM_indx = 0;
	SerialReceiveBitFlag = BIT_RECEIVE_DONE;
	SerialStartReceiveFlag = RECEIVE_STOP;
	GPIOE_SerialIn_init();
	BufferRx_init(&dataRx_buf);
}

// This function sends bits of data from LSB to MSB //
void SerialSetData()
{
	static uint8_t bit_mask = 255; // Chooses current bit of data
	// Reset bit operation pending flag //
		SerialSetBitFlag = BIT_SET_DONE;
	if (bit_mask == 255)	// Start bit
	{
		GPIOE_RESET_PIN(DATA_CH);
		++bit_mask;
		return;
	}
	if(bit_mask > 7)
	{
		// Byte transmission is complete //
		GPIOE_SET_PIN(DATA_CH);	// Stop bit
		bit_mask = 255;
		++dataTx_buf.ptr;
		return;
	}
	// Set of current bit of data to SER_DATA_CH pin //
	if (dataTx_buf.data[dataTx_buf.ptr] & (0x1 << bit_mask))
		GPIOE_SET_PIN(DATA_CH);
	else
		GPIOE_RESET_PIN(DATA_CH);
	++bit_mask; // Bit mask shift towards the MSB bit
}

void SerialGetData()
{
	static uint8_t bit_mask = 255; // Chooses current bit of data

	SerialReceiveBitFlag = BIT_RECEIVE_DONE;
	if (bit_mask == 255)	// Start bit
	{
		++bit_mask;
		return;
	}
	// Set of current bit of data to SER_DATA_CH pin //
	dataRx_buf.data[dataRx_buf.ptr] |= (((GPIOE->IDR & DATA_CH_MSK) >> DATA_CH) & 0x1) << bit_mask;
	// Reset bit operation pending flag //
	++bit_mask; // Bit mask shift towards the MSB bit
	if(bit_mask > 7)
	{
		// Byte reception is complete //
		bit_mask = 255;
		++dataRx_buf.ptr;
		NVIC_DisableIRQ(TIM3_IRQn);
		TIM_indx = 0;
		NVIC_EnableIRQ(EXTI15_10_IRQn);
		EXTI->IMR |= EXTI_IMR_MR11;
		EXTI->EMR |= EXTI_EMR_MR11;
	}
}

void Serial_trancieveData()
{
	if (DeviceMode == MASTER_MODE)
	{
		if (Buffer_Eof(&dataTx_buf))
		{
			SerialOut_stopSend();
			return;
		}
		if(SerialSetBitFlag == PENDING_BIT_SET)	// Main serial clock interrupt in occurred
			SerialSetData();
	}
	else // DeviceMode == SLAVE_MODE
	{
		if (SerialStartReceiveFlag == RECEIVE_START)
		{
			if ((GPIOE->IDR & (0x1 << DATA_SEND)) || Buffer_Eof(&dataRx_buf))
			{
				SerialIn_stopReceive();
				return;
			}
			if(SerialReceiveBitFlag == PENDING_BIT_RECEIVE)
				SerialGetData();
		}
	}
}

void SerialOut_stopSend()
{
	NVIC_DisableIRQ(TIM3_IRQn);
	SerialIn_init();
	WriteToFlash(&dataRx_buf.data[0], BUFFER_SIZE);
	DeviceMode = SLAVE_MODE;
	Buffer_reset(&dataTx_buf);
}

void SerialIn_stopReceive()
{
	NVIC_DisableIRQ(TIM3_IRQn);
	WriteToFlash(&dataRx_buf.data[0], BUFFER_SIZE);
	SerialIn_init();
	DeviceMode = SLAVE_MODE;
	Buffer_reset(&dataTx_buf);
}
