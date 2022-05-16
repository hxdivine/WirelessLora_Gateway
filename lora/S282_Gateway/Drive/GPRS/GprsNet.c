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
//心跳时间
u16 HeartBeatStamp;
//发送数据时间
u16 SendDataStamp;
//空闲下线时间
u16 GprsFreeStamp;

/*
	比较接收到的GPRS数据是否一致 
	用于注册包 响应包检测
	一致返回 0
*/
u8 CompGprsData(u8 *str)
{
	u8 len = str[SYS_TXT_DATLEN];
	
	if(gprs_data_Frame.recLen < len)
	{//接收长度不一致
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
	循环在主函数中  等待GPRS数据
*/
void GprsRunning(void)
{
	static u16 RegTime = 0;			//在GSM可用下 检测是否需要重启
	static u32 GsmNoreg = 0;		//检测4G模块是否可用 不可用则重启
	static u8 failTryCnt = 0;
	if(g_flag & POWERDOWN)
	{//Gsm 电源关闭
		g_flag = 0;
		return;
	}
	GsmNoreg++;
	if(GsmNoreg > 1000 * 360)
	{//GSM 网络状态不存在 重启GSM 未插卡
		RestartGSM();
		GsmNoreg = 1;
	}
	
	if((g_flag & REGOK) || (g_flag & CGATT))
	{
		//开启了GPRS网络
		if(SysInfo.GprsMode & 0x0F)
		{
			if((SysInfo.NetLinkPriority != ETHER_NETWORK) || NetSelected == 1)
			{//NetSelected = 1 以太网不可用 使用GPRS
				GprsFun();
			}
		}
		else if(SysInfo.NetLinkPriority == CELLULAR_NETWORK)
		{
			NetSelected = 2;
		}
		if(NeedSetAPN == 1)
		{//设置APN
			NeedSetAPN = 0;
			GSMSetGprs();
		}

		if(RegTime == 0)
		{
			RegTime = GetSecCnt();
		}
		else if(ABSVALUE(GetSecCnt(),RegTime) > 2)
		{//大于2s
			if(UserInfo.isReboot == 0x3456)
			{//设置了重启
				GetLanguageMode();
				UserInfo.isReboot = 0x11;
				SaveUserInfo();
				//发送短信
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
		{//长时间网络不在线
			//获取网络状态
			GSMGetReg();
			failTryCnt++;
			if(failTryCnt > 15)
			{//尝试10次以上 可能未插卡 使用网口
				if((SysInfo.GprsMode & 0x0F) && SysInfo.NetLinkPriority == CELLULAR_NETWORK)
				{
					NetSelected = 2;
				}
			}
		}
	}
}
/*
	附加心跳包数据

	注册包 + 心跳包
*/
void SendGprsWithReg(void)
{
	u16 i; 	
	u8 *buf = malloc(200);
	u8 RegLen = SysInfo.GprsRegDat[SYS_TXT_DATLEN];
	u8 len = SysInfo.GprsHeartCont[SYS_TXT_DATLEN];
	SendDataStamp = GetSecCnt();
	
	for(i = 0; i < len; i++)
	{  //后移  前段数据为注册包数据 后一段为发送数据
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
	运行GPRS 连接服务器
*/
void GprsFun(void)
{
	//第一次上电 重连设置
	static u8 PwrOnConn = 0;
	//掉线重连次数
	static u8 ConnTryCnt = 0;
	//用于数据发送次数尝试 响应重发次数
	static u8 SendTryCnt = 0;
	//用于判断多次连接服务器失败 采用 以太网连接
	static u8 FailTryCnt = 0;
	//1.用于时间延迟	2.用于历史数据保存时间设置
	static u16 TimDelay = 0;
	//是否掉线状态
	static u8 mqttConSta = 0;
	u16 Port;
	u8 *GprsIp;
	
	if(TimDelay > 0)
	{//需要进行延迟 否则可能造成出错
		TimDelay--;
		return;
	}
	if(FailTryCnt > 2 && SysInfo.NetLinkPriority == CELLULAR_NETWORK)
	{//尝试连接失败 设置以太网连接
		FailTryCnt = 0;
		NetSelected = 2;
		TimDelay = 7000;
		return;
	}
	if(!(g_flag & TCPCONNECTED))
	{//没连接上
		NeedOffline = 0;
		if(PwrOnConn == 0)
		{//第一次上电
			PwrOnConn = 1;
			//重连设置
			g_flag = GPRSNEEDRECONN;
		}
		else
		{
			if(g_GprsState >= S_WAITREGACK)
			{//被服务器关闭
				g_GprsState = S_READYCONNECT;
				ConnTryCnt++;
				//延迟
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
		{//重连设置
			NeedOffline = 0;
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
	}
	
	if(g_flag & GPRSNEEDRECONN)
	{//重连设置
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
			{//网络获取时间
				GprsGetServerTime();
			}
			if(!ISASCCHAR(SysInfo.GprsServerIP[0]))
			{//服务器地址不可用
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
					{//服务器地址不可用
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
			//连接服务器成功
			if(GprsConnectServer(GprsIp,Port) == 0)
			{
				S282_Log("GPRS Conn ok");
				mqttConSta = 0;
				SendTryCnt = 0;
				FailTryCnt = 0;
				if(SysInfo.GprsServerAckReg[SYS_TXT_DATLEN] == 0 && SysInfo.GprsServerAckHeart[SYS_TXT_DATLEN] == 0)
				{//注册响应包 注册心跳包都为0
					ConnTryCnt = 0;
				}
				if((SysInfo.GprsRegMode == 0 || SysInfo.GprsRegMode == 2) &&
					SysInfo.GprsRegDat[SYS_TXT_DATLEN] > 0)
				{//注册包不为空 且注册策略为每次连接 和同时
					g_GprsState = S_REGGING;
				}
				else
				{//不需要注册
					HeartBeatStamp = GetSecCnt();
					g_GprsState = S_REGGING;
					SendDataStamp = GetSecCnt();
				}
				
				if(SysInfo.NetLinkPriority == CELLULAR_NETWORK)
				{//使用蜂窝网连接
					NetSelected = 1;
				}
			}
			else
			{
				ConnTryCnt++;
				FailTryCnt++;
				if(ConnTryCnt == 3)
				{//联网三次失败 重启GSM
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
			{	//注册到服务器
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
				{//无需注册
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
			//等待注册响应
			if(ABSVALUE(GetSecCnt(),SendDataStamp) > 5)
			{//5s无响应
				SendTryCnt++;
				if(SendTryCnt >= SysInfo.GprsRetryCnt)
				{//大于响应重发次数
					GprsDisConnect();
					g_GprsState = S_READYCONNECT;
					ConnTryCnt++;
					TimDelay = 3000;
				}
				else
				{//重新注册
					g_GprsState = S_REGGING;
				}
			}
			break;
		case S_REGOK:
			if(SysInfo.GprsHeartCont[SYS_TXT_DATLEN] > 0 && 
				(ABSVALUE(GetSecCnt(),HeartBeatStamp) > SysInfo.GprsBeatHeart))
			{
				HeartBeatStamp = GetSecCnt();
			  //发送心跳响应包
				SendGprsDat(SysInfo.GprsHeartCont,SysInfo.GprsHeartCont[SYS_TXT_DATLEN]);
				SendDataStamp = GetSecCnt();
				if(SysInfo.GprsServerAckHeart[SYS_TXT_DATLEN] > 0)
				{//需要应答
					g_GprsState = S_WAITHBACK;
				}
			}
			break;
		case S_WAITHBACK:
		case S_WAITHISTACK:
			if(ABSVALUE(GetSecCnt(),SendDataStamp) > 5)
			{//5秒未响应
				SendTryCnt++;
				if(SendTryCnt >= SysInfo.GprsRetryCnt)
				{//大于响应重发次数
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
		{//大于下线空闲包
			GprsFreeStamp = GetSecCnt();
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
	}
	
	if(g_flag & GPRSPACKOK)
	{//数据包到来
		g_flag &= ~GPRSPACKOK;
		GprsFreeStamp = GetSecCnt();
		S282_LogInt("RecGprsDat",gprs_data_Frame.recLen);
		if(SysInfo.GprsServerKickOff[SYS_TXT_DATLEN] > 0
			&& (CompGprsData(SysInfo.GprsServerKickOff) == 0))
		{//下线包  一致
			GprsDisConnect();
			g_GprsState = S_FREE;
		}
		//等待注册响应
		if(g_GprsState == S_WAITREGACK)
		{
			if(CompGprsData(SysInfo.GprsServerAckReg) == 0)
			{//注册成功
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
			{//心跳有响应 更改g_GprsState状态
				ConnTryCnt = 0;
				SendTryCnt = 0;
				g_GprsState = S_REGOK;
				GprsFreeStamp = GetSecCnt();
			}
			else if((SysInfo.GprsMode & 0x0F) == CT_MODBUS)
			{//modbus分析
				AnalyModBus(gprs_data_Frame.recBuf,gprs_data_Frame.recLen,1,mReplyBuf);
			}
		}
	}
}

/*
	SMS卡相关数据 主要是短信 
	不断等待数据到来
*/
void SmsFun(void)
{
	static u8 IsBusy = 0;
	static u8 SmsSeq = 0;
	static u8 Sms_State = S_SMS_FREE;
	static u16 delay = 0;			//避免一直扫描 导致网络通信失效
	
	if(IsBusy == 0x11)
	{//防止重入
		return;
	}
	IsBusy = 0x11;
	
	if(g_Smsflag & NEWSMSCOME)
	{//短信来了
		g_Smsflag &= ~NEWSMSCOME;
		//50ms后读取短信
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
			//获取信号质量
			GSMGetCsq();
			//获取短信数量
			GSMGetMsgCap();
			Sms_State = S_SMS_READCAP;
		break;
		case S_SMS_READCAP:
				if(g_Smsflag & RECSMSCAP)
				{//读到短信数量
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
			//读单条短信
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
					{//当前位置读到了短信
						g_SmsTempCnt--;
						Sms_State = S_SMS_DEL;
					}
					else
					{//当前位置没有短信了
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
			{//读下一条
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
	连接NTP服务器 获取网络时间
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
				{//数据包到来
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
