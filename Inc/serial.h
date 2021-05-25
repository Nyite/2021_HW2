#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "global.h"

#define RECEIVE_START	1
#define RECEIVE_STOP	0

#define PENDING_BIT_SET 	1
#define BIT_SET_DONE		0

#define PENDING_BIT_RECEIVE	1	// Serial bit reception pending
#define BIT_RECEIVE_DONE	0	// Serial bit reception done

void SerialIn_init();
void SerialOut_init();
void SerialSetData();
void Serial_trancieveData();
void SerialOut_stopSend();
void SerialIn_stopReceive();

#endif
