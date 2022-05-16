#include "loraDealDat.h"
#include "lora.h"
#include "SysConfig.h"
#include "hardware.h"
#include "stdio.h"
#include "ddl.h"

#define SET_CMD		0xFF
#define HEART_CMD 0xFD

#define ABS_REPCNT(m,n)	(m >= n? m - n:100)

u8 needRestart = 0;

u8 SendBuf[256];
/*
	LoRa传感器数据发送
	tar：目标  终端设备id
	seq：命令
	buf: 发送缓存区
	len；数据长度
        mode 1 可配置 0 仅上报
*/
void SendLoraPack(u8 seq,u8 *buf,u8 len,u8 mode)
{
  u8 i;
  u8 sum = 0;
  u16 tempRepCnt = RepCnt;
  LORA_PACKHEAD *loraPackHead = (LORA_PACKHEAD *)&loraSendBuf;
  if(len > LORA_BUF_MAX)
  {
    return;
  }
  
  loraPackHead->len = sizeof(LORA_PACKHEAD) + len + 1;
  loraPackHead->src = DevInfo.DevId;		//源地址 本机
  loraPackHead->srcType = 1;
  loraPackHead->tar = 0;	                //目标地址
  loraPackHead->PackSeq = seq;
  loraPackHead->RepCycle = DevInfo.RepCycle;
  loraPackHead->RepCntH = tempRepCnt >> 8;
  loraPackHead->RepCntL = tempRepCnt & 0xFF;
  loraPackHead->WorkFreq = DevInfo.workFreq;
  
  if(mode == 1)
  {
    loraPackHead->PackSeq = 0xFB;
  }
  else if(mode == 2)
  {
    loraPackHead->PackSeq = 0xFF;
  }
  
  for(i = 0; i < len; i++)
  {
    loraSendBuf[sizeof(LORA_PACKHEAD) + i] = buf[i];
  }
  for(i = 0; i < sizeof(LORA_PACKHEAD) + len; i++)
  {
    sum ^= loraSendBuf[i];
  }
  loraSendBuf[i] = sum;
  SendLoraDatPack(loraSendBuf,sizeof(LORA_PACKHEAD) + len + 1);
}

/*
  校验是否是本机芯片ID
*/
u8 IsChipId(u8 *InBuf)
{
  u8 rightFlag = 1;
  for(u8 i = 0; i < 6; i++)
  {  
    if(InBuf[i] != DevInfo.Uuid[i] )
    {
      rightFlag = 0;
      break;
    }
  }
  return rightFlag;
}


/*
	设置计数值
	与主机校准计数值
*/
void SetRepCnt(u8 repCntH,u8 repCntL)
{
	u16 tempCnt = repCntH;
	tempCnt = tempCnt << 8 | repCntL;
	//传输也消耗时间
	if(ABS_REPCNT(tempCnt + 15,RepCnt) > 20)
	{
		RepCnt =  tempCnt + 15;
	}
	
}
/*
	设置工作频段
*/
void SetWorkFreq(void)
{
	if(DevInfo.workFreq != tempWorkFreq)
	{
		//数据交换
		tempWorkFreq = DevInfo.workFreq ^ tempWorkFreq;
		DevInfo.workFreq = DevInfo.workFreq ^ tempWorkFreq;
		tempWorkFreq = DevInfo.workFreq ^ tempWorkFreq;
	}
}

/*
	Lora接收数据发送响应
*/
u8 LoraRepDat(void)
{
	static u8 recFlag = 0;
	static u8 firstFlag = 0;
  LORA_PACKHEAD *loraPackHead = (LORA_PACKHEAD *)&loraRecBuf;
  u8 recLen,i; 
  for(i = 0; i < 100; i++)
  {
    recLen = LoraDatPro(); //检测数据是否到达
    if(recLen > 0)
    {
      if(loraPackHead->PackSeq == HEART_CMD)
      {
        if(IsChipId(&loraRecBuf[sizeof(LORA_PACKHEAD) + 6]))
        {
					if(firstFlag == 0)
					{
						firstFlag = 1;
					}
					else
					{
						SetRepCnt(loraPackHead->RepCntH,loraPackHead->RepCntL);
						if(loraPackHead->WorkFreq != DevInfo.workFreq)
						{
							DevInfo.workFreq = loraPackHead->WorkFreq;
							SaveDevInfo(&DevInfo);
							SX127X_SetFreq(DevInfo.workFreq);
						}
						if(loraPackHead->RepCycle != DevInfo.RepCycle)
						{
							DevInfo.RepCycle = loraPackHead->RepCycle;
							if(DevInfo.RepCycle > 1)
							{
								repTim = 40;
							}
							else
							{
								repTim = 20;
							}
							SaveDevInfo(&DevInfo);
						}  
					}	 
					printf("rec beat heat %d\r\n",i);
					break;
        }  

      }
    }
    delay1ms(30);
  }  

  if(recFlag >= 4)
  {
		needRestart = 1;
  }
  if(i >= 100)
  {//3s 未响应 则发送失败
    recFlag++;
		return 0; 
  }
	needRestart = 0;
	recFlag = 0;
	return 1;
}

