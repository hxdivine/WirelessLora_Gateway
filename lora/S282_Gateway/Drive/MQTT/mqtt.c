#include "systick.h"
#include <stdlib.h>
#include "string.h"

#include "mqtt.h"
#include "GSM234G.h"
#include "config.h"
#include "hardware.h"
#include "EthNet.h"
#include "loraDealDat.h"
#include "iwdg.h"
#include "pub.h"

#include "AliyunIOT.h"
#include "HuaweiIOT.h"

//typedef long int time_t;
u8 m_mqttBuf[MQTT_PWD * 3]; //
u8 *m_sendBuf;

//订阅 发布的主题
MQTTString topicString = MQTTString_initializer;
const u8 mqttDef[5] = "MQTT";
u16 PacketId = 1;

u8 m_State = M_FREE;
/*
	获取MQTT连接时的数据
	客户端ID 用户名 密码
*/
void GetMqttConnectData(u8 *InBuf,u8 mode)
{
	u8 actionLen = 0;
	switch(mode)
	{
		case M_CLIENT:
			if(SysInfo.AliyunSet == 1)
			{
				actionLen += CopyStr(InBuf,SysInfo.MqttDeviceID,MQTT_PWD);
				CopyStr(&InBuf[actionLen],"|securemode=3,signmethod=hmacmd5|",34);
			}
			else
			{
				CopyStr(InBuf,SysInfo.MqttDeviceID,MQTT_PWD);
			}
			CopyStr(&InBuf[MQTT_PWD],SysInfo.MqttUserName,MQTT_PWD);
			CopyStr(&InBuf[MQTT_PWD * 2],SysInfo.MqttPwd,MQTT_PWD);
		break;
		case M_CLIENTID:
			if(SysInfo.AliyunSet == 1)
			{
				actionLen += CopyStr(InBuf,SysInfo.MqttDeviceID,MQTT_PWD);
				CopyStr(&InBuf[actionLen],"|securemode=3,signmethod=hmacmd5|",34);
			}
			else
			{
				CopyStr(InBuf,SysInfo.MqttDeviceID,MQTT_PWD);
			}
		break;
		case M_SUBSCRIBE:
			if(SysInfo.AliyunSet == 1)
			{
				actionLen += CopyStr(InBuf,"/sys/",5);
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.ProductKey,MAXGPRSUSR);
				InBuf[actionLen++] = '/';
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.DeviceName_Could,HW_DEVSECRET_LEN);
				actionLen += CopyStr(&InBuf[actionLen],"/thing/service/property/set",33);
			}
			else if(SysInfo.AliyunSet == 0x10)
			{//  /huawei/v1/devices/60f79a4eaa3bcc028737a53a_12222/command/json
				actionLen += CopyStr(InBuf,"/huawei/v1/devices/",20);
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.HWDevId,MAXGPRSIP);
				actionLen += CopyStr(&InBuf[actionLen],"/command/json",14);
				
			}
			else
			{
				CopyStr(InBuf,SysInfo.MqttSubTopic,MAXREGDAT);
			}
		break;
		case M_PUBLISH:
			if(SysInfo.AliyunSet == 1)
			{
				actionLen += CopyStr(InBuf,"/sys/",5);
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.ProductKey,MAXGPRSUSR);
				InBuf[actionLen++] = '/';
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.DeviceName_Could,HW_DEVSECRET_LEN);
				actionLen += CopyStr(&InBuf[actionLen],"/thing/event/property/post",30);
			}
			else if(SysInfo.AliyunSet == 0x10)
			{	//  /huawei/v1/devices/60f79a4eaa3bcc028737a53a_12222/data/json
				actionLen += CopyStr(InBuf,"/huawei/v1/devices/",20);
				actionLen += CopyStr(&InBuf[actionLen],SysInfo.HWDevId,MAXGPRSIP);
				actionLen += CopyStr(&InBuf[actionLen],"/data/json",12);
			}
			else
			{
				CopyStr(InBuf,SysInfo.MqttPubTopic,MAXREGDAT);
			}
		break;
		default:
			break;
	}
}
/*
	MQTT 连接
	入参 mode: 0 GPRS网络 1 以太网络
*/
void MqttConnect(u8 mode)
{
	u8 i;
	u16 len;
	m_sendBuf = malloc(1000);
	MQTTPacket_connectData mqttConnDat = MQTTPacket_connectData_initializer;
	GetMqttConnectData(m_mqttBuf,M_CLIENT);
	if(SysInfo.MqttDeviceID[MQT_TXT_DATLEN] == 0)
	{
		//设备ID为空时 使用IMEI信号作为设备ID
		for(i = 0; i < 15; i++)
		{
			m_mqttBuf[i] = g_IMEI[i];
		}
		m_mqttBuf[i] = '\0';
	}
	if(SysInfo.MqttUserName[MQT_TXT_DATLEN] == 0)
	{
		//MQTT用户名为空 设置为MQTT
		for(i = 0; i < 4; i++)
		{
			m_mqttBuf[i + MQTT_PWD] = mqttDef[i];
		}
		m_mqttBuf[i] = '\0';
	}
	
	if(SysInfo.MqttPwd[MQT_TXT_DATLEN] == 0)
	{
		//MQTT用户密码为空 设置为MQTT
		for(i = 0; i < 4; i++)
		{
			m_mqttBuf[i + MQTT_PWD * 2] = mqttDef[i];
		}
		m_mqttBuf[i] = '\0';
	}
	
	mqttConnDat.clientID.cstring = (char *)m_mqttBuf;
	if(SysInfo.AliyunSet == 0x01 && SysInfo.MqttRepTim < 60)
	{
		mqttConnDat.keepAliveInterval = 60;
	}
	else
	{
		mqttConnDat.keepAliveInterval = SysInfo.MqttRepTim;
	}
	mqttConnDat.cleansession = 1;
	mqttConnDat.username.cstring = (char *)&m_mqttBuf[MQTT_PWD];
	mqttConnDat.password.cstring = (char *)&m_mqttBuf[MQTT_PWD * 2];

	len = MQTTSerialize_connect(m_sendBuf,MQTT_PWD * 3,&mqttConnDat);
	if(mode == 0)
	{
		SendGprsDat(m_sendBuf,len);
	}
	else
	{
		UipClient2Server(m_sendBuf,len);
	}
	free(m_sendBuf);
}

