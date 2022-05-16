
#ifndef _DEAL_DAT_H_
#define _DEAL_DAT_H_

#include "def.h"

#define SEND_DATA_MAX		128

typedef struct{
	
	u8 recBuf[SEND_DATA_MAX];
	u8 recLen;
	
}OVER_ALLDATA_FRAME;

extern OVER_ALLDATA_FRAME overAllData_Fram;
//void DisPlay(u8 ptr);
extern u8 freqChangeFlag;
extern u8 freqChangeTim;
extern u8 tempWorkfreq;

void PcCmdPro(void);

void CheckDevWorkFreq(void);

void PrintNewDev(void);
#endif
