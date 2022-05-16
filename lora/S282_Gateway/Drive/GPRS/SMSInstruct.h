
#ifndef _SMS_INSTRUCT_H_
#define _SMS_INSTRUCT_H_

#include "def.h"

#define GPRS_MODE				1							//GPRS模式

#define ETH_MODE				2							//网口模式


/*
	6位协议头格式：05 00 03 XX MM NN
	byte 1 : 05, 表示剩余协议头的长度
	byte 2 : 00, 这个值在GSM 03.40规范9.2.3.24.1中规定，表示
							随后的这批超长短信的标识位长度为1（格式中的XX值）。
	byte 3 : 03, 这个值表示剩下短信标识的长度
	byte 4 : XX，这批短信的唯一标志，事实上，SME(手机或者SP)把消息
							合并完之后，就重新记录，所以这个标志是否唯一
							并不是很重要。
	byte 5 : MM, 如果一个超长短信总共5条，这里的值就是5。
	byte 6 : NN, 如果当前短信是这批短信中的第一条的值是1，
							第二条的值是2。
	例如：05 00 03 39 02 01
*/

#define REPLYBUFLEN	 			 	(200)

typedef struct
{
	u8 flag[3];
	u8 GroupID;
	u8 GroupTotal;
	u8 GroupSeq;
}LONGMSGHEAD;

typedef struct
{
	u8 ReplyBuf[REPLYBUFLEN];
	u16 ReplyLen;
	
}REPLYBUF_FRAM;	

void GetLanguageMode(void);

void GSMReadSms(u16 Index,u8 mode);

void SendRestartMsg(void);

void GSMDelSms(u16 Index);

void GSMAnalySms(void);

void SmsSendDat(u8 *InBuf,u16 len);

void SmsAnaSetCmd(u8 *SmsCmd,u8 len);

void GSMReplyFunc(void);

#endif
