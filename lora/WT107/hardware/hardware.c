
#include "hardware.h"
#include "HC_Usart.h"
#include "HC_Spi.h"
#include "HC_Flash.h"
#include "HC_Tim.h"
#include "HC_Time3.h"
#include "HC_I2c.h"
#include "SysConfig.h"
#include "gpio.h"
#include "stdio.h"
#include "BH1750.h"
#include "Sgp30.h"
#include "Soil.h"
#include "Gxht3l.h"
#include "HC_Wdt.h"
#include "stdlib.h"

u16 ill = 0;
u16 Airtemp = 0;
u16 Airhumi = 0;
u16 soilHumi = 0;

u16 co2 = 400;	//默认400
u16 Tvoc = 0;
u16 soilTemp = 0;


u8 ChipId[10];
const u8 HEX[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

u16 RepCnt = 0;

u8 tempWorkFreq;
//设置上报时间 40为每2s上报一次 20为1s上报一次
u8 repTim = 40;
#define CHANGE_CHIP(m,n)	(((m >> 4) + n) | ((m & 0x0f) + n))
/*
  获取芯片ID
*/
void GetChipId(void)
{
  u8 *ptr;
	u8 inc;
  ptr = (u8*)0X00100E7A;
  ChipId[0] = *ptr++;
  ChipId[1] = *ptr++;
  ChipId[2] = *ptr++;
  ChipId[3] = *ptr;
	
	inc = HEX[ChipId[2] & 0x0f];
	
	ChipId[0] = (u8)CHANGE_CHIP(ChipId[0],inc);
	ChipId[1] = (u8)CHANGE_CHIP(ChipId[1],inc);
	ChipId[2] = (u8)CHANGE_CHIP(ChipId[2],inc);
	ChipId[3] = (u8)CHANGE_CHIP(ChipId[3],inc);
	
	inc = ChipId[2];
	ChipId[2] = ChipId[3];
	ChipId[3] = inc;
	for(u8 i = 0; i < 4;i++)
	{
		printf("%02X ",ChipId[i]);
	}

  printf("\r\n");
}
/*
	使用引脚初始化
*/
void GPIOInit(void)
{
	stc_gpio_cfg_t stcGpioCfg;
	
	//输出
	stcGpioCfg.enDir = GpioDirOut;
	stcGpioCfg.enPu = GpioPuDisable;
  stcGpioCfg.enPd = GpioPdEnable;
	
	Gpio_Init(GpioPortB,GpioPin2,&stcGpioCfg);
	Gpio_Init(GpioPortB,GpioPin1,&stcGpioCfg);
	
	stcGpioCfg.enDir = GpioDirIn;
	stcGpioCfg.enPu = GpioPuEnable;
	Gpio_Init(GpioPortB,GpioPin0,&stcGpioCfg);
}
/*
	时钟配置
*/
void ClkInit(void)
{
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,TRUE);
	Sysctrl_SysClkSwitch(SysctrlClkRCL);
	Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);       //配置外部24Mhz时钟
	Sysctrl_SysClkSwitch(SysctrlClkRCH);
	Sysctrl_ClkSourceEnable(SysctrlClkRCL,FALSE);
	Sysctrl_ClkSourceEnable(SysctrlClkXTL, TRUE);   
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
	/*使能dmac外设时钟门控开关*/
	Sysctrl_SetPeripheralGate(SysctrlPeripheralDma,TRUE);
	
	Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);
	//RTC模块时钟使能
	Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);
	Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
	//定时器外设时钟使能
	Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3,TRUE);
}

void CLEDON(void)
{
	Gpio_SetIO(GpioPortB, GpioPin2);
}

void CLEDOFF(void)
{
	Gpio_ClrIO(GpioPortB, GpioPin2);
}

