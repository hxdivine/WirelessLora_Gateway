#include "mqtt.h"
#include "config.h"
#include <stdlib.h>
#include "string.h"
#include "hardware.h"
#include "cJSON.h"
#include "pub.h"
/*
	华为云格式
		{	 
   "msgType": "cloudReq", 
   "serviceId": "******", 
   "mid": 1, 响应需要返回
   "cmd": "LEVEL", 
   "paras": { 
     "DO3_1": 0 
   } 
	}
*/
/*
	阿里云格式
	{"method":"thing.service.property.set",
	"id":"1117987653",
		"params":{"DO3_0":1},
		"version":"1.0.0"}
*/

/*
	分析阿里云 华为云下发的JSON数据
*/
u8 AnalyJsonFromCould(u8 *lp, u8 mode)
{
	cJSON *root = NULL;
	cJSON *item = NULL;			//cjson对象
	int ptr;
	u8 index = 0;
	u16 mid = 1;
	u8 *DevId = malloc(11);
	u8 *no = malloc(10);
	DevId[10] = 0;
	/* 
	{	 
   "msgType": "cloudReq", 
   "serviceId": "******", 
   "mid": 1, 响应需要返回
   "cmd": "LEVEL", 
   "paras": { 
     "DO3_1": 0 
   } 
	}
		*/
	root = cJSON_Parse((char*)lp); 
	if(!root)
	{
		goto analyjsonend;
	}
	if(SysInfo.AliyunSet == 0x10)
	{
		item = cJSON_GetObjectItem(root, "mid");
		if(!item)
		{
			S282_Log("mqtt can not find mid");
			goto analyjsonend;
		}	
		mid = item->valueint;
		item = cJSON_GetObjectItem(root, "paras");
	}
	else
	{
		item = cJSON_GetObjectItem(root, "params");
	}
	
	if(!item)
	{
		S282_Log("mqtt can not find params");
		goto analyjsonend;
	}
	if(!item->child)
	{
		goto analyjsonend;
	}
	do
	{
		ptr = strncasecmp("DO", item->child->string, 2);
		if(ptr == 0)		//DO控制
		{
			if(!(item->child->string[2] >= '0' && item->child->string[2] <= '9'))		//范围在0 - 3
			{
				S282_Log("mqtt data format error");
				goto analyjsonend;
			}
			DevId[index] = item->child->string[2] - '0';
			no[index] = item->child->string[4] - '0';
			if(item->child->string[3] >= '0' && item->child->string[3] <= '9')
			{
				DevId[index] = DevId[index] * 10 + item->child->string[3] - '0';
				no[index] = item->child->string[5] - '0';
			}
	
			MqttSetDOToWT2(item->child->valueint,no[index],DevId[index]);
			index += 1;
		}
		item->child = item->child->next;
	}while(item->child);
	if(index)
	{
		DevId[10] = index;
		MqttSendDOResp(DevId,no,mid,mode);
	}
	analyjsonend:
	free(DevId);
	free(no);
	cJSON_Delete(item);
	return index;
}
/*
	JSON格式数据分析
*/
u8 AnalyJson(u8 *lp, u16 len,u8 mode)
{
	cJSON *root = NULL;
	cJSON *item = NULL;			//cjson对象
	cJSON *itemchild = NULL;	//cjson对象
	int ptr;
	u8 index = 0;
	u8 *DevId = malloc(11);
	u8 *no = malloc(10);
	DevId[10] = 0;
	
	if(SysInfo.AliyunSet > 0)
	{
		if(!AnalyJsonFromCould(lp,mode))
		{
			goto analyjsonend;
		}
	}
	else
	{
		root = cJSON_Parse((char*)lp); 
		//{"sensorDatas":[{"sensorsId":216663,"switcher":1,"flag":"DO0"}],"down":"down"}
		if(!root)
		{
			S282_Log("mqtt Json format Error");
			goto analyjsonend;
		}
		item = cJSON_GetObjectItem(root, "sensorDatas");
		if(!item)
		{
			S282_Log("mqtt can not find sensorDatas");
			goto analyjsonend;
		}
	
		if(item->type != cJSON_Array)
		{
			goto analyjsonend;
		}
		item = cJSON_GetArrayItem(item, 0);
		if(!item)
		{
			S282_Log("mqtt can not find Item");
			goto analyjsonend;
		}
		itemchild = cJSON_GetObjectItem(item, "flag");
		if(!itemchild)
		{
			S282_Log("mqtt can not find flag");
			goto analyjsonend;
		}
		ptr = strncasecmp("DO", itemchild->valuestring, 2);
		if(ptr == 0)		//DO控制
		{
			//{"sensorDatas":[{"sensorsId":216663,"switcher":1,"flag":"DO0"}],"down":"down"}
			if(!(itemchild->valuestring[2] >= '0' && itemchild->valuestring[2] <= '9'))		//范围在0 - 3
			{
				S282_Log("mqtt data format error");
				goto analyjsonend;
			}
		
			DevId[index] = itemchild->valuestring[2] - '0';
			no[index] = itemchild->valuestring[4] - '0';
			if(itemchild->valuestring[3] >= '0' && itemchild->valuestring[3] <= '9')
			{
				DevId[index] = DevId[index] * 10 + itemchild->valuestring[3] - '0';
				no[index] = itemchild->valuestring[5] - '0';
			}
			itemchild = cJSON_GetObjectItem(item, "switcher");
			if(!itemchild)
			{
				S282_Log("mqtt can not find switcher");
				goto analyjsonend;
			}
			MqttSetDOToWT2(itemchild->valueint,no[index],DevId[index]);
			index = 1;
			DevId[10] = index;
			MqttSendDOResp(DevId,no,0,mode);
		}
	}

analyjsonend:
	free(DevId);
	free(no);
	cJSON_Delete(root);
	return index;
}

