
#ifndef _ALIYUN_IOT_H_
#define _ALIYUN_IOT_H_

#include "def.h"
//#include "cJSON.h"

u16 SetRegDatToAli(u8 *InBuf,u8 *Index);

u8 AnalyJsonFromAli(u8 *lp, u8 mode);

u8 AliyunSendDOResp(u8 *DevId,u8 *no,u8 *mBuf);

#endif
