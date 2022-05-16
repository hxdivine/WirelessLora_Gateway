#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "def.h"

extern u8 ChipId[10];

extern u16 RepCnt;
extern u8 SysMin;

extern u16 ill;
extern u16 Airtemp;
extern u16 Airhumi;
extern u16 soilHumi;

extern u16 co2;
extern u16 Tvoc;
extern u16 soilTemp;
extern u8 tempWorkFreq;
extern u8 repTim;

void TotalInit(void);

void CLEDON(void);

void CLEDOFF(void);

void DebugDevInfo(void);

void GetGetCo2Data(void);

u8 IsDataAlarm(void);

void GetSensorData(void);

void LEDShow(void);

#endif
