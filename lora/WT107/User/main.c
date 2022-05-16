
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
	ÿ���ȡCo2������������
*/
u8 GetCo2SenorBySec(void)
{	
	if(ABSVALE(GetSysSec(),tempSec) >= 1)
	{//Co2��Ҫÿ���ȡһ��
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
	{//������������ ��������
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
			{//�豸ǰһ��ID�����е���
			 
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
	{//һ̨�豸���2s����һ������
		if(SendCnt == 0)
		{ 
			SendCnt = 1;
			//��ȡ������ʱ��1.2s
			ReadyToSend(mode);
		}
	}	
	else if(alarmFlag && CheckAlarmTim(GetSysMin()*60+GetSysSec()))
	{//һ̨�豸���2s����һ������  Ԥ��
		if((RepCnt % repTim) > 20 && (RepCnt % repTim) < (repTim - 12))
		{//���ϱ����ڴ���1ʱ �������ϱ�Ԥ��
			if(ABSVALE(RepCnt,DevInfo.DevId * repTim) > 800)
			{//���ͺ�20s
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
	��������ƥ��ģʽ
	ȷ���������յ���������������

*/
void loraToMatchHost(void)
{
	u8 sec2 = 1;
	u8 SendTim = 0;
	u8 isChangeFreg = 1;
	
	CLEDOFF();
	while(1)
	{
		//ǰ4�η�������Ƶ�� ��4��Ĭ��Ƶ��
		LoraRecDat();
		if(sec2 % 2 == 0)
		{//ÿ20s���з���һ��
			sec2 = 0;
			SysMin = GetSysMin();
			if(LoraSendDat(isChangeFreg))
			{
				break;
			}
			SendTim++;
			if(SendTim > 8)
			{//δƥ�䵽����  
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
	//�ڴ˽������� ͬʱ�豸���������ϱ�����
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
	{//10s�������к�
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
