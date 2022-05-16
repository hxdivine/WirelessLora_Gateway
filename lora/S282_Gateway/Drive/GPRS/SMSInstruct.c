#include "gd32e10x.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "systick.h"

#include "SMSInstruct.h"
#include "SMSLanguage.h"
#include "hardware.h"
#include "GSM234G.h"
#include "rtc.h"
#include "config.h"
#include "loraDealDat.h"
#include "GprsNet.h"
#include "dealDat.h"
#include "gb_uni.h"
#include "pub.h"

//��Ӣ�ı�ʶ
u8 EngMode = 0;

//������Ӧ������
REPLYBUF_FRAM replayBuf_Fram;

/*
	��ȡ���ֵĳ���
*/
u8 GetNumLen(u16 dat)
{
	u8 len = 1;
	
	while(dat / 10)
	{
		dat /= 10;
		len++;
	}
	
	return len;
}

/*
	������ת�����ַ���
*/
void GetDataValToStr(u16 dat)
{
	u8 len = GetNumLen(dat);
	u8 i;
	u16 num1;
	u8 num2;
	
	i = len - 1;
	while(len--)
	{
		num1 = (u16)pow(10,i--);
		num2 = dat / num1 % 10;
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = num2 + '0';
	}
}

/*
	��ȡ����ģʽ
*/
void GetLanguageMode(void)
{
	if(UserInfo.isChinese == 1)
	{
		EngMode = 0;
	}
	else
	{
		EngMode = 1;
	}
}
/*
	UNICODE ת GB2312 
	������ͣ���λ�ں�.����unicode��,���ͣ�Ҳ�Ǹ�λ�ں�
	
*/
u16 UniToGbFunc(u16 uni)
{
	u16 *gb;
	u16 i;	 
	gb = (u16*)GB2UNI	;
	for(i = 0; i < sizeof(GB2UNI) / 2; i++) 
	{	 
		if(*gb == uni)
		{
			return (i % 94 + 0xa1) + ((i / 94) + 0xb0) * 256;		   
		}
		gb++;								 	
	}
	return 0x2020;
}

/*
	GB2312 ת UNICODE  
	������ͣ���λ�ں�.����unicode��,���ͣ�Ҳ�Ǹ�λ�ں�
	
*/
u16 GbToUniFunc(u16 gb)
{
	u8 Hight,Low;
	Hight = gb >> 8;
	Low = gb & 0xff;
	if((Hight >= 0xb0 && Hight <= 0xf7) && (Low>= 0xa1 && Low <= 0xfe))
	{
		return (GB2UNI[(Hight - 0xb0) * 94 + Low - 0xa1]);//							 	
	}
	return 0x0020;
}
/*
	��SIM���ж�ȡ��Index������
*/
void GSMReadSms(u16 Index,u8 mode)
{
	g_Smsflag &= ~RECSMSOK;
	g_flag &= ~RECOK;
	
	WriteComm("AT+CMGR=");
	if(ModuleVer != 2)
	{
		if(Index)
		{//UC15���Ŵ�0��ʼ
			Index--;
		}
	}
	if(Index < 10)
	{
		PutInUart1((Index % 10) + 0x30);
	}
	else if(Index < 99)
	{
		PutInUart1((Index / 10 % 10) + 0x30);
		PutInUart1((Index % 10) + 0x30);
	}
	else
	{
		PutInUart1((Index / 100) + 0x30);
		PutInUart1((Index / 10 % 10) + 0x30);
		PutInUart1((Index % 10) + 0x30);
	}
	if(mode == 1)
	{//���ı����ʽ
		WriteComm(",1");
	}
	WriteComm("\r\n");
	GSMRecOK(3000);
}
/*
	ɾ������
*/
void GSMDelSms(u16 Index)
{
	g_flag &= ~RECOK;
	
	if(ModuleVer != 2)
	{
		if(Index)
		{//�����0��ʼ
			Index--;
		}
	}
	WriteComm("AT+CMGD=");

	PutInUart1((Index / 100) + 0x30);
	PutInUart1((Index / 10 % 10) + 0x30);
	PutInUart1((Index % 10) + 0x30);
	WriteComm("\r\n");
	GSMRecOK(3000);
}

