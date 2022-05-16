#ifndef _LORA_H_
#define _LORA_H_

#include "def.h"

#define LORA_BUF_MAX	100

typedef struct
{
  u8 len;			//字节长度
  u8 src;			//源地址
  u8 srcType;                   //本设备类型 WT107
  u8 tar;			//目标地址
  u8 PackSeq;	                //命令序号
  u8 RepCycle;                  //上报周期
  u8 RepCntH;                   //上报当前计数值
  u8 RepCntL;
  u8 WorkFreq;
  
}LORA_PACKHEAD;

//接收缓存区 buf
extern u8 loraRecBuf[LORA_BUF_MAX];
//发送缓存区buf
extern u8 loraSendBuf[sizeof(LORA_PACKHEAD) + LORA_BUF_MAX + 1];


void InitLora(void);

u8 LoraDatPro(void);

void SendLoraDatPack(u8 *buf,u8 len);

u8 SX127X_SetFreq(u8 ch);

#endif