/*
	MQTT 断开连接
	入参 mode: 0 GPRS网络 1 以太网络
*/
void MqttDisConnect(u8 mode)
{
	u16 len;
	len = MQTTSerialize_disconnect(m_mqttBuf,MQTT_PWD * 3);
	
	if(mode == 0)
	{
		SendGprsDat(m_mqttBuf,len);
	}
	else
	{
		UipClient2Server(m_mqttBuf,len);
	}
}

/*
	MQTT Ping
	防止MQTT断开连接
*/
void MqttPingReq(u8 mode)
{
	u16 len;
	len = MQTTSerialize_pingreq(m_mqttBuf,MQTT_PWD * 3);
	if(mode == 0)
	{
		SendGprsDat(m_mqttBuf,len);
	}
	else
	{
		UipClient2Server(m_mqttBuf,len);
	}
}
/*
	MQTT 订阅主题
*/
void MqttSubscribe(u8 mode)
{
	//服务质量
	int qos = 0;
	u16 len,i;
	m_sendBuf = malloc(1000);
	GetMqttConnectData(m_mqttBuf,M_SUBSCRIBE);
	if(!ISASCCHAR(m_mqttBuf[0]))
	{//tlink平台要求订阅主题为，序列号+“/+”
		GetMqttConnectData(m_mqttBuf,M_CLIENTID);
		if(!ISASCCHAR(m_mqttBuf[0]))
		{
			for(i = 0; i < 15; i++)
			{
				m_mqttBuf[i] = g_IMEI[i];
			}
			m_mqttBuf[i] = '\0';
		}
		for(i = 0; i < 60; i++)
		{
			if(!ISASCCHAR(m_mqttBuf[i]))
			{
				break;
			}
		}
		m_mqttBuf[i++] = '/';
		m_mqttBuf[i++] = '+';
		m_mqttBuf[i++] = '\0';
	}
	
	topicString.cstring = (char *)m_mqttBuf;

	len = MQTTSerialize_subscribe(m_sendBuf,MQTT_PWD * 3,0,PacketId,1,&topicString,&qos);
	PacketId++;
	if(mode == 0)
	{
		SendGprsDat(m_sendBuf,len);
	}
	else
	{
		UipClient2Server(m_sendBuf,len);
	}
	free(m_sendBuf);
}

