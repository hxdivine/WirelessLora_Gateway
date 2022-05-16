
#ifndef _HUAWEIYUN_IOT_H_
#define _HUAWEIYUN_IOT_H_

#include "def.h"

u16 SetRegDatToHW(u8 *InBuf,u8 *Index);

u8 AnalyJsonFromHW(u8 *lp, u8 mode);

u8 HWyunSendResp(u16 mid,u8 *mBuf);

u8 HWyunSendDOResp(u8 *DevId,u8 *no,u8 *mBuf);

#endif
