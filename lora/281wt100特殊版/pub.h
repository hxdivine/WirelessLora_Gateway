

#define vu8 __IO u8
#define vu16 __IO u16
#define vu32 __IO u32

#define PHONELEN 22

#define ISNUM(dat)    (dat>=0x30&&dat<=0x39)
#define ISTELNUM(dat)    ((dat>=0x30&&dat<=0x39)||dat=='*'||dat=='#'||dat=='+')
#define ISVALIDCHAR(dat)    (dat>=0x20&&dat<0xff)
#define ISNOTVALIDCHAR(dat)    (!(dat>=0x20&&dat<0xff))

#define ISASCCHAR(dat)    (dat>=0x20&&dat<=0x7f)
#define ISNOTASCCHAR(dat)    (!(dat>=0x20&&dat<=0x7f))
#define COMPRESSDAY(year,mon,day) ((year<<9)+(mon<<5)+day)
#define htons(a) 			((a>>8)|(a<<8))
	
void ProcNor(void);
//pub.c
u16 GetSysms(void);

void PrintfStr(u8 *str,u16 len);

u8 ChangeStr_u8(u8 *str,u8 *dat);
u8 CompStr(u8 *src,u8 * dst,u8 len);

void ClearBuf(u8 *buf,u16 len);
void delayTick(u16 t);



u8 CompStr(u8 *src,u8 * dst,u8 len);
u16 CopyStr(u8 *den,u8 *src,u16 MaxLen);
void  DebugPrintf(u8 *buf,u8 len);
u8 Change2bAsc2U8(u8* buf);
void uart1Isr(u8 dat);
void uart3Isr(u8 dat);
void UART3_SendByte(u8 data);
void ProcPcCmd(void);
void pccmdIsr(u8 dat);
void SetLowPwr(u8 mode);


extern vu16 SysTick,Sys1msCnt;	
#define UART_SENDBUF_SIZE1   128
extern u8 TxBuffer1[UART_SENDBUF_SIZE1];
extern u16 TxPtr1,TxCnt1;


extern u8 InitRf433Ok,AdcCh;
extern u8 LedCnt;

extern u16 GapMsTime;//收到主机数据后的ms计数
extern u8 GapCnt;//当前为第一个空隙gap
extern u8 BatVal;

#define TIMEI   850

void GetGap(void);

#include "stdio.h"