/*
	MQTT 发布主题
*/
void MqttPublish(u8 *InBuf,u16 len,u8 mode)
{
	u8 qos = 1;
	u16 i;
	m_sendBuf = malloc(1000);
	GetMqttConnectData(m_mqttBuf,M_PUBLISH);
	if(!ISASCCHAR(m_mqttBuf[0]))
	{//没有填写发布的主题
		GetMqttConnectData(m_mqttBuf,M_CLIENTID);
		if(!ISASCCHAR(m_mqttBuf[0]))
		{
			for(i = 0; i < 15; i++)
			{
				m_mqttBuf[i] = g_IMEI[i];
			}
			m_mqttBuf[i] = '\0';
		}		
	}
	memset(m_sendBuf,0,1000);
	topicString.cstring = (char *)m_mqttBuf;
	S282_Log(InBuf);
	len = MQTTSerialize_publish(m_sendBuf, 1000, 0, qos, 0, PacketId, topicString, InBuf, len);
	PacketId++;
	if(mode == 0)
	{
		SendGprsDat(m_sendBuf,len);
	}
	else
	{
		UipClient2Server(m_sendBuf,len);
	}
	free(m_sendBuf);
}

/*
	网络下线保存数据  在网络正常时 上报数据
	适用MQTT协议
*/
void MqttOfflineToSaveRegDat(void)
{
		//用于历史数据保存时间
	static u32 historyRepTim = 0;
	if(ABSVALUE(GetSecCnt(),historyRepTim) > SysInfo.MqttRepTim && SysInfo.MqttRepData)
	{//一次上报周期 保存一次历史数据
		historyRepTim = GetSecCnt();
		for(u8 i = 0; i < DEVICE_MAX; i++)
		{
			if(DevInfo[i].isExist == 1)
			{
		//		SetHistory(historyRepPtr);
				SaveHistoryRep(i,DevInfo[i].Type,1);
			}
		}
	}
}


/*
	MQTT 补传历史数据
*/
u16 MqttHistoryRep(u8 *InBuf)
{
	u16 len = 0;
	u8 tempId;
	u8* str = malloc(40);
	len = sprintf((char*)InBuf,"{\"sensorDatas\":[");
	ReadHistroyRep((u8*)&History,--historyRepCnt);
	tempId = History.DeviceId;
	len += SetRegValToBuf(&InBuf[len],History.DeviceId,History.DevType,&History.Temp);
	if(historyRepCnt > 0)
	{
		ReadHistroyRep((u8*)&History,historyRepCnt-1);
		if(tempId != History.DeviceId)
		{
			len += SetRegValToBuf(&InBuf[len],History.DeviceId,History.DevType,&History.Temp);
			--historyRepCnt;
		}
	}
	History.seq = 0xFFFF;
	SaveHistoryRep(0,DEV_WT107,0);
	sprintf((char*)str, "%.2d:%.2d:%.2d %.2d/%.2d/%d UTC", History.hour, History.min, History.sec, History.mon ,History.day,History.year);
	len += sprintf((char*)&InBuf[len - 1],"],\"time\":\"%s\",\"retransmit\":\"enable\"}",str);
	free(str);
	return len;
}

