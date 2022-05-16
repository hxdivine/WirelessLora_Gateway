
#ifndef _HARDWARE_H_
#define _HARHWARE_H_

#include "def.h"

#define PWRLEDON						gpio_bit_set(GPIOB,GPIO_PIN_13);
#define PWRLEDOFF						gpio_bit_reset(GPIOB,GPIO_PIN_13);

#define GPRSLEDON						gpio_bit_set(GPIOA,GPIO_PIN_8);
#define GPRSLEDOFF					gpio_bit_reset(GPIOA,GPIO_PIN_8);

#define NETLEDON						gpio_bit_set(GPIOB,GPIO_PIN_14);
#define NETLEDOFF						gpio_bit_reset(GPIOB,GPIO_PIN_14);
#define NETLED_ENABLE				(NetLedCnt = LED_STATUS)

#define RFLEDON							gpio_bit_set(GPIOB,GPIO_PIN_15);
#define RFLEDOFF						gpio_bit_reset(GPIOB,GPIO_PIN_15);
#define RFLED_ENABLE				(LoraLedCnt = LED_STATUS)
	
#define GSMPWRON						gpio_bit_set(GPIOA,GPIO_PIN_11);
#define GSMPWROFF						gpio_bit_reset(GPIOA,GPIO_PIN_11);

#define KEYRST							((gpio_input_port_get(GPIOB) & GPIO_PIN_8))

#define DET5V								((gpio_input_port_get(GPIOA) & GPIO_PIN_12) == 0)

#define ISNUM(dat)					(dat >= 0x30 && dat <= 0x39)				
#define ISVALIDCHAR(dat)		(dat >= 0x20 && dat <= 0xFF)
#define ISASCCHAR(dat)			(dat >= 0x20 && dat <= 0x7F)
#define ABSVALUE(a,b)				(a > b? a - b:b - a)	//获取绝对值

#define LED_STATUS					100 //2s

extern u8 version[];

extern u8 NetLedCnt;
extern u8 LoraLedCnt;
extern u16 GprsLedCnt;
extern u8 GprsLedStart;
extern u8 GprsLedEnd;
extern u8 UpdateTime;
extern u32 SysTimCnt;

extern u8 ChipId[4];

extern u16 RepCnt;

extern u8 DB_Flag;

void InitNVIC(void);

void InitGPIO(void);

void PutInUart0(u8 dat);

void PutInUart1(u8 dat);

void SendString(u8 *buf);

u8 OpenGSM(void);

void CloseGSM(void);

void RestartGSM(void);

u8 CompStr(u8 *str1,u8 *str2,u8 len);

u8 CopyStr(u8 *str1,u8 *str2,u8 DataLen);

u8 NumToStr(u8 *str1,u16 num);

u16 StrToNum(u8 *str);

u8 IsIPaVailable(u8 *TarIp);

u8 GetDevOnlineIndex(u8 *tempPtr,u8 *nextPtr);

void GPRSLEDShow(void);

void SetUtcTime(u8 *str);

u32 GetSysTimCnt(void);

u16 GetSecCnt(void);

void TotalInit(void);

void DebugSysInfo(void);

void GetTerminalW2Info(u8 Index,u8 mode,u8 isLast);

void CheckRefactory(void);

void CheckPower(void);

void ChekPwd(void);

void JudgeDevSendTim(void);

void S282_LogInt(u8 *str1,u8 len);

void S282_Log(u8 *str);

#endif
