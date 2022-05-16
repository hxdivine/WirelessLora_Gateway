#include "stdlib.h"
#include "systick.h"

#include "GprsNet.h"
#include "GSM234G.h"
#include "SMSInstruct.h"
#include "config.h"
#include "hardware.h"
#include "dealDat.h"
#include "modbus.h"
#include "mqtt.h"
#include "iwdg.h"
#include "pub.h"
#include "netLink.h"

u8 g_GprsState = S_FREE;
//����ʱ��
u16 HeartBeatStamp;
//��������ʱ��
u16 SendDataStamp;
//��������ʱ��
u16 GprsFreeStamp;

/*
	�ȽϽ��յ���GPRS�����Ƿ�һ�� 
	����ע��� ��Ӧ�����
	һ�·��� 0
*/
u8 CompGprsData(u8 *str)
{
	u8 len = str[SYS_TXT_DATLEN];
	
	if(gprs_data_Frame.recLen < len)
	{//���ճ��Ȳ�һ��
		return 1;
	}
	
	for(u8 i = 0; i < len; i++)
	{
		if(str[i] != gprs_data_Frame.recBuf[i])
		{
			return 1;
		}
	}
	return 0;
}

/*
	ѭ������������  �ȴ�GPRS����
*/
void GprsRunning(void)
{
	static u16 RegTime = 0;			//��GSM������ ����Ƿ���Ҫ����
	static u32 GsmNoreg = 0;		//���4Gģ���Ƿ���� ������������
	static u8 failTryCnt = 0;
	if(g_flag & POWERDOWN)
	{//Gsm ��Դ�ر�
		g_flag = 0;
		return;
	}
	GsmNoreg++;
	if(GsmNoreg > 1000 * 360)
	{//GSM ����״̬������ ����GSM δ�忨
		RestartGSM();
		GsmNoreg = 1;
	}
	
	if((g_flag & REGOK) || (g_flag & CGATT))
	{
		//������GPRS����
		if(SysInfo.GprsMode & 0x0F)
		{
			if((SysInfo.NetLinkPriority != ETHER_NETWORK) || NetSelected == 1)
			{//NetSelected = 1 ��̫�������� ʹ��GPRS
				GprsFun();
			}
		}
		else if(SysInfo.NetLinkPriority == CELLULAR_NETWORK)
		{
			NetSelected = 2;
		}
		if(NeedSetAPN == 1)
		{//����APN
			NeedSetAPN = 0;
			GSMSetGprs();
		}

		if(RegTime == 0)
		{
			RegTime = GetSecCnt();
		}
		else if(ABSVALUE(GetSecCnt(),RegTime) > 2)
		{//����2s
			if(UserInfo.isReboot == 0x3456)
			{//����������
				GetLanguageMode();
				UserInfo.isReboot = 0x11;
				SaveUserInfo();
				//���Ͷ���
				SendRestartMsg();
			}
			RegTime = 0;
		}
		GsmNoreg = 0;
		SmsFun();
		failTryCnt = 0;
	}
	else
	{
		RegTime = 0;
		if((GsmNoreg & 0x3FF) == 0)
		{//��ʱ�����粻����
			//��ȡ����״̬
			GSMGetReg();
			failTryCnt++;
			if(failTryCnt > 15)
			{//����10������ ����δ�忨 ʹ������
				if((SysInfo.GprsMode & 0x0F) && SysInfo.NetLinkPriority == CELLULAR_NETWORK)
				{
					NetSelected = 2;
				}
			}
		}
	}
}
/*
	��������������

	ע��� + ������
*/
void SendGprsWithReg(void)
{
	u16 i; 	
	u8 *buf = malloc(200);
	u8 RegLen = SysInfo.GprsRegDat[SYS_TXT_DATLEN];
	u8 len = SysInfo.GprsHeartCont[SYS_TXT_DATLEN];
	SendDataStamp = GetSecCnt();
	
	for(i = 0; i < len; i++)
	{  //����  ǰ������Ϊע������� ��һ��Ϊ��������
		buf[RegLen + i] = SysInfo.GprsHeartCont[i];
	}	
	if(SysInfo.GprsRegMode >= 1 && RegLen > 0)
	{	
		for(i = 0; i < RegLen; i++)
		{
			buf[i]= SysInfo.GprsRegDat[i];
		} 	
		SendGprsDat(buf,RegLen + len);
	}
	else 
	{
	  SendGprsDat(&buf[RegLen],len);
	}
	free(buf);
}