/*
	阿里云 华为云设置寄存器数值
*/
u16 OtherSetRegValToBuf(u8 *TxBuf,u8 id,u8 type,u16 *RxBuf)
{
	u16 len = 0;
	switch(type)
	{
		case DEV_WT107:	
			len += sprintf((char*)&TxBuf[len],"\"TEMP%d\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"-%d.%d,",(RxBuf[0]-0x8000) / 10,(RxBuf[0]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[0] / 10,RxBuf[0] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"HUMI%d\":%d.%d,",id,RxBuf[1] / 10,RxBuf[1] % 10);
			len += sprintf((char*)&TxBuf[len],"\"ILLU%d\":%d,",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"\"CO2%d\":%d,",id,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"\"TVOC%d\":%d,",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"\"SOILTEMP%d\":",id);			
			if(RxBuf[5] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"-%d.%d,",(RxBuf[5]-0x8000) / 10,(RxBuf[5]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[5] / 10,RxBuf[5] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"SOILHUMI%d\":%d.%d,",id,RxBuf[6] / 10,RxBuf[6] % 10);
		break;
		case DEV_WT102:
			len += sprintf((char*)&TxBuf[len],"\"DO%d_0\":%d,",id,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"\"DO%d_1\":%d,",id,RxBuf[1]);
		break;
		case DEV_WT108:
			len += sprintf((char*)&TxBuf[len],"\"SOILPH%d\":%d.%d,",id,RxBuf[0] / 100,RxBuf[0] % 100);
		break;
		case DEV_WT109:
			len += sprintf((char*)&TxBuf[len],"\"SOILEC%d\":%d,",id,RxBuf[0]);
		break;
		case DEV_WT104:
		case DEV_WT103:
			len += sprintf((char*)&TxBuf[len],"\"TEMP%d\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"-%d.%d,",(RxBuf[0]-0x8000) / 10,(RxBuf[0]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[0] / 10,RxBuf[0] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"HUMI%d\":%d.%d,",id,RxBuf[1] / 10,RxBuf[1] % 10);	
			len += sprintf((char*)&TxBuf[len],"\"BAT%d\":%d,",id,RxBuf[2]);
		break;
		case DEV_WT105:
		case DEV_WT106:
			len += sprintf((char*)&TxBuf[len],"\"TEMP%d\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"-%d.%d,",(RxBuf[0]-0x8000) / 10,(RxBuf[0]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[0] / 10,RxBuf[0] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"BAT%d\":%d,",id,RxBuf[1]);
		break;
		case DEV_WT110:
			len += sprintf((char*)&TxBuf[len],"\"SOILTEMP%d\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"-%d.%d,",(RxBuf[0]-0x8000) / 10,(RxBuf[0]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[0] / 10,RxBuf[0] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"SOILHUMI%d\":%d.%d,",id,RxBuf[1] / 10,RxBuf[1] % 10);
			len += sprintf((char*)&TxBuf[len],"\"SOILPH%d\":%d.%d,",id,RxBuf[2] / 100,RxBuf[2] % 100);
			len += sprintf((char*)&TxBuf[len],"\"SOILEC%d\":%d,",id,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"\"SOILN%d\":%d,",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"\"SOILP%d\":%d,",id,RxBuf[5]);
			len += sprintf((char*)&TxBuf[len],"\"SOILK%d\":%d,",id,RxBuf[6]);
		break;
		case DEV_WT101:
			len += sprintf((char*)&TxBuf[len],"\"DI%d_0\":%d,",id,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"\"DI%d_1\":%d,",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"\"DI%d_2\":%d,",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"\"DI%d_3\":%d,",id,RxBuf[3]);
		break;
		case DEV_WT111:
			len += sprintf((char*)&TxBuf[len],"\"TEMP%d\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",(RxBuf[0]-0x8000) / 10,(RxBuf[0]-0x8000) % 10);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d.%d,",RxBuf[0] / 10,RxBuf[0] % 10);
			}
			len += sprintf((char*)&TxBuf[len],"\"HUMI%d\":%d.%d,",id,RxBuf[1] / 10,RxBuf[1] % 10);
			len += sprintf((char*)&TxBuf[len],"\"ILLU%d\":%d,",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"\"WINDSPEED%d\":%d.%d,",id,RxBuf[3] / 10,RxBuf[3] % 10);
			len += sprintf((char*)&TxBuf[len],"\"WINDDIRECTIONVALUE%d\":%d,",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"\"WINDDIRECTION%d\":%d,",id,RxBuf[5]);	
		break;
		default:
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,0,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,1,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,2,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,3,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,4,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,5,RxBuf[5]);
			len += sprintf((char*)&TxBuf[len],"\"DEV%d_%d\":%d,",id,6,RxBuf[6]);
			break;
	}
	return len;
}

