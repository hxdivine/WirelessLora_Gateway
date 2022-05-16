
#ifndef _HC_USART_H_
#define _HC_USART_H_

#include "def.h"
#include "gpio.h"
#include "uart.h"

void Usart1Init(void);

void Usart0Init(void);

void SendStr(u8 *InBuf,u8 len);

void CloseUartRx(void);

#define MAX_LEN 10

typedef struct
{
	u8 mRepBuf[MAX_LEN];
	u8 len;
	
}USART_REC;

extern USART_REC usartRec;

extern u8 RxBuf[];
extern u8 UartLen;

#endif
