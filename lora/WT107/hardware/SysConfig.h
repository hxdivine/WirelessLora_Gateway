#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "def.h"

#define E2PROM_ADDR     0x1000

typedef struct
{
  u8 DevId;                      //???ID
  u8 TarId[6];                   //????ID
	u8 Uuid[6];
  u8 RepCycle;                   //????   ?????
  u8 workFreq;                    //????
  u8 IsWrite;                      //???
	u16 HeightLimit[7];
	u16 LowLimit[7];
	
}DEVINFO;

extern DEVINFO DevInfo;

void SetDevInfoDef(void);

void SaveDevInfo(DEVINFO *dev);

void ReadDevInfo(DEVINFO *dev);

#endif
