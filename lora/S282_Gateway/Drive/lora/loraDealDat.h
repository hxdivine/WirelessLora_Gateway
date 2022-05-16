
#ifndef _LORA_DEAL_DAT_H_
#define _LORA_DEAL_DAT_H_

#include "def.h"
#include "config.h"

typedef struct
{
	u8 len;				//字节长度
	u8 src;				//源地址
	u8 srcType; 	//源地址类型 0WT107 1WT108 2WT109 WT102 
	u8 tar;				//目标地址
	u8 PackSeq;		//命令序号
	u8 RepCycle;	//上报周期
	u8 RepCntH;		//当前计数值
	u8 RepCntL;
	u8 workFreq;	//主机工作频段
}LORA_PACKHEAD;

//下发到设备的结构体
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
	
	u8 baud;					//波特率  1 2 3 4 5 6
	u8 dataLen;				//数据位个数
	u8 verifyType;		//校验位 0:无 1：寄 2：偶 
	u8 stopLen;				//停止位个数0:1, 1:1.5, 2:2 
	
	u16 onlineTim;		//剩余配置时间
	u8 avaliable;			//可配置标志
	u8 isNode;				//是否为配置过的节点
}TERMINNAL_INFO;

#define LORA_BUF_MAX	200
//接收缓存区 buf
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
