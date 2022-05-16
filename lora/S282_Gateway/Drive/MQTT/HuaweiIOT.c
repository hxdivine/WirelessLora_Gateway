
#include <stdlib.h>

#include "loraDealDat.h"
#include "HuaweiIOT.h"
#include "pub.h"
#include "mqtt.h"

/*上报格式
{ 
   "msgType":"deviceReq", 
   "data": [ 
   { 
      "serviceId":"1245", 
      "serviceData":{ 
				"DO0": 1, 
				"TEMP31":30.3 
       },
      "eventTime":"20160503T121540Z" 
   }
	]
}
*/
u16 SetRegDatToHW(u8 *InBuf,u8 *Index)
{
	u16 len = 0;
	u8* str = malloc(30);         
	len = sprintf((char*)InBuf,"{\"msgType\":\"deviceReq\",\"data\":[{\"serviceId\":\"%s\",\"serviceData\":{",SysInfo.HWSerId);
	//HWSetRepBuf
	len += MqttSetRepBuf(&InBuf[len],Index);
	sprintf((char*)str, "\"%d%.2d%.2dT%.2d%.2d%.2dZ\"", calendar.w_year,calendar.w_month,calendar.w_day, calendar.hour, calendar.min, calendar.sec);
	len += sprintf((char*)&InBuf[len],",\"eventTime\":%s}]}",str);
	free(str);
	return len;
}
/*
	DO控制响应
{ 
   "msgType": "deviceRsp", 
   "mid": 1, 
   "errcode": 0, 
   "body": { //该部分可以省略
     "DO3_1": 0 
   } 
}
*/

u8 HWyunSendResp(u16 mid,u8 *mBuf)
{
	u8 len;
	len = sprintf((char*)mBuf,"{\"msgType\":\"deviceRsp\",\"mid\":%d,\"errcode\":0}",mid);
	return len;
}
/*
	云平台设置DO响应
*/
u8 HWyunSendDOResp(u8 *DevId,u8 *no,u8 *mBuf)
{
	u16 len = 0;
	u8* str = malloc(30);         
	len = sprintf((char*)mBuf,"{\"msgType\":\"deviceReq\",\"data\":[{\"serviceId\":\"%s\",\"serviceData\":{",SysInfo.HWSerId);
	for(u8 i = 0; i < DevId[10]; i++)
	{
		len += sprintf((char*)&mBuf[len],"\"DO%d_%d\":%d,",DevId[i],no[i],DevRegVal[(DevId[i]-1) * 7 + no[i] % 2]);
	}
	mBuf[len - 1] = '}';
	sprintf((char*)str, "\"%d%.2d%.2dT%.2d%.2d%.2dZ\"", calendar.w_year,calendar.w_month,calendar.w_day, calendar.hour, calendar.min, calendar.sec);
	len += sprintf((char*)&mBuf[len],",\"eventTime\":%s}]}",str);
	free(str);
	return len;
}