/*
	金鸽云设置寄存器数值
*/
u16 SetRegValToBuf(u8 *TxBuf,u8 id,u8 type,u16 *RxBuf)
{
	u16 len = 0;
	switch(type)
	{
		case DEV_WT107:	
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"TEMP%d\",\"value\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]-65536);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]);
}
			
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"HUMI%d\",\"value\":%d},",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"ILLU%d\",\"value\":%d},",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"CO2%d\",\"value\":%d},",id,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"TVOC%d\",\"value\":%d},",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILTEMP%d\",\"value\":",id);			
			if(RxBuf[5] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[5]-65536);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[5]);
			}
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILHUMI%d\",\"value\":%d},",id,RxBuf[6]);
		break;
		case DEV_WT102:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DO%d_0\",\"switcher\":\"%d\"},",id,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DO%d_1\",\"switcher\":\"%d\"},",id,RxBuf[1]);
		break;
		case DEV_WT108:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILPH%d\",\"value\":%d},",id,RxBuf[0]);
		break;
		case DEV_WT109:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILEC%d\",\"value\":%d},",id,RxBuf[0]);
		break;
		case DEV_WT104:
		case DEV_WT103:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"TEMP%d\",\"value\":%d},",id,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"HUMI%d\",\"value\":%d},",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"BAT%d\",\"value\":%d},",id,RxBuf[2]);
		break;
		case DEV_WT105:
		case DEV_WT106:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"TEMP%d\",\"value\":",id);
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d},",0 - (RxBuf[0]&0x7FFF));
			}
			else
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]);
			}
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"BAT%d\",\"value\":%d},",id,RxBuf[1]);
		break;
		case DEV_WT110:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILTEMP%d\",\"value\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]-65536);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]);
			}
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILHUMI%d\",\"value\":%d},",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILPH%d\",\"value\":%d},",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILEC%d\",\"value\":%d},",id,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILN%d\",\"value\":%d},",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILP%d\",\"value\":%d},",id,RxBuf[5]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"SOILK%d\",\"value\":%d},",id,RxBuf[6]);
		break;
		case DEV_WT101:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DI%d_0\",\"switcher\":\"%d\"},",id,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DI%d_1\",\"switcher\":\"%d\"},",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DI%d_2\",\"switcher\":\"%d\"},",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DI%d_3\",\"switcher\":\"%d\"},",id,RxBuf[3]);
		break;
		case DEV_WT111:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"TEMP%d\",\"value\":",id);			
			if(RxBuf[0] & 0x8000)
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]-65536);
			}
			else 
			{
				len += sprintf((char*)&TxBuf[len],"%d},",RxBuf[0]);
			}
			
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"HUMI%d\",\"value\":%d},",id,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"ILLU%d\",\"value\":%d},",id,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"WINDSPEED%d\",\"value\":%d},",id,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"WINDDIRECTIONVALUE%d\",\"value\":%d},",id,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"WINDDIRECTION%d\",\"value\":%d},",id,RxBuf[5]);	
		break;
		default:
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,0,RxBuf[0]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,1,RxBuf[1]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,2,RxBuf[2]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,3,RxBuf[3]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,4,RxBuf[4]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,5,RxBuf[5]);
			len += sprintf((char*)&TxBuf[len],"{\"flag\":\"DEV%d-%d\",\"value\":%d},",id,6,RxBuf[6]);
			break;
	}
	return len;
}
/*
	报警上报标识
*/
u8 MqttRepAlarmFlag(u8 mode,u8 *InBuf)
{
	u8 len = 0;
	if(mode)
	{
		len += sprintf((char*)InBuf,",\"state\":\"alarm\"");
	}
	else
	{
		len += sprintf((char*)InBuf,",\"state\":\"recovery\"");
	}
	return len;
}
/*
	设置MQTT响应Buf
*/
u16 MqttSetRepBuf(u8 *InBuf,u8 *Index)
{
	u8 isLast = 0,alarmFlag = 0,nextPtr = *Index;
	u16 len = 0;
	
	isLast = GetDevOnlineIndex(Index,&nextPtr);
	if(*Index == 0 && isLast == 1 && nextPtr == 0)
	{
		if(SysInfo.AliyunSet > 0)
		{
			InBuf[len] = '}';
		}
		else
		{
			InBuf[len] = ']';
		}
		return 1;
	}
	for(u8 i = 0; i < 4; i++)
	{
		if(SysInfo.AliyunSet > 0)
		{
			len += OtherSetRegValToBuf(&InBuf[len],DevInfo[*Index].deviceId,DevInfo[*Index].Type,&DevRegVal[(*Index) * 7]);
		}
		else
		{
			len += SetRegValToBuf(&InBuf[len],DevInfo[*Index].deviceId,DevInfo[*Index].Type,&DevRegVal[(*Index) * 7]);
			if(i == 0 && DevAlarm[*Index] != 0)
			{//当前第一个报警
				alarmFlag = DevAlarm[*Index];
				*Index = nextPtr;
				if(isLast)
				{
					*Index = 0;
				}
				break;
			}
		}
		*Index = nextPtr;
		if(isLast)
		{
			*Index = 0;
			break;
		}
		if(DevAlarm[*Index] == 1)
		{//下一个是报警
			break;
		}
		isLast = GetDevOnlineIndex(Index,&nextPtr);
	}
	if(SysInfo.AliyunSet > 0)
	{
		if(len > 0)
		{
			InBuf[len - 1] = '}';
		}
		else
		{
			InBuf[len] = '}';
		}	
	}
	else
	{
		if(len > 0)
		{
			InBuf[len - 1] = ']';
		}
		else
		{
			InBuf[len] = ']';
		}
		if(alarmFlag == 1)
		{
			len += MqttRepAlarmFlag(1,&InBuf[len]);
		}
		else if(alarmFlag == 2)
		{
			len += MqttRepAlarmFlag(0,&InBuf[len]);
		}
	}
	return len;
}

