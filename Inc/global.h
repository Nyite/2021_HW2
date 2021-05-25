#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stm32f4xx.h"

// Device modes //
#define MASTER_MODE	0
#define SLAVE_MODE	1

#define BUFFER_SIZE	256

enum FREQ {
	FREQ_0		=	0x0,
	FREQ_100	=	0x1,
	FREQ_500	= 	0x2,
	FREQ_1K		=	0x3,
	FREQ_10K	=	0x4,
	FREQ_20K	=	0x5,
	FREQ_50K	= 	0x6, // Not working with 8MHz
	FREQ_100K	=	0x7, // Not working with 8MHz
};

#define SER_TRANFER_RATE FREQ_20K

// Serial bus PE ports defines //
#define	DATA_SEND	10
#define DATA_CH		11
#define DATA_CH_MSK 0x1 << DATA_CH
#define FREQ_POS	12
#define FREQ_MASK	0x7 << FREQ_POS

// Parallel bus PE pins defines //
#define PAR_BUS_MSK 0xFF // PE0-PE7 bus line
#define PAR_ENABLE_PIN	8
#define PAR_READY_PIN	9

#endif
