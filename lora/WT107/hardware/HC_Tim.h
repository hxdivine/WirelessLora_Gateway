
#ifndef _HC_TIM_H_
#define _HC_TIM_H_

#include "def.h"

typedef struct{
	u8 min;
	u8 sec;
}TIM_CLOCK;

void TimInit(void);

u8 GetSysSec(void);

u8 GetSysMin(void);

#endif