/*
	������������ �ж�����ȷ��
*/
void GSMAnalySms(void)
{
	u8 ptr;
	u16 i,j;
	u8 recLen,k;
	
	ptr = sms_data_Frame.recbuf[0] + 2;
	UserInfo.SmsTelNoLen = sms_data_Frame.recbuf[ptr];
	UserInfo.SmsTelNoIn = sms_data_Frame.recbuf[ptr + 1];

	for(i = ptr + 2; i < (ptr + 2 + (UserInfo.SmsTelNoLen + 1) / 2); i++)
	{
		if(i - ptr - 2 >= 8)
		{
			return;
		}
		UserInfo.SmsTelNo[i - ptr - 2] = sms_data_Frame.recbuf[i];
	}
	//�ȱ��� �ö��ſ�������������
	SaveUserInfo();
	
	ptr = i + 2;
	/*ʱ��*/
	ptr += 6;
	ptr++;
	
	recLen = sms_data_Frame.recbuf[ptr++];
	
	if(sms_data_Frame.recbuf[i + 1] == 0x00)
	{//Ӣ��
		for(i = 0,j = 0,k = 0; i < recLen; i++)
		{
			if(i >= GSM_LEN - 1)
			{
				break;
			}
			sms_data_Frame.sendbuf[i] = (((sms_data_Frame.recbuf[k + ptr]) << j) & 0x7F) | (sms_data_Frame.recbuf[k + ptr - 1] >> (8 - j));
			j++;
			k++;
			if(j % 8 == 0)
			{
				j = 0;
				k--;
			}
		}
	}
	else
	{//unicode
		for(i = ptr,k = 0; i < recLen + ptr; i += 2)
		{
			if(sms_data_Frame.recbuf[i] == 0 && sms_data_Frame.recbuf[i + 1] < 0x80)
			{//Ӣ�ĸ�ʽ
				sms_data_Frame.sendbuf[k++] = sms_data_Frame.recbuf[i + 1];
			}
			else
			{
				j = UniToGbFunc((sms_data_Frame.recbuf[i] << 8) + sms_data_Frame.recbuf[i + 1]);
				sms_data_Frame.sendbuf[k++] = j >> 8;
				sms_data_Frame.sendbuf[k++] = j & 0xFF;
			}
		}
		//����ָ���
		recLen = k;
	}
	
	if(recLen < GSM_LEN)
	{
		sms_data_Frame.sendbuf[recLen] = '\0';
	}
	
	S282_Log(sms_data_Frame.sendbuf);
	
	if(CompStr(sms_data_Frame.sendbuf,SysInfo.PassCode,4))
	{
		GetLanguageMode();
		SmsAnaSetCmd(&sms_data_Frame.sendbuf[4],recLen - 4);
	}
}

/*
	����
*/
void AutoLineBreak(void)
{
	replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = 0x0D;
	replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = 0x0A;
}

/*
	��ȡ�豸ID
*/
void GetDeviceId(void)
{
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],"ID:",3);
	if(SysInfo.DeviceId > 10)
	{
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.DeviceId / 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.DeviceId % 10 + '0';
	}
	else
	{
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.DeviceId + '0';
	}
}
/*
	��ȡ�豸ʱ��
*/
void GetDeviceTim(void)
{
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = '2';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = (calendar.w_year / 10) % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = (calendar.w_year % 10) + '0';
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_YEAR,4);
		
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.w_month / 10 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.w_month % 10 + '0';
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_MON,4);

		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.w_day / 10 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.w_day % 10 + '0';
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_DAY,4);
	
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.hour / 10 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.hour % 10 + '0';
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_HOUR,4);
	
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.min / 10 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = calendar.min % 10 + '0';
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_MIN,4);
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_UTC,20);
}
/*
	�ַ���ת��ֵ
*/
u8 Str2Num(u8 *str,u16 *dat)
{
	u8 i;
	*dat = 0;
	for(i = 0; i < 5; i++)
	{
		if(!ISNUM(str[i]))
		{
			break;
		}
		*dat = *dat * 10 + str[i] - 0x30;
	}
	return i;
}
/*
	��ȡ�������ȼ�
*/
void GetNetPriority(void)
{
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_NETPRIORITY,32);
	switch(SysInfo.NetLinkPriority)
	{
		case ALL_NETWORK:
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_SAMETIM,32);
			break;
		case CELLULAR_NETWORK:
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_CELL_PRI,32);
			break;
		case ETHER_NETWORK:
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_ETH_PRI,32);
			break;
		default:
			break;
	}
}

/*
	��ȡ�豸״̬
*/
void GetDeviceStatus(void)
{
	GetDeviceTim();
	AutoLineBreak();
	
	GetDeviceId();
	AutoLineBreak();
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_IMEI,32);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],g_IMEI,16);
	AutoLineBreak();
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_GSMVAL,64);
	replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = g_SigLevel / 10 + '0';
	replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = g_SigLevel % 10 + '0';
	AutoLineBreak();
	
	if(!DET5V)
	{//δ���ⲿ��Դ
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_PWR2OFF,64);
	}
	else
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_PWRISOK,64);
	}
	AutoLineBreak();

	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_VERSION,16);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)version,8);
	AutoLineBreak();		
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_MODEL,16);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)UserInfo.version,10);
	AutoLineBreak();
	
	if(ISASCCHAR(SysInfo.DeviceName[0]))
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)SysInfo.DeviceName,64);
	}
}
/*
	����������Ϣ
*/
void SendRestartMsg(void)
{
	replayBuf_Fram.ReplyLen = 0;
	
	replayBuf_Fram.ReplyLen += CopyStr(replayBuf_Fram.ReplyBuf,(u8*)C_SMSREBOOT,64);
	
	SmsSendDat(replayBuf_Fram.ReplyBuf,replayBuf_Fram.ReplyLen);
}
/*
	��ȡ��������ַ�Ͷ˿ں�
*/
void GetGprsServer(void)
{
	if(IsIPaVailable(SysInfo.GprsServerIP) || !ISVALIDCHAR(SysInfo.GprsServerIP[0]))
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_IP,32);
	}
	else
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_DORMAIN,32);
	}
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],SysInfo.GprsServerIP,32);
	AutoLineBreak();
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_PORT,32);
	GetDataValToStr(SysInfo.GprsServerPort);
}
/*
	��ȡ����״̬ GPRS ETH ���� �ر�
*/
void GetNetStatus(u8 model,u8 status)
{
	switch(model)
	{
		case GPRS_MODE:
			if(status & 0x0F)
			{//����
				if(status & 0x0F == 1)
				{
					replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_GPRSOPEN0,64);
				}
				else
				{
					replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_GPRSOPEN1,64);				
				}
			}
			else
			{
				replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_GPRSCLOSE,64);
			}
		break;
		case ETH_MODE:
			if(status & 0x0F)
			{
				if(status & 0x0F == 1)
				{
					replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_ETHOPEN0,64);
				}
				else
				{
					replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_ETHOPEN1,64);
				}
			}
			else
			{
				replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_ETHCLOSE,64);
			}
		break;
		default:
			break;
	}
}

