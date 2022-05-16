
#ifndef _PUB_H_

#include "def.h"
#include "rtc.h"
#include "config.h"

extern u16 LocalBatVal;

extern u8 g_SigLevel;

extern u8_t	g_IMEI[15];
extern u8_t g_SIMCode[8];

extern int32_t g_flag;
extern int32_t g_Smsflag;
extern int32_t g_NoRespCnt;
extern u8 ModuleVer;

extern u8 g_SmsCnt;
extern u8 g_SmsTempCnt;
extern u8 NeedSetAPN;
extern u8 NeedOffline;
extern u8 SMSFlag;

extern _CALENDAR_ calendar;
//终端设备寄存器值
extern u16 DevRegVal[MAXREG];
////终端设备DO寄存器值
extern u8 DevAlarm[DEVICE_MAX];

#endif