/*
	����GPRS ���ӷ�����
*/
void GprsFun(void)
{
	//��һ���ϵ� ��������
	static u8 PwrOnConn = 0;
	//������������
	static u8 ConnTryCnt = 0;
	//�������ݷ��ʹ������� ��Ӧ�ط�����
	static u8 SendTryCnt = 0;
	//�����ж϶�����ӷ�����ʧ�� ���� ��̫������
	static u8 FailTryCnt = 0;
	//1.����ʱ���ӳ�	2.������ʷ���ݱ���ʱ������
	static u16 TimDelay = 0;
	//�Ƿ����״̬
	static u8 mqttConSta = 0;
	u16 Port;
	u8 *GprsIp;
	
	if(TimDelay > 0)
	{//��Ҫ�����ӳ� ���������ɳ���
		TimDelay--;
		return;
	}
	if(FailTryCnt > 2 && SysInfo.NetLinkPriority == CELLULAR_NETWORK)
	{//��������ʧ�� ������̫������
		FailTryCnt = 0;
		NetSelected = 2;
		TimDelay = 7000;
		return;
	}
	if(!(g_flag & TCPCONNECTED))
	{//û������
		NeedOffline = 0;
		if(PwrOnConn == 0)
		{//��һ���ϵ�
			PwrOnConn = 1;
			//��������
			g_flag = GPRSNEEDRECONN;
		}
		else
		{
			if(g_GprsState >= S_WAITREGACK)
			{//���������ر�
				g_GprsState = S_READYCONNECT;
				ConnTryCnt++;
				//�ӳ�
				TimDelay = 5000;
			}
			if(ConnTryCnt >= 3)
			{
				ConnTryCnt = 0;
				g_GprsState = S_READYCONNECT;
			}
		}
	}
	else
	{
		FailTryCnt = 0;
		if(NeedOffline == 1)
		{//��������
			NeedOffline = 0;
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
	}
	
	if(g_flag & GPRSNEEDRECONN)
	{//��������
		g_flag &= ~GPRSNEEDRECONN;
		TimDelay = 0;
		S282_Log("ReConnected");
		GprsDisConnect();
		g_GprsState = S_READYCONNECT;
		ConnTryCnt = 0;
	}
	if(g_GprsState == S_FREE)
	{
		g_GprsState = S_READYCONNECT;
		TimDelay = 500;
	}
	
	switch(g_GprsState)
	{
		case S_FREE:
			break;
		case S_READYCONNECT:
			if(UpdateTime == 1)
			{//�����ȡʱ��
				GprsGetServerTime();
			}
			if(!ISASCCHAR(SysInfo.GprsServerIP[0]))
			{//��������ַ������
				FailTryCnt++;
				break;
			}
			else	
			{
				if(SysInfo.NetLinkPriority != ALL_NETWORK && (SysInfo.AliyunSet & 0x10))
				{
					Port = 1883;
					GprsIp = SysInfo.HWServer;
					if(!ISASCCHAR(SysInfo.HWServer[0]))
					{//��������ַ������
						FailTryCnt++;
						break;
					}
				}
				else
				{
					Port = SysInfo.GprsServerPort;
					GprsIp = SysInfo.GprsServerIP;
				}
			}
			//���ӷ������ɹ�
			if(GprsConnectServer(GprsIp,Port) == 0)
			{
				S282_Log("GPRS Conn ok");
				mqttConSta = 0;
				SendTryCnt = 0;
				FailTryCnt = 0;
				if(SysInfo.GprsServerAckReg[SYS_TXT_DATLEN] == 0 && SysInfo.GprsServerAckHeart[SYS_TXT_DATLEN] == 0)
				{//ע����Ӧ�� ע����������Ϊ0
					ConnTryCnt = 0;
				}
				if((SysInfo.GprsRegMode == 0 || SysInfo.GprsRegMode == 2) &&
					SysInfo.GprsRegDat[SYS_TXT_DATLEN] > 0)
				{//ע�����Ϊ�� ��ע�����Ϊÿ������ ��ͬʱ
					g_GprsState = S_REGGING;
				}
				else
				{//����Ҫע��
					HeartBeatStamp = GetSecCnt();
					g_GprsState = S_REGGING;
					SendDataStamp = GetSecCnt();
				}
				
				if(SysInfo.NetLinkPriority == CELLULAR_NETWORK)
				{//ʹ�÷���������
					NetSelected = 1;
				}
			}
			else
			{
				ConnTryCnt++;
				FailTryCnt++;
				if(ConnTryCnt == 3)
				{//��������ʧ�� ����GSM
					RestartGSM();	
				}
				GprsDisConRemote();
				TimDelay = ConnTryCnt * 10;
				if(SysInfo.GprsMode & 0x02)
				{
					MqttOfflineToSaveRegDat();
				}
			}
			break;
		case S_REGGING:
			if(SysInfo.GprsMode & 0x01)
			{	//ע�ᵽ������
				SendGprsDat(SysInfo.GprsRegDat,SysInfo.GprsRegDat[SYS_TXT_DATLEN]);			
				SendDataStamp = GetSecCnt();
				if(SysInfo.GprsServerAckReg[SYS_TXT_DATLEN] > 0)
				{
					g_GprsState = S_WAITREGACK;
					
					if(SysInfo.GprsRegMode >= 1)
					{
						g_GprsState = S_WAITHISTACK;
					}
				}
				else
				{//����ע��
					g_GprsState = S_REGOK;
					GprsFreeStamp = GetSecCnt();
				}
				
				HeartBeatStamp = GetSecCnt();
			}
			else if(SysInfo.GprsMode == 0x02)
			{
				if(MqttPro(mqttConSta,0))
				{
					GprsDisConnect();
					mqttConSta = 0;
					S282_Log("GPRS DisCon");
					g_GprsState = S_FREE;
					MqttOfflineToSaveRegDat();
				}
				else
				{
					mqttConSta = 1;
					//MqttOfflineToSaveRegDat();
				}
				return;
			}
			break;
		case S_WAITREGACK:
			//�ȴ�ע����Ӧ
			if(ABSVALUE(GetSecCnt(),SendDataStamp) > 5)
			{//5s����Ӧ
				SendTryCnt++;
				if(SendTryCnt >= SysInfo.GprsRetryCnt)
				{//������Ӧ�ط�����
					GprsDisConnect();
					g_GprsState = S_READYCONNECT;
					ConnTryCnt++;
					TimDelay = 3000;
				}
				else
				{//����ע��
					g_GprsState = S_REGGING;
				}
			}
			break;
		case S_REGOK:
			if(SysInfo.GprsHeartCont[SYS_TXT_DATLEN] > 0 && 
				(ABSVALUE(GetSecCnt(),HeartBeatStamp) > SysInfo.GprsBeatHeart))
			{
				HeartBeatStamp = GetSecCnt();
			  //����������Ӧ��
				SendGprsDat(SysInfo.GprsHeartCont,SysInfo.GprsHeartCont[SYS_TXT_DATLEN]);
				SendDataStamp = GetSecCnt();
				if(SysInfo.GprsServerAckHeart[SYS_TXT_DATLEN] > 0)
				{//��ҪӦ��
					g_GprsState = S_WAITHBACK;
				}
			}
			break;
		case S_WAITHBACK:
		case S_WAITHISTACK:
			if(ABSVALUE(GetSecCnt(),SendDataStamp) > 5)
			{//5��δ��Ӧ
				SendTryCnt++;
				if(SendTryCnt >= SysInfo.GprsRetryCnt)
				{//������Ӧ�ط�����
					GprsDisConnect();
					g_GprsState = S_READYCONNECT;
					ConnTryCnt++;
					TimDelay = 3000;
				}
				else
				{
					if(g_GprsState == S_WAITHISTACK)
					{
						SendGprsWithReg();
					}
					else
					{
						g_GprsState = S_REGOK;
						GprsFreeStamp = GetSecCnt();
					}
				}
			}
			break;
		default:
			break;
	}
	WatchDogFeed();
	if(g_GprsState >= S_REGOK)
	{
		if(ABSVALUE(GetSecCnt(),GprsFreeStamp) >= SysInfo.GprsFreeOffline)
		{//�������߿��а�
			GprsFreeStamp = GetSecCnt();
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
	}
	
	if(g_flag & GPRSPACKOK)
	{//���ݰ�����
		g_flag &= ~GPRSPACKOK;
		GprsFreeStamp = GetSecCnt();
		S282_LogInt("RecGprsDat",gprs_data_Frame.recLen);
		if(SysInfo.GprsServerKickOff[SYS_TXT_DATLEN] > 0
			&& (CompGprsData(SysInfo.GprsServerKickOff) == 0))
		{//���߰�  һ��
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
		//�ȴ�ע����Ӧ
		if(g_GprsState == S_WAITREGACK)
		{
			if(CompGprsData(SysInfo.GprsServerAckReg) == 0)
			{//ע��ɹ�
				if(SysInfo.GprsServerAckHeart[SYS_TXT_DATLEN] == 0)
				{
					ConnTryCnt = 0;
				}
				SendTryCnt = 0;
				g_GprsState = S_REGOK;
				GprsFreeStamp = GetSecCnt();
				HeartBeatStamp = GetSecCnt();
			}
		}
		else if(g_GprsState >= S_REGOK)
		{
			if(g_GprsState >= S_WAITHBACK && CompGprsData(SysInfo.GprsServerAckHeart))
			{//��������Ӧ ����g_GprsState״̬
				ConnTryCnt = 0;
				SendTryCnt = 0;
				g_GprsState = S_REGOK;
				GprsFreeStamp = GetSecCnt();
			}
			else if((SysInfo.GprsMode & 0x0F) == CT_MODBUS)
			{//modbus����
				AnalyModBus(gprs_data_Frame.recBuf,gprs_data_Frame.recLen,1,mReplyBuf);
			}
		}
	}
}

/*
	SMS��������� ��Ҫ�Ƕ��� 
	���ϵȴ����ݵ���
*/
void SmsFun(void)
{
	static u8 IsBusy = 0;
	static u8 SmsSeq = 0;
	static u8 Sms_State = S_SMS_FREE;
	static u16 delay = 0;			//����һֱɨ�� ��������ͨ��ʧЧ
	
	if(IsBusy == 0x11)
	{//��ֹ����
		return;
	}
	IsBusy = 0x11;
	
	if(g_Smsflag & NEWSMSCOME)
	{//��������
		g_Smsflag &= ~NEWSMSCOME;
		//50ms���ȡ����
		delay = 0;
	}
	if(delay)
	{
		delay--;
		IsBusy = 0;
		return;
	}
	switch(Sms_State)
	{
		case S_SMS_FREE:
			g_Smsflag &= ~(NEWSMSCOME | RECSMSCAP);
			g_SmsTempCnt = 0;
			//��ȡ�ź�����
			GSMGetCsq();
			//��ȡ��������
			GSMGetMsgCap();
			Sms_State = S_SMS_READCAP;
		break;
		case S_SMS_READCAP:
				if(g_Smsflag & RECSMSCAP)
				{//������������
					g_Smsflag &= ~RECSMSCAP;
					if(g_SmsTempCnt > 0)
					{
						SmsSeq = 0;
						delay = 100;
						Sms_State = S_SMS_READ;
					}
					else
					{
						delay = 15000;
						Sms_State = S_SMS_FREE;
					}
				}
//				else
//				{
//						delay = 15000;
//						Sms_State = S_SMS_FREE;
//				}
		break;
		case S_SMS_READ:
			//����������
			SmsSeq++;
			if(g_SmsTempCnt == 0 || SmsSeq > g_SmsCnt)
			{
				Sms_State = S_SMS_FREE;
				delay = 1000;
			}
			else
			{
				GSMReadSms(SmsSeq,0);
				Sms_State = S_SMS_READWAIT;
			}
		break;
		case S_SMS_READWAIT:
			if(g_SmsTempCnt > 0)
			{
				if(g_flag & RECOK)
				{
					if(g_Smsflag & RECSMSOK)
					{//��ǰλ�ö����˶���
						g_SmsTempCnt--;
						Sms_State = S_SMS_DEL;
					}
					else
					{//��ǰλ��û�ж�����
						g_flag &= ~RECOK;
						Sms_State = S_SMS_READ;
					}
				}
				delay = 100;
			}
		break;
		case S_SMS_DEL:
			GSMDelSms(SmsSeq);
			Sms_State = S_SMS_ANA;
		break;
		case S_SMS_ANA:
			GSMAnalySms();
			delay = 1000;
			if(g_SmsTempCnt > 0)
			{//����һ��
				Sms_State = S_SMS_READ;
			}
			else
			{
				Sms_State = S_SMS_FREE;
//				ReadSysInfo();
			}
		break;
		default:
			Sms_State = S_SMS_FREE;
			break;
	}
	WatchDogFeed();
	IsBusy = 0;
}
/*
	����NTP������ ��ȡ����ʱ��
*/
void GprsGetServerTime(void)
{
	u8 *GprsIp = malloc(30);
	sprintf((char*)GprsIp,"time.nist.gov");
	for(u8 j = 0; j < 3; j++)
	{
		if(GprsConnectServer(GprsIp,13) == 0)
		{
			u8 i = 0;
			do
			{
				i++;
				delayMs(10);
				if(g_flag & GPRSPACKOK)
				{//���ݰ�����
					g_flag &= ~GPRSPACKOK;
					S282_Log(gprs_data_Frame.recBuf);
					SetUtcTime(gprs_data_Frame.recBuf);
					break;
				}
			}while(i < 10);
			GprsDisConnect();
			break;
		}
	}
	UpdateTime = 0;
	free(GprsIp);
}
