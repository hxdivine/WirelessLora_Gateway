
#ifndef _SMS_INSTRUCT_H_
#define _SMS_INSTRUCT_H_

#include "def.h"

#define GPRS_MODE				1							//GPRSģʽ

#define ETH_MODE				2							//����ģʽ


/*
	6λЭ��ͷ��ʽ��05 00 03 XX MM NN
	byte 1 : 05, ��ʾʣ��Э��ͷ�ĳ���
	byte 2 : 00, ���ֵ��GSM 03.40�淶9.2.3.24.1�й涨����ʾ
							���������������ŵı�ʶλ����Ϊ1����ʽ�е�XXֵ����
	byte 3 : 03, ���ֵ��ʾʣ�¶��ű�ʶ�ĳ���
	byte 4 : XX���������ŵ�Ψһ��־����ʵ�ϣ�SME(�ֻ�����SP)����Ϣ
							�ϲ���֮�󣬾����¼�¼�����������־�Ƿ�Ψһ
							�����Ǻ���Ҫ��
	byte 5 : MM, ���һ�����������ܹ�5���������ֵ����5��
	byte 6 : NN, �����ǰ���������������еĵ�һ����ֵ��1��
							�ڶ�����ֵ��2��
	���磺05 00 03 39 02 01
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
