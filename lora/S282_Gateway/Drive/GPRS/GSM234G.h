
#ifndef _GSM234G_H_
#define _GSM234G_H_

#include "def.h"

//话机相关//flag
#define RECOK						1					//1  
#define RECCHAR					(1<<1)		//0x02
#define REGOK						(1<<2)		//0x04
#define TIMEVALID				(1<<3)		//0x08
#define CLIPCOME				(1<<4)		//0x10
#define CALLOK					(1<<5)		//0x20
#define CGATT						(1<<6)		//0x40
#define NOSIM						(1<<7)		//0x80
#define WAITPIN					(1<<8)		//0x100
#define PINOK						(1<<9)		//0x200
#define READYID					(1<<10)		//0x400
#define RECID						(1<<11)		//0x800
#define RECERR					(1<<12)		//0x1000
#define POWERDOWN				(1<<13)		//0x2000
#define RINGCOME				(1<<14)		//0x4000
#define ACTIVCALL				(1<<15)		//0x8000
#define REMOTEEND				(1<<16)		//0x10000
#define INITOK					(1<<17)		//0x20000
#define PINERR					(1<<18)		//0x40000
#define NOCARRIER				(1<<19)		//0x80000
#define READYSIMID			(1<<20)		//0x100000
#define SIMID						(1<<21)		//0x200000
//GPRS相关
#define  GPRSCONNFAIL 	(1<<22)		//0x400000
#define  GPRSIPDLEN 		(1<<23)		//0x800000
#define	 GPRSIPDDATA		(1<<24) 	//0x1000000
#define  GPRSPACKOK			(1<<25)		//0x2000000
//连接
#define  GPRSCONNECTED	(1<<26)		//0x4000000
#define  GPRSNEEDRECONN	(1<<27)		//0x8000000需要重连
//tcp udp相关
#define  GPRSSENDOK			(1<<28)		//0x10000000
#define  GPRSSENDFAIL		(1<<29)		//0x20000000
#define  TCPCONNECTED		(1<<30)		//0x40000000


//短信Flag
#define READYRECSMSPDU	(1)
#define RECSMSOK				(1 << 1)
#define RECSMSCAP				(1 << 2)
#define RECSMSDEL				(1 << 3)
#define NEWSMSCOME			(1 << 4)
#define SMSSENDOK				(1 << 5)
#define MMSSENGOK				(1 << 6)

//IP数据长度
#define GPRSDATALEN			300

#define GSM_LEN					160

#define SMS_RECLEN			220

typedef struct{
	
	u8 recbuf[SMS_RECLEN];
	u8 sendbuf[GSM_LEN];
	u16 recLen;
	
}SMS_Data_Frame;

typedef struct{

	u8 recBuf[GPRSDATALEN];
	u8 recLen;
}GPRS_DATA_Frame;

extern GPRS_DATA_Frame gprs_data_Frame;

extern SMS_Data_Frame sms_data_Frame;

extern u8 g_SmsTelNo[11];

extern u8 g_SmsCnt;
extern u8 g_SmsTempCnt;
extern u8 NeedSetAPN;
extern u8 NeedOffline;
extern u8 SMSFlag;
u8 GSMInit(void);

void WriteComm(u8 *dat);

u8 SendCmd(u8 *cmd,u32 flag,u8 cnt,u16 ms);

u8 CheckAt(void);

u8 GSMSetGprs(void);

void GSMGetCsq(void);

void GSMGetReg(void);

void GSMGetMsgCap(void);

void PutModuleVer(void);

u8 GSMRecOK(u16 ms);

void GprsDisConnect(void);

void GprsDisConRemote(void);

u8 GprsConnectServer(u8 *TarIP,u16 port);

void GprsAnalysisCmd2G(void);

void GprsAnalysisCmd3GOr4G(void);

void SimAnalysisCmd(void);

void UartToGSM(u8 dat);

u8 SendGprsDat(u8 *InBuf,u16 actionLen);

void GSMReplyFunc(void);

u8 WaitGsmChar(u8 dat,u16 ms);

void SendHexAsAsc(u8 dat);

#endif