/*
	��ȡAPN USER PWD
*/
void GetGprsAPNConfig(void)
{
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_APN,32);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],SysInfo.GprsApn,32);
	AutoLineBreak();
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_USR,32);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],SysInfo.GprsUsr,32);
	AutoLineBreak();
	
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_PWD,32);
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],SysInfo.GprsPwd,32);
	AutoLineBreak();
}
/*
	��ȡ��̫���������Ͷ˿ں�
*/
void GetEthServer(void)
{
	if(IsIPaVailable(SysInfo.EthTarIP) || !ISVALIDCHAR(SysInfo.EthTarIP[0]))
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_IP,32);
	}
	else
	{
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_DORMAIN,32);
	}
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],SysInfo.EthTarIP,MAXGPRSIP);
	AutoLineBreak();
	replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_PORT,32);
	GetDataValToStr(SysInfo.EthTarTcpPort);
}

LONGMSGHEAD longMsgHead;
/*
	���Ͷ���ָ��
	��UserInfo.SmsTelNo��ȡ�绰����
	��˵��Σ�
	INBUF 			���ͻ�����
	ACTIONLEN 	����ʵ�ʳ���
	LEN 				�ܳ���
	CHINESEFLAG ���ı�ʶ
	LONGMSGFLAG �����ű�ʶ
*/
void SmsSendGsm(u8 *InBuf,u8 ActionLen,u8 len,u8 chineseFlag,u8 LongMsgFlag)
{
	u8 MsgLen = 0;
	
	if(!(g_flag & RECOK))
	{
		return;
	}
	delay_1ms(100);  
	//����ָ������
	WriteComm("AT+CMGS=");
	//unicode ���ͳ�����ֵ
	//��2Ϊȥβ����
	MsgLen = (4 + (UserInfo.SmsTelNoLen + 1) / 2 + 3 + len);
	
	if(longMsgHead.flag[0])
	{
		MsgLen += 6;
	}
	//���ݳ���
	if(MsgLen < 10)
	{
		PutInUart1(MsgLen + '0');
	}
	else if(MsgLen < 100)
	{
		PutInUart1(MsgLen / 10 + '0');
		PutInUart1(MsgLen % 10 + '0');
	}
	else
	{
		PutInUart1(MsgLen / 100 + '0');
		PutInUart1(MsgLen / 10 % 10 + '0');
		PutInUart1(MsgLen % 10 + '0');
	}

	PutInUart1(0x0D);
	//�ȴ����>�� �����������
	WaitGsmChar('>',200);
	SendHexAsAsc(0x00);
	
	if(LongMsgFlag == 1)
	{//������ ����Э��ͷ��Ϣ
		SendHexAsAsc(0x41);
	}
	else
	{
		SendHexAsAsc(0x01);
	}
	SendHexAsAsc(0x03);
	SendHexAsAsc(UserInfo.SmsTelNoLen);
	//��żУ��
	SendHexAsAsc(UserInfo.SmsTelNoIn);
	
	for(u8 i = 0; i < (UserInfo.SmsTelNoLen + 1) / 2; i++)
	{//���ź���
		SendHexAsAsc(UserInfo.SmsTelNo[i]);
	}
	SendHexAsAsc(0x00);
	
	if(chineseFlag == 1)
	{//00ΪӢ�ĸ�ʽ 08ΪUnicode���ĸ�ʽ
		SendHexAsAsc(0x08);
	}
	else
	{
		SendHexAsAsc(0x00);
	}
	
	if(longMsgHead.flag[0])
	{//������
		if(chineseFlag)
		{//����Ϣʵ�ʳ���
			SendHexAsAsc(ActionLen + 6);
		}
		else
		{	//����Ϣʵ�ʳ�,��һ�ַ���ǰ��6����־����ʾ8���ַ�
			SendHexAsAsc(ActionLen - 1 + 8);
		}
		for(u8 i = 0; i < longMsgHead.flag[0] + 1; i++)
		{
			SendHexAsAsc(longMsgHead.flag[i]);
		}
		
	}
	else
	{//����Ϣʵ�ʳ���
		SendHexAsAsc(ActionLen);
	}
	
	for(u8 i = 0; i < len; i++)
	{//���Ͷ���
		SendHexAsAsc(InBuf[i]);
	}
	g_flag &= ~RECOK;
	g_Smsflag &= ~SMSSENDOK;
	//������
	PutInUart1(0x1A);
	
	if(GSMRecOK(7000) == 1)
	{
		S282_Log("SEND OK");
	}
	else
	{
		S282_Log("SEND FAIL");
	}
}

