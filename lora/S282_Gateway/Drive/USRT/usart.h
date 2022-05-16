
#ifndef _USART_H_
#define _USART_H_

#include "def.h"
#include "gd32e10x.h"

#define SENDBUFF_MAX 	128
#define RECBUFF_MAX		1400
extern u8 TxBuffer0[SENDBUFF_MAX];
extern u8 TxBuffer1[SENDBUFF_MAX];

extern __IO u16 TxPtr0,TxCnt0;
extern __IO u16 TxPtr1,TxCnt1;

typedef struct
{
	u8 	recBuf[RECBUFF_MAX];
	u16 recLen;
	u8 	recFlag;
	u8 sendFlag;
	
}USART_FRAME_STRUCT;	

extern USART_FRAME_STRUCT usart_Frame;

extern u8 sendStop;

void InitUsart0(u32 baudRate);

void InitUsart1(u32 baudRate);

#endif