/*
	获取需要上传的数据 
*/
u16 MqttGetRegData(u8 *InBuf,u8 *Index)
{
	u16 len = 0;
	u8* str = malloc(40);
	len = sprintf((char*)InBuf,"{\"sensorDatas\":[");
	len += MqttSetRepBuf(&InBuf[len],Index);
	sprintf((char*)str, "%.2d:%.2d:%.2d %.2d/%.2d/%d UTC", calendar.hour, calendar.min, calendar.sec, calendar.w_month,calendar.w_day,calendar.w_year);
	len += sprintf((char*)&InBuf[len],",\"time\":\"%s\"}",str);

	free(str);
	return len;
}
/*
	向云平台发送JSON数据
*/
void MqttSendRegData(u8 *sta,u8 *delay,MQTTTEMP_TIM *temp_Tim,u8 mode)
{
	static u8 firstFlag = 0;
	static u8 sendPtr = 0;
	if(*sta)
	{
		u8 *mBuf = (u8*)malloc(900);
		u16 len = 0;
		if(mBuf == NULL)
		{
			return;
		}
		NETLED_ENABLE;
		
		if(sendPtr == 0)
		{//第一次进来
			temp_Tim->freeTim = GetSecCnt();
			temp_Tim->pubTim = GetSecCnt();
		}
		if(SysInfo.AliyunSet == 1)
		{
			len = SetRegDatToAli(mBuf,&sendPtr);
		}
		else if(SysInfo.AliyunSet == 0x10)
		{
			len = SetRegDatToHW(mBuf,&sendPtr);
		}
		else
		{
			len = MqttGetRegData(mBuf,&sendPtr);
		}
		MqttPublish(mBuf,len,mode);
		*delay = 120;

		if(sendPtr == 0)
		{
			*sta = 0;
		}	
		if(SysInfo.MqttRepData)
		{
			if(firstFlag== 0)
			{
				firstFlag = 1;
				//首次发送后 发送历史记录
				while(historyRepCnt > 0)
				{
					len = MqttHistoryRep(mBuf);
					MqttPublish(mBuf,len,mode);
					delayMs(40);
					WatchDogFeed();
				}
				SaveUserInfo();
			}
		}
		else
		{//防止主机持续在线 关了补传后 又开
			firstFlag = 0;
		}
	
		free(mBuf);
	}
}



/*
	WT102下发设置
*/
void MqttSetDOToWT2(u8 intDat,u8 no,u8 devId)
{
	u8 option;
	if(intDat)
	{//DO 开  DOUTN(1, no);
		option = 1;
	}
	else
	{//DO 关 DOUTN(0, no);
		option = 0;
	}
	option = no % 2? option | 0x80:option;
	DownLoad2TerminalWT2(devId,1,option);
	if(DB_Flag)
			printf("mqtt control relay%d: %s\r\n", no, intDat?"ON":"OFF");
}

/*
	DO控制响应
*/
void MqttSendDOResp(u8 *DevId,u8 *no,u16 mid,u8 mode)
{
	u8 *mBuf = (u8*)malloc(256);
	u8 *str = (u8*)malloc(40);
	u8 len;
	if(SysInfo.AliyunSet == 1)
	{
		len = AliyunSendDOResp(DevId,no,mBuf);
	}
	else if(SysInfo.AliyunSet == 0x10)
	{
		len = HWyunSendResp(mid,mBuf);
		MqttPublish(mBuf,len,mode);
		len = HWyunSendDOResp(DevId,no,mBuf);
	}
	else
	{
		sprintf((char*)str, "%.2d:%.2d:%.2d %.2d/%.2d/%d UTC", calendar.hour, calendar.min, calendar.sec, calendar.w_month,calendar.w_day,calendar.w_year);
		len = sprintf((char*)mBuf,"{\"sensorDatas\":[");
		len += sprintf((char*)&mBuf[len],"{\"flag\":\"DO%d_%d\",\"switcher\":%d}],\"time\":\"%s\"}",DevId[0],no[0],DevRegVal[(DevId[0]-1) * 7 + no[0] % 2],str);
	}
	MqttPublish(mBuf,len,mode);
	free(mBuf);
	free(str);
}


