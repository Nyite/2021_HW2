#include "buffer.h"

//static BUFFER dataTx_buf;	// Buffer to send data from

// Public function to initialize static buffers //
void BufferTx_init(BUFFER *B)
{
	B->ptr = 0;
	// Preset values to be transmitted //
	for (int i = 0; i < BUFFER_SIZE; i++)
		B->data[i] = i;
}

void BufferRx_init(BUFFER *B)
{
	B->ptr = 0;
}

// This function states if whole data transfer is completed //
uint8_t Buffer_Eof(BUFFER* B)
{
	if (B->ptr > BUFFER_SIZE - 1)
		return 1;
	return 0;
}

void Buffer_reset(BUFFER* B)
{
	B->ptr = 0;
}
