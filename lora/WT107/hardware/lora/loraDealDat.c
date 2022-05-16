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
	LoRa���������ݷ���
	tar��Ŀ��  �ն��豸id
	seq������
	buf: ���ͻ�����
	len�����ݳ���
        mode 1 ������ 0 ���ϱ�
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
  loraPackHead->src = DevInfo.DevId;		//Դ��ַ ����
  loraPackHead->srcType = 1;
  loraPackHead->tar = 0;	                //Ŀ���ַ
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
  У���Ƿ��Ǳ���оƬID
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
	���ü���ֵ
	������У׼����ֵ
*/
void SetRepCnt(u8 repCntH,u8 repCntL)
{
	u16 tempCnt = repCntH;
	tempCnt = tempCnt << 8 | repCntL;
	//����Ҳ����ʱ��
	if(ABS_REPCNT(tempCnt + 15,RepCnt) > 20)
	{
		RepCnt =  tempCnt + 15;
	}
	
}
/*
	���ù���Ƶ��
*/
void SetWorkFreq(void)
{
	if(DevInfo.workFreq != tempWorkFreq)
	{
		//���ݽ���
		tempWorkFreq = DevInfo.workFreq ^ tempWorkFreq;
		DevInfo.workFreq = DevInfo.workFreq ^ tempWorkFreq;
		tempWorkFreq = DevInfo.workFreq ^ tempWorkFreq;
	}
}

/*
	Lora�������ݷ�����Ӧ
*/
u8 LoraRepDat(void)
{
	static u8 recFlag = 0;
	static u8 firstFlag = 0;
  LORA_PACKHEAD *loraPackHead = (LORA_PACKHEAD *)&loraRecBuf;
  u8 recLen,i; 
  for(i = 0; i < 100; i++)
  {
    recLen = LoraDatPro(); //��������Ƿ񵽴�
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
  {//3s δ��Ӧ ����ʧ��
    recFlag++;
		return 0; 
  }
	needRestart = 0;
	recFlag = 0;
	return 1;
}

/*
  Lora��������
  mode 1 ������ 0 ���ϴ� ��������
	return 0 ʧ�� 1�ɹ�
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
  //������ʱ��ʣ������ʱ��
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
    //�ӳٱ�Ҫ  ��Ӱ��������
    delay1ms(340);
  }
  SX127X_SetFreq(0);
  */
	    SendLoraPack(11,SendBuf,ptr,mode);
    //�ӳٱ�Ҫ  ��Ӱ��������
    delay1ms(290);
	if(mode == 2)
	{
		return 1;
	}
	return LoraRepDat();
}

/*
  Lora��������

  mode 1���Խ������� ���ü���  0 ���ܽ�������
*/
void LoraRecDat(void)
{
  LORA_PACKHEAD *loraPackHead = (LORA_PACKHEAD *)&loraRecBuf;
  u8 recLen = LoraDatPro();			//��������Ƿ񵽴�
  u8 i,sum = 0;
	
  if(recLen > 0)
  {//�����ݽ��յ�
    for(i = 0; i < recLen; i++)
    {//У��
      sum ^= loraRecBuf[i];
    }

		
    if(!IsChipId(&loraRecBuf[sizeof(LORA_PACKHEAD) + 6]))
    {
      return;
    }
    if(sum == 0 && loraPackHead->PackSeq == SET_CMD)
    {//������ַ
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
			//���óɹ���Ӧ
      LoraSendDat(2);
      
			if(tempWorkFreq != DevInfo.workFreq)
			{
				DevInfo.workFreq = tempWorkFreq;
				delay1ms(300);
				SX127X_SetFreq(DevInfo.workFreq);
			}
			//���óɹ� LED��˸2s
			LEDShow();
			CLEDON();
    }
  }
}