void SetMQTT_Tim(MQTTTEMP_TIM *temp_Tim,u8 mode)
{
	temp_Tim->pingTim = GetSecCnt();
	temp_Tim->freeTim = GetSecCnt();
	temp_Tim->tryCnt++;
	MqttPingReq(mode);
	if(temp_Tim->tryCnt >= 2)
	{
		temp_Tim->tryCnt = 0;
		m_State = M_CLOSE;
	}
}
/*
	MQTT数据格式分析
*/
u8 MQTT_ClientString(u8 *sta,MQTTTEMP_TIM *temp_Tim,unsigned char* buf, int buflen, u8 mode)
{
	int index = 0;
	int rem_length = 0;
	u8 header;
	int strindex = 0;
	u8 firstConnect = 0;
	strindex = strindex;//warning
	header = buf[index++];
	index += MQTTPacket_decodeBuf(&buf[index], &rem_length);
	switch (header>>4)
	{
		//connect ack
		//20 02 
		//00 		当前会话
		//00    连接返回码
		case CONNACK:
		{
			unsigned char sessionPresent, connack_rc;
			if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0)
			{
				S282_Log("not connect");
			}
			else
			{
					*sta = M_SUBTOPIC;
					temp_Tim->freeTim = GetSecCnt();
			}
			break;
		}
		
		//发布消息  home/garden/fountain  ??ON
		//30 18 
		//00 14 68 6f 6d 65 2f 67 61 72 64 65 6e 2f 66 6f 75 6e 74 61 69 6e 
		//4f 4e
		case PUBLISH:
		{
			unsigned char dup, retained, *payload;
			int qos, payloadlen;
			MQTTString topicName = MQTTString_initializer;
			
			if (MQTTDeserialize_publish(&dup, &qos, &retained, &PacketId, &topicName,
					&payload, &payloadlen, buf, buflen) == 1)
			{
				WatchDogFeed();
				AnalyJson(payload, payloadlen,mode);
			}
			break;
		}
		
		//40 02 00 01		publish ack????
		//50 02 00 02		PUBREC ????
		//60 02 00 02		PUBREL ????
		//70 02 00 02		PUBCOMP ????
		case PUBACK:
		case PUBREC:
		case PUBREL:
		case PUBCOMP:
		{
				temp_Tim->pingTim = GetSecCnt();
				temp_Tim->tryCnt = 0;
				temp_Tim->freeTim = GetSecCnt();
			break;
		}
		
		//subscribe ack  
		//90 03 
		//00 01 	//?????
		//01			//?? qos1
		case SUBACK:
		{
			unsigned short packetid;
			int maxcount = 1, count = 0;
			int grantedQoSs[1];
			if (MQTTDeserialize_suback(&packetid, maxcount, &count, grantedQoSs, buf, buflen) == 1)
			{
			//	if((grantedQoSs[0] & 0x80)==0)
				{
					*sta = M_PINGBACK;
					temp_Tim->pingTim = GetSecCnt();
					temp_Tim->pubTim = GetSecCnt() - 6;
					temp_Tim->freeTim = GetSecCnt();
					temp_Tim->tryCnt = 0;
					firstConnect = 1;
				}
		//		else
				{
				//	*sta = M_CLOSE;
				}
			}
			break;
		}
		
		//unsubscribe ack
		//b0 02 
		//00 02
		case UNSUBACK:
		{
			break;
		}
		
		//c0 00		ping request
		//d0 00		pint response
		//e0 00		disconnect req
		case PINGREQ:
		case PINGRESP:
		{
		//	if((header>>4) == PINGRESP)
			{
				temp_Tim->pingTim = GetSecCnt();
				temp_Tim->tryCnt = 0;
				temp_Tim->freeTim = GetSecCnt();
			}
			break;
		}
		case DISCONNECT:
			*sta = M_FREE;
		S282_Log("disconnect");
		break;
	}
	return firstConnect;
}

