#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "global.h"

// Pseudo circular buffer //
typedef struct buffer
{
	uint8_t data[BUFFER_SIZE];
	uint16_t ptr;
} BUFFER;

void 	BufferTx_init(BUFFER *B);
void	BufferRx_init(BUFFER *B);

uint8_t BufferTx_getNextByte();
uint8_t Buffer_Eof();
void Buffer_reset(BUFFER* B);

#endif
