#ifndef _MQTT_H_
#define _MQTT_H_

#include "gd32e10x.h"

#include "MQTTPacket.h"
#include "tapdev.h"

#define MQTT_BUF_OFFSET						80

typedef enum
{
	M_CLIENT = 0,
	M_CLIENTID,
	M_SUBSCRIBE,
	M_PUBLISH,
	
}MQTTDATA_TYPE;

typedef enum
{
	M_FREE = 0,											//空闲
	M_SUBTOPIC,												//MQTT订阅成功
	M_PINGBACK,											//心跳	
	M_CLOSE,										//等待关闭成功
	
}MQTT_STATE;

typedef struct
{
	u16 subTim;
	u16 pingTim;
	u16 pubTim;
	u16 freeTim;
	u16 tryCnt;
}MQTTTEMP_TIM;

void MqttSendDOResp(u8 *DevId,u8 *no,u16 mid,u8 mode);

u16 MqttSetRepBuf(u8 *InBuf,u8 *Index);

u16 SetRegValToBuf(u8 *TxBuf,u8 id,u8 type,u16 *RxBuf);

void MqttSetDOToWT2(u8 intDat,u8 no,u8 devId);

u8 MqttPro(u8 staCon,u8 mode);

void MqttOfflineToSaveRegDat(void);

u8 AnalyJsonFromCould(u8 *lp, u8 mode);

u8 AnalyJson(u8 *lp, u16 len,u8 mode);

#endif