/*
	���Ͷ���
	�Զ��Ÿ�ʽ���д���
	�����Ļ���Ӣ�� �ǳ����Ż��Ƿǳ�����
	ʵ�ʷ��Ͷ��ŵ��� SMSSENDGSM����
*/
void SmsSendDat(u8 *InBuf,u16 len)
{
#define ZH_LONG_LEN 134		//�������134�ֽ�
#define EN_LONG_LEN	153		//Ӣ�����153�ֽ�
#define MAXSMSNO		8			//�����Ϣ��
	
	u8 chineseFlag = 0;
	u8 CharNo = 0;
	u16 outPtr,inPtr,uni;
	u16 j,k;
	
	static u8 longMsgID = 0;
	
	for(u16 i = 0; i < len; i++)
	{
		if(InBuf[i] >= 0x80)
		{//�Ǻ��� һ������ռ�����ֽ�
			chineseFlag = 1;
			i++;
		}
		CharNo++;
	}
	if(chineseFlag == 1)
	{//��������
		if(CharNo <= 70)
		{//�ǳ�����
			//�ǳ�����
			longMsgHead.flag[0] = 0;
			outPtr = 0;
			
			for(u16 i = 0; i < len; i++)
			{
				if(InBuf[i] == 0xFF || InBuf[i] == 0)
				{
					break;
				}
				
				if(InBuf[i] > 0x80)
				{//����
					uni = GbToUniFunc(InBuf[i] * 256 + InBuf[i + 1]);
					i++;
					sms_data_Frame.sendbuf[outPtr++] = uni >> 8;
					sms_data_Frame.sendbuf[outPtr++] = uni & 0xFF;
				}
				else
				{
					sms_data_Frame.sendbuf[outPtr++] = 0x00;
					sms_data_Frame.sendbuf[outPtr++] = InBuf[i];
				}
			}
			//���Ͷ���
			SmsSendGsm(sms_data_Frame.sendbuf,outPtr,outPtr,chineseFlag,0);
		}
		else
		{//������
			//��Ϣ������
			longMsgID++;
			longMsgHead.flag[0] = 5;
			longMsgHead.flag[1] = 0;
			longMsgHead.flag[2] = 3;
			longMsgHead.GroupID = longMsgID;
			longMsgHead.GroupTotal = (CharNo * 2 + ZH_LONG_LEN - 1) / ZH_LONG_LEN;
			if(longMsgHead.GroupTotal > MAXSMSNO)
			{
				longMsgHead.GroupTotal = MAXSMSNO;
			}
			longMsgHead.GroupSeq = 1;
			
			inPtr = 0;
			while(1)
			{//����̫�� �ֶ�η���
				for(outPtr = 0; outPtr < ZH_LONG_LEN && inPtr < len; inPtr++)
				{
					if(InBuf[inPtr] == 0xFF || InBuf[inPtr] == 0)
					{
						break;
					}
					if(InBuf[inPtr] > 0x80)
					{//����
						uni = GbToUniFunc(InBuf[inPtr] * 256 + InBuf[inPtr + 1]);
						inPtr++;
						sms_data_Frame.sendbuf[outPtr++] = uni >> 8;
						sms_data_Frame.sendbuf[outPtr++] = uni & 0xFF;
					}
					else
					{
						sms_data_Frame.sendbuf[outPtr++] = 0x00;
						sms_data_Frame.sendbuf[outPtr++] = InBuf[inPtr];
					}
				}
				//���Ͷ��� ����ʵ���ı������뷢�ͳ�����ͬ
				SmsSendGsm(sms_data_Frame.sendbuf,outPtr,outPtr,chineseFlag,1);
				longMsgHead.GroupSeq++;
				if(longMsgHead.GroupSeq > MAXSMSNO)
				{//�����������
					break;
				}
				if(inPtr >= len)
				{//InBuf���ݷ������
					break;
				}
			}
		}
	}		
	else
	{
		if(len <= 160)
		{//��ͨӢ��
			longMsgHead.flag[0] = 0; //�ǳ�����
			outPtr = len - len / 8;
			
			j = 0;
			k = 0;
			for(u16 i = 0; i < outPtr; i++)
			{
				if(j == len - 1)
				{
					sms_data_Frame.sendbuf[i] = InBuf[j] >> k;
				}
				else
				{
					sms_data_Frame.sendbuf[i] = (InBuf[j] >> k) | (InBuf[j + 1] << (7 - k));
				}
				j++;
				k = (k + 1) % 7; //0123456
				if(k == 0)
				{
					j++;
				}
			}
			SmsSendGsm(sms_data_Frame.sendbuf,len,outPtr,chineseFlag,0);
		}
		else
		{//Ӣ�ĳ�����
			if(len > EN_LONG_LEN * MAXSMSNO)
			{
				len = EN_LONG_LEN * MAXSMSNO;
			}
			longMsgID++;
			longMsgHead.flag[0] = 5;
			longMsgHead.flag[1] = 0;
			longMsgHead.flag[2] = 3;
			longMsgHead.GroupID = longMsgID;
			//Ӣ�Ķ������153�ֽ�
			longMsgHead.GroupTotal = (len + EN_LONG_LEN - 1) / EN_LONG_LEN;
			if(longMsgHead.GroupTotal > MAXSMSNO)
			{
				longMsgHead.GroupTotal = MAXSMSNO;
			}
			longMsgHead.GroupSeq = 1;
			inPtr = 1;
			u8 lastInPtr = 0;
			while(1)
			{
				j = 0; 
				k = 6;//6���ֽڵ���Ϣͷ LONGMSGHEAD
				//���׼���ż��� ��һ���ֽ���ǰ6���ֽںϲ�
				sms_data_Frame.sendbuf[0] = InBuf[inPtr++] << (7 - k);
				
				k = 0;
				for(outPtr = 1; outPtr < EN_LONG_LEN && inPtr < len; outPtr++)
				{
					if(inPtr >= len - 1)
					{//���һ���ֽ�
						sms_data_Frame.sendbuf[outPtr++] = InBuf[inPtr++] >> k;
						break;
					}
					else
					{
						sms_data_Frame.sendbuf[outPtr] = (InBuf[inPtr] >> k) | (InBuf[inPtr + 1] << (7 - k));
					}
					inPtr++;
					k = (k + 1) % 7;
					if(k == 0)
					{
						inPtr++;
					}
				}
				//inptr - lastInptr Ϊʵ�ʳ���
				SmsSendGsm(sms_data_Frame.sendbuf,inPtr - lastInPtr,outPtr,chineseFlag,1);
				longMsgHead.GroupSeq++;
				
				if(longMsgHead.GroupSeq > MAXSMSNO)
				{
					break;
				}
				if(inPtr >= len)
				{
					break;
				}
				lastInPtr = inPtr;
			}
		}
	}
}