/*
  Lora发送数据
  mode 1 可配置 0 仅上传 不可配置
	return 0 失败 1成功
*/
u8 LoraSendDat(u8 mode)
{
  u8 ptr = 0;
  SendBuf[ptr++] = DevInfo.TarId[0];
  SendBuf[ptr++] = DevInfo.TarId[1];
  SendBuf[ptr++] = DevInfo.TarId[2];
  SendBuf[ptr++] = DevInfo.TarId[3];
  SendBuf[ptr++] = DevInfo.TarId[4];
  SendBuf[ptr++] = DevInfo.TarId[5];
  
  SendBuf[ptr++] = DevInfo.Uuid[0];
  SendBuf[ptr++] = DevInfo.Uuid[1];
  SendBuf[ptr++] = DevInfo.Uuid[2];
  SendBuf[ptr++] = DevInfo.Uuid[3];
  SendBuf[ptr++] = DevInfo.Uuid[4];
  SendBuf[ptr++] = DevInfo.Uuid[5];
  //让主机时别剩下配置时间
  SendBuf[ptr++] = SysMin;
  
  SendBuf[ptr++] = Airtemp >> 8;
  SendBuf[ptr++] = Airtemp & 0xFF;
  SendBuf[ptr++] = Airhumi >> 8;
  SendBuf[ptr++] = Airhumi & 0xFF;
  SendBuf[ptr++] = ill >> 8;
  SendBuf[ptr++] = ill & 0xFF;
	
  SendBuf[ptr++] = co2 >> 8;
  SendBuf[ptr++] = co2 & 0xFF; 
  SendBuf[ptr++] = Tvoc >> 8;
  SendBuf[ptr++] = Tvoc & 0xFF;
  SendBuf[ptr++] = soilTemp >> 8;
  SendBuf[ptr++] = soilTemp & 0xFF;
  SendBuf[ptr++] = soilHumi >> 8;
  SendBuf[ptr++] = soilHumi & 0xFF;
  SendBuf[ptr++] = 0;
  SendBuf[ptr++] = 0;
  
//	u8 freq = DevInfo.workFreq;
	if(mode > 10)
	{
	//	freq = 1;
		mode = 1;
		SX127X_SetFreq(1);
		delay1ms(110);  
	}
	/*
	if(SX127X_SetFreq(freq))
  {
    delay1ms(110);     
    SendLoraPack(11,SendBuf,ptr,mode);
    //延迟必要  会影响误码率
    delay1ms(340);
  }
  SX127X_SetFreq(0);
  */
	    SendLoraPack(11,SendBuf,ptr,mode);
    //延迟必要  会影响误码率
    delay1ms(290);
	if(mode == 2)
	{
		return 1;
	}
	return LoraRepDat();
}

/*
  Lora接收数据

  mode 1可以进行配置 设置计数  0 不能进行设置
*/
void LoraRecDat(void)
{
  LORA_PACKHEAD *loraPackHead = (LORA_PACKHEAD *)&loraRecBuf;
  u8 recLen = LoraDatPro();			//检测数据是否到达
  u8 i,sum = 0;
	
  if(recLen > 0)
  {//有数据接收到
    for(i = 0; i < recLen; i++)
    {//校验
      sum ^= loraRecBuf[i];
    }

		
    if(!IsChipId(&loraRecBuf[sizeof(LORA_PACKHEAD) + 6]))
    {
      return;
    }
    if(sum == 0 && loraPackHead->PackSeq == SET_CMD)
    {//本机地址
      for(i = 0; i < 6; i++)
      {
        DevInfo.TarId[i] = loraRecBuf[sizeof(LORA_PACKHEAD) + i] & 0xFF;
      }
      DevInfo.DevId = loraPackHead->tar;
      for(i = 0; i < 7; i++)
      {
        DevInfo.HeightLimit[i] = loraRecBuf[sizeof(LORA_PACKHEAD) + 12 + 4*i] << 8 | loraRecBuf[sizeof(LORA_PACKHEAD) + 13 + 4*i];
				DevInfo.LowLimit[i] = loraRecBuf[sizeof(LORA_PACKHEAD) + 14 + 4*i] << 8 | loraRecBuf[sizeof(LORA_PACKHEAD) + 15 + 4*i];
      }
			
      DevInfo.workFreq = loraPackHead->WorkFreq;
      DevInfo.RepCycle = loraPackHead->RepCycle;
			if(DevInfo.RepCycle > 1)
			{
				repTim = 40;
			}
			else
			{
				repTim = 20;
			}
      SetRepCnt(loraPackHead->RepCntH,loraPackHead->RepCntL);
      SaveDevInfo(&DevInfo);
      DebugDevInfo();
			SetWorkFreq();
			//设置成功响应
      LoraSendDat(2);
      
			if(tempWorkFreq != DevInfo.workFreq)
			{
				DevInfo.workFreq = tempWorkFreq;
				delay1ms(300);
				SX127X_SetFreq(DevInfo.workFreq);
			}
			//设置成功 LED闪烁2s
			LEDShow();
			CLEDON();
    }
  }
}