/*
	连接MQTT
	支持蜂窝网 以太网
*/
u8 MqttPro(u8 staCon,u8 mode)
{
	static MQTTTEMP_TIM temp_Tim = {0,0,0,0};
	static u8 delay = 0;
	static u8 sta = 0;
	u8 rec = 0;
	if(delay)
	{
		delay--;
		return 0;
	}
	
	if(staCon == 0)
	{//掉线重连
		m_State = M_FREE;
		sta = 0;
	}
	
	MqttSendRegData(&sta,&delay,&temp_Tim,mode);
	WatchDogFeed();
	switch(m_State)
	{
		case M_FREE:
			if(ABSVALUE(GetSecCnt(),temp_Tim.pingTim) > 3)
			{
				temp_Tim.pingTim = GetSecCnt();
				temp_Tim.freeTim = GetSecCnt();
				MqttConnect(mode);
				temp_Tim.tryCnt++;
				if(temp_Tim.tryCnt >= 5)
				{
					temp_Tim.tryCnt = 0;
					temp_Tim.pingTim = 0;
					rec = 1;
					sta = 0;
					m_State = M_CLOSE;
				}
			}
		break;
		case M_SUBTOPIC:
			if(ABSVALUE(GetSecCnt(),temp_Tim.subTim) > 3)
			{//3S
				temp_Tim.subTim = GetSecCnt();
				temp_Tim.freeTim = GetSecCnt();
				MqttSubscribe(mode);
				temp_Tim.tryCnt++;
				if(temp_Tim.tryCnt >= 5)
				{
					temp_Tim.tryCnt = 0;
					m_State = M_CLOSE;
					sta = 0;
				}
			}
		break;
		case M_PINGBACK:
			if(SysInfo.AliyunSet == 0x01)
			{
				if(ABSVALUE(GetSecCnt(),temp_Tim.freeTim) > 40)
				{//6S
					SetMQTT_Tim(&temp_Tim,mode);
				}		
			}
			else
			{
				if(ABSVALUE(GetSecCnt(),temp_Tim.freeTim) > 8)
				{//6S
					SetMQTT_Tim(&temp_Tim,mode);
				}	
			}
		break;
		case M_CLOSE:
		{
			MqttDisConnect(mode);
			rec = 1;
			sta = 0;
		}
		break;
		default:
			break;
	}
	
	if(m_State == M_PINGBACK)
	{
		if(SysInfo.AliyunSet == 0x01)
		{//阿里云心跳要在60 - 300s  太短会掉线 太长不重连 
			if(ABSVALUE(GetSecCnt(),temp_Tim.pingTim) > 30)
			{//取个30s  
				temp_Tim.pingTim = GetSecCnt();
				MqttPingReq(mode);
			}
		}
		else
		{
			if(ABSVALUE(GetSecCnt(),temp_Tim.pingTim) > 5)
			{
				temp_Tim.pingTim = GetSecCnt();
				MqttPingReq(mode);
			}
		}
		if(ABSVALUE(GetSecCnt(),temp_Tim.pubTim) > (SysInfo.MqttRepTim - 1))
		{//提前1S发送 
			sta = 1;
			temp_Tim.pubTim = GetSecCnt();
			temp_Tim.freeTim = GetSecCnt();
		}
	}
	WatchDogFeed();
	if(mode == 1)
	{
		if(g_RecEthLen)
		{//接收到数据
			temp_Tim.freeTim = GetSecCnt();
			if(MQTT_ClientString(&m_State,&temp_Tim,tcp_client_Recbuf,g_RecEthLen,1))
			{
				sta = 1;
			}
			g_RecEthLen = 0;
		}
	}
	else
	{
		if(g_flag & GPRSPACKOK)
		{//接收到数据
			temp_Tim.freeTim = GetSecCnt();
			if(MQTT_ClientString(&m_State,&temp_Tim,gprs_data_Frame.recBuf,gprs_data_Frame.recLen,0))
			{
				sta = 1;
			}
			g_flag &= ~GPRSPACKOK;
		}
	}
	return rec;
}
