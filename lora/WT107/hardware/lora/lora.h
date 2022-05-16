#ifndef _LORA_H_
#define _LORA_H_

#include "def.h"

#define LORA_BUF_MAX	100

typedef struct
{
  u8 len;			//�ֽڳ���
  u8 src;			//Դ��ַ
  u8 srcType;                   //���豸���� WT107
  u8 tar;			//Ŀ���ַ
  u8 PackSeq;	                //�������
  u8 RepCycle;                  //�ϱ�����
  u8 RepCntH;                   //�ϱ���ǰ����ֵ
  u8 RepCntL;
  u8 WorkFreq;
  
}LORA_PACKHEAD;

//���ջ����� buf
extern u8 loraRecBuf[LORA_BUF_MAX];
//���ͻ�����buf
extern u8 loraSendBuf[sizeof(LORA_PACKHEAD) + LORA_BUF_MAX + 1];


void InitLora(void);

u8 LoraDatPro(void);

void SendLoraDatPack(u8 *buf,u8 len);

u8 SX127X_SetFreq(u8 ch);

#endif
