
#ifndef _LORA_DEAL_DAT_H_
#define _LORA_DEAL_DAT_H_

#include "def.h"
#include "config.h"

typedef struct
{
	u8 len;				//�ֽڳ���
	u8 src;				//Դ��ַ
	u8 srcType; 	//Դ��ַ���� 0WT107 1WT108 2WT109 WT102 
	u8 tar;				//Ŀ���ַ
	u8 PackSeq;		//�������
	u8 RepCycle;	//�ϱ�����
	u8 RepCntH;		//��ǰ����ֵ
	u8 RepCntL;
	u8 workFreq;	//��������Ƶ��
}LORA_PACKHEAD;

//�·����豸�Ľṹ��
typedef struct
{
	u8 TarId;
	u8 TerminalRepCycle;
	u8 Type;
	u8 FreqCh;
	
}DOWNLOADINFO;

typedef struct
{
	u8 Id;
	u8 uId[6];
	u8 type;	
	
	u8 baud;					//������  1 2 3 4 5 6
	u8 dataLen;				//����λ����
	u8 verifyType;		//У��λ 0:�� 1���� 2��ż 
	u8 stopLen;				//ֹͣλ����0:1, 1:1.5, 2:2 
	
	u16 onlineTim;		//ʣ������ʱ��
	u8 avaliable;			//�����ñ�־
	u8 isNode;				//�Ƿ�Ϊ���ù��Ľڵ�
}TERMINNAL_INFO;

#define LORA_BUF_MAX	200
//���ջ����� buf
extern u8 loraRecBuf[LORA_BUF_MAX];

extern TERMINNAL_INFO tempDev[];

extern u8 devPtr;

u8 LoraPro(void);

u8 SendLoraData(u8 tar,u8 seq,u8 *buf,u8 len);

u8 DownLoad2TerminalWT2(u8 id,u8 mode,u8 option);

u8 DownLoad2TerminalWT1(u8 id,u8 *InBuf);

void SetWorkFreq(void);

void ChangeTempDevID(u8 *uId,u8 type,u8 Id);

void NewDevOffline(void);

#endif
