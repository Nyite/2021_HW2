#ifndef _PARALLEL_H_
#define _PARALLEL_H_

#include "global.h"

#define PAR_START	1
#define PAR_STOP	0

#define PAR_READY		0
#define PAR_NOT_READY	1

#define PAR_ENABLE		1
#define PAR_NOT_ENABLE	0

void ParallelIn_init();
void ParallelOut_init();
void ParallelTranceive();

void Parallel_appendData();
void Parallel_stopTranfer();
void Parallel_stopReception();

#endif
