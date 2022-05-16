
#include "hardware.h"
#include "HC_Tim.h"
#include "loraDealDat.h"
#include "SysConfig.h"
#include "HC_Time3.h"
#include "stdio.h"
#include "ddl.h"
#include "HC_Wdt.h"
#include "HC_Usart.h"

#define MAX_DEV 			50+10
#define ABSVALE(m,n)	(m > n?m-n:n-m)
#define ABSDEV(dev)		(dev == 1 ? MAX_DEV - 1:dev - 2)

u8 SysMin = 0;
u8 tempSec;
u8 timCount = 0;
u8 alarmFlag = 0;
u16 alarmSendTim = 0;

u8 PCSetDevUid(void)
{
	u8 rec = 0;
	if(UartLen > 3)
	{
			if(UartLen > 9)
			{
				if(RxBuf[0] == 'M' && RxBuf[1] == 'A' && RxBuf[2] == 'C')
				{
					//MAC:ASDFGH
					for(u8 i = 0; i < 6; i++)
					{
						DevInfo.Uuid[i] = RxBuf[i+4];
					}
					SaveDevInfo(&DevInfo);
					
					printf("MAC=%s",DevInfo.Uuid);
					rec = 1;
					//MAC:ASDFGH
					for(u8 i = 0; i < 6; i++)
					{
							if(DevInfo.Uuid[i] != RxBuf[i+4])
						{
							rec = 0;
							break;
						}
					}
			}
				UartLen = 0;
		}
		else if(RxBuf[0] == 'S' && RxBuf[1] == 'E' && RxBuf[2] == 'T')
		{
			printf("ReadyOk");
			UartLen = 0;
		}
	
	}
	return rec;
}


/*
	每秒读取Co2传感器的数据
*/
u8 GetCo2SenorBySec(void)
{	
	if(ABSVALE(GetSysSec(),tempSec) >= 1)
	{//Co2需要每秒读取一次
		GetGetCo2Data();
		timCount++;
		tempSec = GetSysSec();
		WdtFeedDog();
		return 1;
	}
	return 0;
}


void ReadyToSend(u8 mode)
{
	LoraSendDat(mode);
	if(needRestart == 1)
	{//主机可能下线 重启连接
		NVIC_SystemReset();
	}
	LEDShow();
	if(mode)
	{
		CLEDON();
	}
}
	

u8 CheckAlarmTim(u16 tim)
{
	if(ABSVALE(alarmSendTim,tim) > 60)
	{
		return 1;
	}
	return 0;
}

void LoraPro(u8 mode)
{
	static u8 SendCnt = 0;
	WdtFeedDog();
	if(timCount >= 8)
	{
		GetSensorData();
		if(IsDataAlarm())
		{
			if(alarmFlag)
			{
				alarmFlag = 2;
			}
			else
			{
				alarmFlag = 1;
				alarmSendTim = GetSysMin() *60 + GetSysSec();
			}
		}
		printf("Co2:%d,Tvoc:%d\r\n",co2,Tvoc);
		printf("Ill:%d\r\n",ill);
		printf("temp:%d,Humi:%d\r\n",Airtemp,Airhumi);
		printf("SoilTemp:%d,SoilHumi:%d\r\n",soilTemp,soilHumi);
		timCount = 0;
		if(mode == 0)
		{
			if(RepCnt >= ABSDEV(DevInfo.DevId) * repTim && RepCnt <= ((ABSDEV(DevInfo.DevId)+1) * repTim))
			{//设备前一个ID不进行点亮
			 
			}
			else
			{
				CLEDON();
				delay100us(625);
				CLEDOFF();
			}
		}
	}
	if(RepCnt >= (DevInfo.DevId - 1) * repTim && RepCnt <= ((DevInfo.DevId - 1) * repTim + 12))
	{//一台设备间隔2s发送一次数据
		if(SendCnt == 0)
		{ 
			SendCnt = 1;
			//读取传感器时间1.2s
			ReadyToSend(mode);
		}
	}	
	else if(alarmFlag && CheckAlarmTim(GetSysMin()*60+GetSysSec()))
	{//一台设备间隔2s发送一次数据  预警
		if((RepCnt % repTim) > 20 && (RepCnt % repTim) < (repTim - 12))
		{//当上报周期大于1时 ，可以上报预警
			if(ABSVALE(RepCnt,DevInfo.DevId * repTim) > 800)
			{//发送后20s
				ReadyToSend(mode);
				alarmFlag = 0;				
			}
		}
	}	
	else if(RepCnt >= ABSDEV(DevInfo.DevId) * repTim && RepCnt <= ((ABSDEV(DevInfo.DevId)+1) * repTim))
	{
			SendCnt = 0;
	}
	else
	{
		GetCo2SenorBySec();
	}
}

/*
	开机进入匹配模式
	确保主机接收到本机的配置请求

*/
void loraToMatchHost(void)
{
	u8 sec2 = 1;
	u8 SendTim = 0;
	u8 isChangeFreg = 1;
	
	CLEDOFF();
	while(1)
	{
		//前4次发主机的频段 后4次默认频段
		LoraRecDat();
		if(sec2 % 2 == 0)
		{//每20s进行发送一次
			sec2 = 0;
			SysMin = GetSysMin();
			if(LoraSendDat(isChangeFreg))
			{
				break;
			}
			SendTim++;
			if(SendTim > 8)
			{//未匹配到主机  
				NVIC_SystemReset();
			}	
			if(SendTim > 4)
			{
				isChangeFreg = 11;
			}
		}
		sec2 += GetCo2SenorBySec();
	}
	printf("set oprate\r\n");
	CLEDON();
	//在此进行配置 同时设备按周期性上报数据
	while(SysMin < 5)
	{
		LoraRecDat();
		SysMin = GetSysMin();
		LoraPro(1);
	}
}


void SetDevSerialNum(void)
{
	u8 sec = 0;
	if(DevInfo.Uuid[0] == (ChipId[0] >> 4) && DevInfo.Uuid[5] == (ChipId[2] & 0x0F)&& DevInfo.Uuid[1] == (ChipId[0] & 0x0F))
	{//10s设置序列号
		while(1)
		{
			sec += GetCo2SenorBySec();
			if(PCSetDevUid())
			{
				break;
			}
		}
	}
	CloseUartRx();
}

int main(void)
{
	TotalInit();
	DebugDevInfo();
	tempSec = GetSysSec();
	SetDevSerialNum();
	TimeCntStart();
	WdtFeedDog();
	GetGetCo2Data();
	GetSensorData();
	loraToMatchHost();
	CLEDOFF();
	printf("Configuration Over\r\n");
	timCount = 0;
	while(1)
	{
		LoraPro(0);
	}
}