/*
	所有初始化
*/
void TotalInit(void)
{
	ClkInit();
	GPIOInit();
	CLEDON();
	Usart0Init();
	Usart1Init();
	
	SpiInit();
	IIC_Init();
	WdtInit();
	FlashInit();
	GetChipId();
	ReadDevInfo(&DevInfo);
  if(DevInfo.IsWrite != 1)
  {
    SetDevInfoDef();
  }
	if(DevInfo.RepCycle == 1)
	{
		repTim = 20;
	}
	TimInit();
	//计数不准确
	Time3Init(18730);
	delay1ms(50);
	BH175_Init();
	Gxht_Restart();
	delay1ms(10);
	SGP30_Init();
	
	tempWorkFreq = DevInfo.workFreq;
}

void DebugDevInfo(void)
{
	printf("\r\n/************************************/\r\n");
	printf("/*   WWW.4G-IOT.COM copyright 2021  */\r\n");
	printf("/*       All rights Reserved        */\r\n");
	printf("/*    king Pigeon Hi-Tech.Co.,ltd.  */\r\n");
	printf("/*              DevID:%02d            */\r\n",DevInfo.DevId); 
	printf("/*     TarID:");
  for(u8 i = 0; i < 6; i++)
  {
     printf("%02X ",DevInfo.TarId[i]);
  }
	printf("     */\r\n");
	printf("/*      S/N:");
  for(u8 i = 0; i < 6; i++)
  {
     printf("%02X ",DevInfo.Uuid[i]);
  }
	printf("      */\r\n"); 
	printf("/*temperature-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[0],DevInfo.LowLimit[0]);
	printf("/*   humidity-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[1],DevInfo.LowLimit[1]);
	printf("/*Illuminance-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[2],DevInfo.LowLimit[2]);
	printf("/*        CO2-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[3],DevInfo.LowLimit[3]);
	printf("/*       TVOC-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[4],DevInfo.LowLimit[4]);
	printf("/*   soilTemp-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[5],DevInfo.LowLimit[5]);
	printf("/*   soilHumi-limit:%05d H %05d L */\r\n",DevInfo.HeightLimit[6],DevInfo.LowLimit[6]);
	printf("/*             RepCycle:%02d          */\r\n",DevInfo.RepCycle);
  printf("/*             workFreq:%02d          */\r\n",DevInfo.workFreq);
	printf("/*    latest build @ %s    */\r\n",__DATE__);
	printf("/************************************/\r\n");
}

#define CheckNum(num,max,min)	(num > max ? 1 : (num < min ? 1 : 0))
/*
	获取Co2数据
	需要每隔1s读一次
*/
void GetGetCo2Data(void)
{
	SGP30_GetCo2Data(&co2,&Tvoc);
	delay1ms(1);
	Gxht_GetTempAndHumi(&Airtemp,&Airhumi);
	delay1ms(50);
	BH175_GetIllData(&ill);
}
/*
	是否预警
*/
u8 IsDataAlarm(void)
{
	if(CheckNum(Airtemp,DevInfo.HeightLimit[0],DevInfo.LowLimit[0]))
	{
		return 1;
	}
	else if(CheckNum(Airhumi,DevInfo.HeightLimit[1],DevInfo.LowLimit[1]))
	{
		return 1;
	}
	else if(CheckNum(ill,DevInfo.HeightLimit[2],DevInfo.LowLimit[2]))
	{
		return 1;
	}
	else if(CheckNum(co2,DevInfo.HeightLimit[3],DevInfo.LowLimit[3]))
	{
		return 1;
	}
	else if(CheckNum(Tvoc,DevInfo.HeightLimit[4],DevInfo.LowLimit[4]))
	{
		return 1;
	}
	else if(CheckNum(soilTemp,DevInfo.HeightLimit[5],DevInfo.LowLimit[5]))
	{
		return 1;
	}
	else if(CheckNum(soilHumi,DevInfo.HeightLimit[6],DevInfo.LowLimit[6]))
	{
		return 1;
	}
	return 0;
}

/*
	获取传感器数据
*/
void GetSensorData(void)
{
	Soil_GetSoilTempAndHUmi(&soilTemp,&soilHumi);
}

void LEDShow(void)
{
	for(u8 i = 0; i < 6; i++)
	{
		if(i % 2 == 0)
		{
			CLEDON();
		}
		else
		{
			CLEDOFF();
		}
		delay1ms(200);
	}
}