/*
	��ȡ�ն��豸������ͨ��״̬
*/
void GetDevCommunicateState(u8 mode)
{
	for(u8 i = 0,j = 0; i < DEVICE_MAX; i++)
	{		
		if(DevInfo[i].deviceId > 0 && DevInfo[i].deviceId <= DEVICE_MAX)
		{
			if(DevInfo[i].isExist == mode)
			{//����ʧ���豸
				if(j == 0)
				{
					j = 1;
					if(mode)
					{
						replayBuf_Fram.ReplyLen += sprintf((char*)&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(char*)C_DEVNOR);
					}
					else
					{
						replayBuf_Fram.ReplyLen += sprintf((char*)&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(char*)C_DEVABNOR);
					}
				}
				replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = DevInfo[i].deviceId / 10 + '0';
				replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = DevInfo[i].deviceId % 10 + '0';
				replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = ',';
			}
		}
	}
	if(replayBuf_Fram.ReplyLen)
	{
		replayBuf_Fram.ReplyLen--;
		AutoLineBreak();
	}
	
}

/*
	��������ָ������
*/
void SmsAnaSetCmd(u8 *SmsCmd,u8 len)
{
	static u8 IsBusy = 0;
	
	u8 i,j;
	u8 tempFlag = 0,resetFlag = 0,CodeFlag = 1;
	u16 tempVal = 0;
	u8	year,mon,day,hour,min;
	u8 	deviceNo = 0;
	
	if(IsBusy == 1)
	{//��ֹ����
		return;
	}
	IsBusy = 1;
	//�����λ�����ǻ���
	if(SmsCmd[len - 1] < 0x20)
	{
		len--;
	}
	if(SmsCmd[len - 1] < 0x20)
	{
		len--;
	}
	//��շ���������
	replayBuf_Fram.ReplyLen = 0;
	
	SmsCmd[len] = '\0';
	if(len == 5 && SmsCmd[0] == 'P')
	{
		for(i = 0; i < 4; i++)
		{
			if(!ISASCCHAR(SmsCmd[i + 1]))
			{//���ַ�
				break;
			}
		}
		if(i >= 4)
		{
			for(i = 0; i < 4; i++)
			{
				SysInfo.PassCode[i] = SmsCmd[i + 1];
			}
			SaveSysInfo();
			replayBuf_Fram.ReplyLen += CopyStr(replayBuf_Fram.ReplyBuf,SysInfo.PassCode,4);
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_SETPASSOK,64);
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(SmsCmd[0] == 'I' && SmsCmd[1] == 'D')
	{//ID
		//��ѯ�豸ID
		if(SmsCmd[2] == 'E')
		{//ID:xxx
			GetDeviceId();
		}
		else 
		{//�����豸IDxxxxx
			for(i = 2; i < len; i++)
			{
				if(!ISNUM(SmsCmd[i]))
				{//������
					tempFlag = 1;
					break;
				}
			}
			if(i < 8 && tempFlag == 0)
			{//�豸ID���ô�247
				for(i = 2; i < len; i++)
				{
					tempVal = tempVal * 10 + SmsCmd[i] - '0'; 
				}
				if(tempVal < 248 && tempVal > 0)
				{
					SysInfo.DeviceId = tempVal;
					SaveSysInfo();
					//ID:xxx
					GetDeviceId();
				}
				else
				{
					CodeFlag = 0;
				}
			}
			else
			{
				CodeFlag = 0;
			}
		}
	}
	else if(SmsCmd[0] == 'D' && SmsCmd[7] == 'T')
	{//D201224T1212   2020��12��24��12ʱ12��
		for(i = 1; i < len; i++)
		{
			if( (!ISNUM(SmsCmd[i])) && (i != 7))
			{//���ӷ�����
				tempFlag = 1;
				break;
			}
		}
		if(i < 13 && tempFlag == 0)
		{
			year = StrToNum(&SmsCmd[0]);
			mon = StrToNum(&SmsCmd[3]);;
			day = StrToNum(&SmsCmd[6]);;
			hour = StrToNum(&SmsCmd[9]);;
			min = StrToNum(&SmsCmd[12]);;
			if(mon < 13 && day < 32 && hour < 60 && min < 60)
			{
				RTC_Set(year,mon,day,hour,min,0);
				RTC_Get();
				GetDeviceTim();
			}
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(SmsCmd[0] == 'D' && len == 1)
	{//D
		GetDeviceTim();
	}
	else if(SmsCmd[0] == 'E' && SmsCmd[1] == 'E' && len == 2)
	{
		GetDeviceStatus();
	}
	else if(CompStr(SmsCmd,"NET",3))
	{
		if(len == 3)
		{
			GetNetPriority();
		}
		else if(len == 4 && (0x30 <= SmsCmd[3] < 0x33))
		{
			SysInfo.NetLinkPriority = SmsCmd[3];
			SaveSysInfo();
			GetNetPriority();
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(CompStr(SmsCmd,"GPRSIP",6))
	{
		if(CompStr(&SmsCmd[6],"DEL",3))
		{//GPRSIPDEL
			//�Ͽ�����
			GprsDisConnect();
			//GPRS ������IP��ַ��Ϊ0
			memset(SysInfo.GprsServerIP,0,MAXGPRSIP);
			SysInfo.GprsServerPort = 0;
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				memset(SysInfo.EthTarIP,0,MAXGPRSIP);
				SysInfo.EthTarTcpPort = 0;
			}
			
			//����
			SaveSysInfo();
			GetGprsServer();
		}
		else if(len == 6)
		{//GPRSIP
			GetGprsServer();
		}
		else if(len > 6)
		{//GPRSIPxxxxxxx*xxxx
			for(i = 0; i <= MAXGPRSIP + 1; i++)
			{
				if(SmsCmd[i + 6] == '*')
				{
					break;
				}
			}
			if( i <= MAXGPRSIP && len > i)
			{
				SmsCmd[i + 6] = 0;
				CopyStr(&SysInfo.GprsServerIP[0],&SmsCmd[6],i);					
				SysInfo.GprsServerPort = StrToNum(&SmsCmd[i + 7]);
				//GprsConnRetryCnt=0;
				if(SysInfo.NetLinkPriority != ALL_NETWORK)
				{
					CopyStr(SysInfo.EthTarIP,&SmsCmd[5],i);	
					SysInfo.EthTarTcpPort = SysInfo.GprsServerPort;
				}
				
				g_flag |= GPRSNEEDRECONN;
				SaveSysInfo();
				
				GetGprsServer();
			}
		}	
		else
		{
			CodeFlag = 0;
		}
	}
	else if(SmsCmd[0] == 'A' && SmsCmd[1] == 'P')
	{
		if(CompStr(&SmsCmd[2],"DEL",3))
		{//APDEL
			memset(SysInfo.GprsApn,0,MAXGPRSAPN);
			memset(SysInfo.GprsUsr,0,MAXGPRSUSR);
			memset(SysInfo.GprsPwd,0,MAXGPRSUSR);
			SaveSysInfo();
			GetGprsAPNConfig();
		}
		else if(len == 2)
		{//AP
			GetGprsAPNConfig();
		}
		else if(len > 4)
		{//APxxxxx#xxxx#xxxx
			for(i = 2,j = 0; i < len; i++)
			{
				if(SmsCmd[i] == '#')
				{
					j++;
					if(j == 2)
					{
						break;
					}
				}
			}
			//��ʽ��ȷ
			if(j == 2 && i < len)
			{
				
				for(i = 0; i < MAXGPRSAPN + 1; i++)
				{
					if(SmsCmd[i + 2] == '#')
					{
						break;
					}
				}
				//�ҵ�APN
				j = i + 2 + 1;// #��һ��λ��
				CopyStr(SysInfo.GprsApn,&SmsCmd[2], i);
				for(i = 0; i < MAXGPRSUSR + 1; i++)
				{
					if(SmsCmd[j + i] == '#')
					{
						break;
					}
				}
				//�ҵ�USER
				CopyStr(SysInfo.GprsUsr,&SmsCmd[j],i); 
				j += i + 1;
				if(len - j > MAXGPRSUSR)
				{//���೤�� ����
					CopyStr(SysInfo.GprsPwd,&SmsCmd[j],MAXGPRSUSR);
				}
				else
				{
					CopyStr(SysInfo.GprsPwd,&SmsCmd[j],len - j);
				}
				SaveSysInfo();
				GetGprsAPNConfig();
				//��Ҫ����APN
				NeedSetAPN = 1;
			}
			else
			{
				CodeFlag = 0;
			}
		}	
		else
		{
			CodeFlag = 0;
		}
	}	
	else if(CompStr(SmsCmd,"GPRSonline",10))
	{//GPRS����
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_GPRSONLINE,64);
	}
	else if(CompStr(SmsCmd,"GPRS",4))
	{
		if(CompStr(&SmsCmd[4],"ON:MODBUS",9))
		{//GPRSON1  ����������
			SysInfo.GprsMode |= 0x01;
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				SysInfo.Rj45Mode |= 0x01;
			}
			
			SaveSysInfo();
			GetNetStatus(GPRS_MODE,SysInfo.GprsMode);
		}
		else if(CompStr(&SmsCmd[4],"ON:MQTT",7))
		{
			SysInfo.GprsMode |= 0x02;
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				SysInfo.Rj45Mode |= 0x02;
			}
			
			SaveSysInfo();
			GetNetStatus(GPRS_MODE,SysInfo.GprsMode);
		}
		else if(CompStr(&SmsCmd[4],"OFF",3))
		{//GPRSOFF	�رշ�����
			SysInfo.GprsMode &= ~(0x0F);
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				SysInfo.Rj45Mode &= ~(0x0F);
			}
			GetNetStatus(GPRS_MODE,SysInfo.GprsMode);
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(CompStr(SmsCmd,"ETHON:MODBUS",12))
	{//ETHON1    ��������
		SysInfo.Rj45Mode |= 0x01;
		if(SysInfo.NetLinkPriority != ALL_NETWORK)
		{
			SysInfo.GprsMode |= 0x01;
		}
		SaveSysInfo();
		GetNetStatus(ETH_MODE,SysInfo.Rj45Mode);
	}
	else if(CompStr(SmsCmd,"ETHON:MQTT",10))
	{
		SysInfo.Rj45Mode |= 0x02;
		if(SysInfo.NetLinkPriority != ALL_NETWORK)
		{
			SysInfo.GprsMode |= 0x02;
		}
		SaveSysInfo();
		GetNetStatus(ETH_MODE,SysInfo.Rj45Mode);
	}
	else if(CompStr(SmsCmd,"ETHOFF",6))
	{//ETHOFF		�ر�����
		SysInfo.Rj45Mode &= ~(0x0F);
		if(SysInfo.NetLinkPriority != ALL_NETWORK)
		{
			SysInfo.GprsMode &= ~(0x0F);
		}
		
		SaveSysInfo();
		GetNetStatus(ETH_MODE,SysInfo.Rj45Mode);
	}
	else if(CompStr(SmsCmd,"INTE",4))
	{//INTE ��ѯ����״̬
		GetNetStatus(GPRS_MODE,SysInfo.GprsMode);
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = ',';
		GetNetStatus(ETH_MODE,SysInfo.Rj45Mode);
	}		
	else if(CompStr(SmsCmd,"ETHIP",5))
	{
		if(CompStr(&SmsCmd[5],"DEL",3))
		{//ETHIPDEL
			memset(SysInfo.EthTarIP,0,MAXGPRSIP);
			SysInfo.EthTarTcpPort = 0;
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				memset(SysInfo.GprsServerIP,0,MAXGPRSIP);
				SysInfo.GprsServerPort = 0;
			}
			
			SaveSysInfo();
			GetEthServer();
		}
		else if(len == 5)
		{//ETHIP
			GetEthServer();
		}
		else if(len > 6)
		{//ETHIPxxxx*xxxx
			for(i = 0; i <= MAXGPRSIP + 1; i++)
			{
				if(SmsCmd[i + 5] == '*')
				{
					break;
				}
			}
			if( i <= MAXGPRSIP && len > i)
			{
				memset(SysInfo.EthTarIP,0,MAXGPRSIP);
				SmsCmd[i + 5]=0;
				CopyStr(&SysInfo.EthTarIP[0],&SmsCmd[5],i);					
				SysInfo.EthTarTcpPort = StrToNum(&SmsCmd[i + 6]);
				
				if(SysInfo.NetLinkPriority != ALL_NETWORK)
				{
					CopyStr(SysInfo.GprsServerIP,&SmsCmd[5],i);	
					SysInfo.GprsServerPort = SysInfo.EthTarTcpPort;
				}
				SaveSysInfo();
				GetEthServer();
			}
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(CompStr(SmsCmd,"RECONT",6))
	{//RECONTxxx �����豸���� ����
		if(ISNUM(SmsCmd[6]))
		{		
			SysInfo.GprsRetryCnt = StrToNum(&SmsCmd[6]);
			if(SysInfo.GprsRetryCnt > 255)
			{
				SysInfo.GprsRetryCnt = 1;
			}				
			if(SysInfo.NetLinkPriority != ALL_NETWORK)
			{
				SysInfo.EthRetryCnt = SysInfo.GprsRetryCnt;
			}
			SaveSysInfo();
		}
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_RECONT,64);

		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.GprsRetryCnt / 100 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.GprsRetryCnt / 10 % 10 + '0';
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = SysInfo.GprsRetryCnt % 10 + '0';
		
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_COUNT,64);
	}
	else if(CompStr(SmsCmd,"REBOOT",6))
	{//����
		UserInfo.isReboot = 0x3456;
		SaveUserInfo();
		NVIC_SystemReset();
	}
	else if(CompStr(SmsCmd,"RESET",5))
	{//�ָ���������
		resetFlag = 1;
		replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)REFACTOK,32);
	}
	else if(CompStr(SmsCmd,"DELDEV",6))
	{//ɾ���ն��豸 DELDEVxxx
		deviceNo = (SmsCmd[6] - '0') * 10 + SmsCmd[7] - '0';
		if(deviceNo > 0 && deviceNo <= DEVICE_MAX)
		{
			DelDevWT2Config(deviceNo-1);
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_DELDEV1,16);

			GetDataValToStr(deviceNo);
			replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(u8*)C_DELHIS,16);
		}
		else
		{
			CodeFlag = 0;
		}
	}
	else if(CompStr(SmsCmd,"RCU",4))
	{
		if(SmsCmd[3] == 'C')
		{//��ѯ���ն�ͨ��״̬ RCUC
			GetDevCommunicateState(1);
			GetDevCommunicateState(0);
			if(replayBuf_Fram.ReplyLen < 10)
			{
				
				replayBuf_Fram.ReplyLen += sprintf((char*)&replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen],(char*)C_DEVNO);
			}
		}
		else
		{//��ѯ�Ĵ�����ǰֵ  RCUxxyyzz   xxyyzz�ǵ�ַ
			u8 Index,num;
			for(i = 3; i < len; i += 2)
			{
				if(ISNUM(SmsCmd[i]) && ISNUM(SmsCmd[i + 1]))
				{//��ȡ�ն�ID
					deviceNo = (SmsCmd[i] - '0') * 10 + SmsCmd[i + 1] - '0';
					if(deviceNo == 0 || deviceNo > DEVICE_MAX)
					{
						break;
					}
					replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = 'R';
					replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = deviceNo / 10 + '0';
					replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = deviceNo % 10 + '0';				
					replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = ':';		
					deviceNo -= 1;
					switch(DevInfo[deviceNo].Type)
					{
						case DEV_WT102:
						case DEV_WT103:
						case DEV_WT104:
						case DEV_WT105:
						case DEV_WT106:
							num = 2;
						break;
						case DEV_WT101:
							num = 4;
							break;
						case DEV_WT108:
						case DEV_WT109:
							num = 1;
						break;
						default:
							num = 7;
					}
					for(j = 0; j < num; j++)
					{					
						GetDataValToStr(DevRegVal[deviceNo * 7 + j]);
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = ',';
					}	
					if(DevAlarm[deviceNo] == 1)
					{
						replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[--replayBuf_Fram.ReplyLen],(u8*)"(Y)",16);					
					}
					else
					{
						replayBuf_Fram.ReplyLen += CopyStr(&replayBuf_Fram.ReplyBuf[--replayBuf_Fram.ReplyLen],(u8*)"(N)",16);	
					}
					if(DevInfo[Index].Type >= DEV_WT103 && DevInfo[Index].Type <= DEV_WT104)
					{
						GetDataValToStr(DevRegVal[deviceNo * 7 + 2] / 10);
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = '.';
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = DevRegVal[deviceNo * 7 + 2] % 10 + '0';
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = 'V';
					}
					else 	if(DevInfo[Index].Type >= DEV_WT105 && DevInfo[Index].Type <= DEV_WT106)
					{
						GetDataValToStr(DevRegVal[deviceNo * 7 + 1] / 10);
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = '.';
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = DevRegVal[deviceNo * 7 + 1] % 10 + '0';
						replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen++] = 'V';
					}
					AutoLineBreak();	
				}
				else 
				{
					CodeFlag = 1;
					break;
				}
			}
			if(replayBuf_Fram.ReplyLen == 0)
			{
				CodeFlag = 1;
			}
		}	
	}
	else
	{
		CodeFlag = 0;
	}
	
	if(CodeFlag == 0)
	{
		replayBuf_Fram.ReplyLen = CopyStr(replayBuf_Fram.ReplyBuf,(u8*)C_FORMATERR,64);
	}
	SmsCmd[0] = '\0';
	//ģʽ1 ���Ͷ���
	GSMReplyFunc();
	if(resetFlag)
	{
		SetSysInfoDef();
	}
	
	IsBusy = 0;
}
/*
	GSM ��Ӧ
	mode 1 ����
	mode 2 GPRS����
*/
void GSMReplyFunc(void)
{
	if(replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen - 2] == 0x0D &&
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen - 1] == 0x0A )
	{
		replayBuf_Fram.ReplyBuf[replayBuf_Fram.ReplyLen - 2] = '\0';
		replayBuf_Fram.ReplyLen -= 2;
	}
	SmsSendDat(replayBuf_Fram.ReplyBuf,replayBuf_Fram.ReplyLen);
	replayBuf_Fram.ReplyLen = 0;
}
