#include "lora.h"
#include "sx127x_Drive.h"
#include "stdio.h"
#include "gpio.h"

u8 loraRecBuf[LORA_BUF_MAX];
//���ͻ�����buf
u8 loraSendBuf[sizeof(LORA_PACKHEAD) + LORA_BUF_MAX + 1];

/*
  ��ʼ��Lora������
*/
void InitLora(void)
{
	stc_gpio_cfg_t gpioStruct;
	DDL_ZERO_STRUCT(gpioStruct);
	gpioStruct.enDir = GpioDirOut;
	gpioStruct.enDrv = GpioDrvH;

  printf("InitLora\r\n");
  G_LoRaConfig.BandWidth = BW125KHZ;    //BW = 125KHz  BW125KHZ
  G_LoRaConfig.SpreadingFactor = SF09;  //SF = 9
  G_LoRaConfig.CodingRate = CR_4_6;     //CR = 4/6
  G_LoRaConfig.PowerCfig = 15;          //19?��1dBm
  G_LoRaConfig.MaxPowerOn = TRUE;       //��?�䨮1|?��?a??
  G_LoRaConfig.CRCON = TRUE;            //CRCD��?��?a??
  G_LoRaConfig.ExplicitHeaderOn = TRUE; //Header?a??
  
	Gpio_Init(GpioPortA,GpioPin4,&gpioStruct);
	Gpio_Init(GpioPortB,GpioPin1,&gpioStruct);
	
	Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);
//	Gpio_SetAfMode(GpioPortB, GpioPin1,GpioAf1);
  if(SX127X_Lora_init() != NORMAL)
  {//����ģ���ʼ��
    SX127X_StartRx();
  }
}

/*
  ����lora���ݰ�
*/
void SendLoraDatPack(u8 *buf,u8 len)
{
  G_LoRaConfig.PayloadLength = len;
  SX127X_TxPacket(buf);
}
/*
  LoRaѭ������Ƿ������ݵ���
*/
u8 LoraDatPro(void)
{
  u8 recFlag = 0;
  //�������ݳ���
  u8 recLen = 0;
  
  SX127X_Read(REG_LR_IRQFLAGS,&recFlag);

  //�����ݵ���
  if(recFlag)
  {
    //����жϱ�־
    SX127X_Write(REG_LR_IRQFLAGS,0xFF);
    
    if(recFlag & RFLR_IRQFLAGS_TXDONE)
    {//�������ת����
      SX127X_StartRx();
    }
    
    if(recFlag & RFLR_IRQFLAGS_RXDONE)
    {
      //������G_LoRaConfig.PayloadLength
      SX127X_RxPacket(loraRecBuf);
      recLen = G_LoRaConfig.PayloadLength;
      //��ȡ��ǰ�ŵ��е�RSSIֵ
      s16 rssi = SX127X_Current_RSSI();
      printf("LoRaRecLen:%d,SNR:%d,RSSIC:%d,%d\r\n",recLen,G_LoRaPara.Packet_SNR,(s16)G_LoRaPara.Packet_RSSI,rssi);      
    }
    
    SX127X_Read(REG_LR_IRQFLAGS, &recFlag);
    if(recFlag)
    { 
      InitLora();
    }
  }
  
  return recLen;
}
