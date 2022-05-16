
#ifndef _RTC_H_

#define _RTC_H_

#include "def.h"

/*时钟结构体*/
typedef struct{

	u8 hour;
	u8 min;
	u8 sec;
	
	u16 w_year;
	u8 w_month;
	u8 w_day;
	u8 week;
	
}_CALENDAR_;


extern u8 timFlag;

void RTCInit(void);

u8 Is_Leap_Year(u16 year);

u8 RTC_Set(u8 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);

u8 RTC_Get(void);

u8 RTC_Get_Week(u16 year,u8 month,u8 day);

#endif
