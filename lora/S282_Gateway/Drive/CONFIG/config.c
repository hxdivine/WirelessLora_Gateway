#include "systick.h"
#include "stdio.h"
#include "string.h"

#include "dealDat.h"
#include "config.h"
#include "spi_flash.h"
#include "iwdg.h"
#include "pub.h"
#include "hardware.h"

u16 historyCnt = 0xffff;														//历史记录数量
u16 historyRepCnt = 0xffff;												//历史数据上报记录数量
u16 alarmCnt = 0xffff;															//报警记录数量
u16 historyPtr = 0;
u16 historyRepPtr = 0;							
u16 alarmPtr = 0;													

/*
	清空设备参数
*/
void ClearSysInfo(void)
{
	memset(SysInfo.DeviceName,0,MAXDEVICENAME);

	memset(SysInfo.GprsApn,0,MAXGPRSAPN);
	memset(SysInfo.GprsUsr,0,MAXGPRSUSR);
	memset(SysInfo.GprsPwd,0,MAXGPRSUSR);
	memset(SysInfo.GprsServerIP,0,MAXGPRSIP);

	for(u8 i = 0; i < 4; i++)
	{
		SysInfo.GateIp[i] = 0;
		SysInfo.SubNetMask[i] = 0;
		SysInfo.DnsIp1[i] = 0;
		SysInfo.DnsIp2[i] = 0;
	}
	memset(SysInfo.GprsRegDat,0,MAXREGDAT);
	memset(SysInfo.GprsServerAckReg,0,MAXREGDAT);
	memset(SysInfo.GprsServerKickOff,0,MAXREGDAT);
	memset(SysInfo.GprsServerAckHeart,0,MAXREGDAT);
	
	memset(SysInfo.GprsHeartCont,0,MAXREGDAT);
	memset(SysInfo.EthTarIP,0,MAXGPRSIP);
	
	memset(SysInfo.EthRegDat,0,MAXREGDAT);
	memset(SysInfo.EthServerAckReg,0,MAXREGDAT);
	memset(SysInfo.EthServerKickOff,0,MAXREGDAT);
	memset(SysInfo.EthServerAckHeart,0,MAXREGDAT);
	memset(SysInfo.EthHeartCont,0,MAXREGDAT);
	
	memset(SysInfo.MqttSubTopic,0,MAXREGDAT);
	memset(SysInfo.MqttDeviceID,0,MQTT_PWD);
	memset(SysInfo.MqttPubTopic,0,MAXREGDAT);
	memset(SysInfo.MqttUserName,0,MQTT_PWD);
	memset(SysInfo.MqttPwd,0,MQTT_PWD);
	
	memset(SysInfo.DeviceName_Could,0,HW_DEVSECRET_LEN);
	memset(SysInfo.DeviceSecret,0,MAXGPRSUSR);
	memset(SysInfo.ProductKey,0,HW_DEVSECRET_LEN);
	
	memset(SysInfo.HWDevId,0,MAXGPRSIP);
	memset(SysInfo.HWDevSecret,0,HW_DEVSECRET_LEN);
	memset(SysInfo.HWSerId,0,MQTT_PWDLEN_MAX);
	memset(SysInfo.HWServer,0,MAXGPRSIP);
}
/*
	设置用户参数初始化
*/
void SetUserInfoDef(void)
{
	UserInfo.isChinese = 1;
	UserInfo.SmsTelNoIn = 0;
	UserInfo.SmsTelNoLen = 0;
	UserInfo.SmsTelNo[0] = 0;
	UserInfo.isReboot = 0x11;
	UserInfo.isWriteFlag = 0x11;
	UserInfo.year = 21;
	UserInfo.month = 3;
	UserInfo.day = 23;
	memcpy(UserInfo.version,"BL280 V1.1",10);
	SaveUserInfo();
}

void DevInfoParamInit(u8 Index)
{
	DevInfo[Index].deviceId = 0;
	DevInfo[Index].isExist = 0;
	DevInfo[Index].Type = DEV_WT107;
	memset(DevInfo[Index].devName,0,sizeof(DESCRIBE_DEVICE));
	for(u8 j = 0; j < 6; j++)
	{
		DevInfo[Index].uniId[j] = 0xFF;
		DevInfo[Index].LowLim[j] = 0;
		DevInfo[Index].HighLim[j] = 0;
		memset(DevInfo[Index].name[j],0,sizeof(DESCRIBE_NAME_LEN));
	}
	memset(DevInfo[Index].name[6],0,sizeof(DESCRIBE_NAME_LEN));
	DevInfo[Index].LowLim[6] = 0;
	DevInfo[Index].HighLim[6] = 0;
}

/*
	设置终端设备信息初始化
*/
void SetDevInfoDef(void)
{
	//设置终端设备
	for(u8 i = 0; i < DEVICE_MAX; i++)
	{
		DevInfoParamInit(i);
	}
		SaveDevWT2Config(0);
	SaveDevWT2Config(40);
}
/*
	设置设备默认参数
*/
void SetSysInfoDef(void)
{
	//ClearHistory(1);
	SetUserInfoDef();
	SetDevInfoDef();
	ClearSysInfo();
	for(u8 i = 0; i < 4; i++)
	{//1234
		SysInfo.PassCode[i] = '1' + i;
	}
	SysInfo.DeviceName[0] = 0;
	SysInfo.DeviceId = 1;
	SysInfo.AliyunSet = 0;
	
	SysInfo.GprsMode = 0;
	memcpy(SysInfo.GprsServerIP,"modbus.dtuip.com",20);
	SysInfo.GprsServerPort = 6651;
	
	SysInfo.GprsBeatHeart = 60;
	SysInfo.GprsFreeOffline = 120;
	SysInfo.GprsRetryCnt = 3;

	sprintf((char *)SysInfo.GprsHeartCont,"%s","ACK");
	SysInfo.GprsHeartCont[SYS_TXT_DATLEN] = 3;
	SysInfo.GprsRegMode = 2;
	
	SysInfo.Rj45Mode = 0;
	SysInfo.Dhcp = 0;

	SysInfo.LocalIp[0] = (u8)(LOCAL_IPDEF >> 24);
	SysInfo.LocalIp[1] = (u8)(LOCAL_IPDEF >> 16);
	SysInfo.LocalIp[2] = (u8)(LOCAL_IPDEF >> 8);
	SysInfo.LocalIp[3] = (u8)LOCAL_IPDEF;
	
	SysInfo.LocalPort = LOCAL_PORT;
	
	SysInfo.GateIp[0] = (u8)(DEFAULT_GW >> 24);
	SysInfo.GateIp[1] = (u8)(DEFAULT_GW >> 16);
	SysInfo.GateIp[2] = (u8)(DEFAULT_GW >> 8);
	SysInfo.GateIp[3] = (u8)DEFAULT_GW;
	SysInfo.SubNetMask[0] = (u8)(DEFAULT_MASK >> 24);
	SysInfo.SubNetMask[1] = (u8)(DEFAULT_MASK >> 16);
	SysInfo.SubNetMask[2] = (u8)(DEFAULT_MASK >> 8);
	SysInfo.SubNetMask[3] = (u8)DEFAULT_MASK;
	SysInfo.DnsIp1[0] = (u8)(DEFAULT_DNS >> 24);
	SysInfo.DnsIp1[1] = (u8)(DEFAULT_DNS >> 16);
	SysInfo.DnsIp1[2] = (u8)(DEFAULT_DNS >> 8);
	SysInfo.DnsIp1[3] = (u8)DEFAULT_DNS;
	
	for(u8 i = 0; i < 4; i++)
	{
		SysInfo.DnsIp2[i] = 0;
	}
	memcpy(SysInfo.EthTarIP,"modbus.dtuip.com",20);
	SysInfo.EthTarTcpPort = 6651;
	
	SysInfo.EthBeatHeart = 60;
	SysInfo.EthFreeOffline = 120;
	SysInfo.EthRetryCnt = 3;
	
	sprintf((char *)SysInfo.EthHeartCont,"%s","ACK");
	SysInfo.EthHeartCont[SYS_TXT_DATLEN] = 3;
	SysInfo.EthDatType = 0x00;
	SysInfo.GprsDatType = 0x00;
	
	SysInfo.EthRegMode = 2;

	SysInfo.MqttRepTim = 60;
	SysInfo.MqttRepData = 0;
	
	SysInfo.Workfreq = 1;
	tempWorkfreq = 1;
	SysInfo.TerminalRepCycle = 3;

	//以太网优先
	SysInfo.NetLinkPriority = ETHER_NETWORK;
	SaveSysInfo();
}
/*
	初始化 终端设备信息
*/
void DevInfoInit(void)
{
	ReadDevWT2Config(0);
	delayMs(1);
	ReadDevWT2Config(40);
	
	for(u8 i = 0; i < DEVICE_MAX; i++)
	{//所有设备设置掉线状态
		DevInfo[i].isExist = 0;
	}
}
/**
	获取历史记录总数
*/
void GetHistoyrCodeLen(void)
{
	u16 i = 0;
	u16 lastSeq = 0;
	for(i = 0; i < MAXALARM; i++)
	{
		W25Q_Read((u8*)&History,ALARM_ADDR(i),4);
		if(History.seq == 0xFFFF)
		{
			alarmCnt = i;
			break;
		}
		if(History.seq - lastSeq > 100)
		{//不连续
			alarmCnt = lastSeq + 1;
		}
		lastSeq = History.seq;
	}
	alarmPtr = alarmCnt;
	if(alarmCnt == 0xFFFF)
	{
		alarmCnt = 0;
		alarmPtr = 0;
	}
	
	lastSeq = 0;
	for(i = 0; i < MAXRECORDNO; i++)
	{
		W25Q_Read((u8*)&History,HISRECORD_ADDR(i),4);
		if(History.seq == 0xFFFF)
		{
			historyCnt = i;
			break;
		}
		if(History.seq - lastSeq > 100)
		{
			historyCnt = lastSeq + 1;
		}
		lastSeq = History.seq;
	}
	historyPtr = historyCnt;
	if(historyCnt == 0xFFFF)
	{
		historyCnt = 0;
		historyPtr = 0;
	}
	
	
	lastSeq = 0;
	for(i = 0; i < MAXRECORDNO; i++)
	{
		W25Q_Read((u8*)&History,HISTORY_REPDATCORD_ADDR(i),4);
		if(History.seq == 0xFFFF)
		{
			historyRepCnt = i;
			break;
		}
		if(History.seq - lastSeq > 100)
		{
			historyRepCnt = lastSeq + 1;
		}
		lastSeq = History.seq;
	}
	historyRepPtr = historyRepCnt;
	if(historyRepCnt == 0xFFFF)
	{
		historyRepCnt = 0;
		historyRepPtr = 0;
	}
}
/*
	数据初始化
*/
void SysInfoInit(void)
{
	DevInfoInit();
	ReadUserInfo();
	GetHistoyrCodeLen();
	__nop();
	ReadSysInfo();
	if(UserInfo.isWriteFlag != 0x11)
	{
		EraseChipOrSector(NULL,0);
		SetSysInfoDef();
		
		historyCnt = 0;
		historyRepCnt = 0;
		alarmCnt = 0;
		
		historyPtr = 0;
		historyRepPtr = 0;							
		alarmPtr = 0;
	}
}
/*
	读取设备信息
*/
void ReadSysInfo(void)
{
	ReadConfig(SYSINFO_MODE,(u8 *)&SysInfo,0);
}
/*
	保存设备配置信息
*/
void SaveSysInfo(void)
{
	WriteConfig(SYSINFO_MODE,(u8 *)&SysInfo,0);
	delayMs(2);
	ReadConfig(SYSINFO_MODE,(u8 *)&SysInfo,0);
}
/*
	读取用户配置信息
*/
void ReadUserInfo(void)
{
	ReadConfig(USERINFO_MODE,(u8 *)&UserInfo,0);
}
/*
	保存用户配置信息
*/
void SaveUserInfo(void)
{
	WriteConfig(USERINFO_MODE,(u8 *)&UserInfo,0);
	
	ReadConfig(USERINFO_MODE,(u8 *)&UserInfo,0);
}
/*
	删除终端设备
*/
void DelDevWT2Config(u16 ptr)
{
	//设备设置不存在
	memset(&DevInfo[ptr],0,sizeof(TERMINALINFO_WT2));
	DevInfo[ptr].isExist = 0;
	DevInfo[ptr].deviceId = 0;
	DevInfo[ptr].uniId[0] = 0xFF;
	DevInfo[ptr].Type = DEV_WT107;
	if(ptr > 24)
	{
		WriteConfig(TERMINALWT2_MODE,(u8*)&DevInfo[25],1);
	}
	else
	{
		WriteConfig(TERMINALWT2_MODE,(u8*)&DevInfo,0);
	}
}

/*
	保存终端设备信息
*/
void SaveDevWT2Config(u16 ptr)
{
	if(ptr > 24)
	{
		WriteConfig(TERMINALWT2_MODE,(u8*)&DevInfo[25],1);
		delayMs(1);
		ReadConfig(TERMINALWT2_MODE,(u8*)&DevInfo[25],1);
	}
	else
	{
		WriteConfig(TERMINALWT2_MODE,(u8*)&DevInfo,0);
		delayMs(1);
		ReadConfig(TERMINALWT2_MODE,(u8*)&DevInfo,0);
	}
}
/*
	读取终端设备信息
*/
u8 ReadDevWT2Config(u8 Index)
{
	if(Index > 24)
	{
		ReadConfig(TERMINALWT2_MODE,(u8 *)&DevInfo[25],1);
	}
	else
	{
		ReadConfig(TERMINALWT2_MODE,(u8 *)&DevInfo,0);
	}
	return 1;
}
/*
	清除历史记录
*/
void ClearHistory(u8 mode)
{
	//每页存储128条 1条32byte MAXRECORDNO  / 128
	u8 recordMax = MAXRECORDNO / 128;
	if(mode)
	{
		if(historyCnt != 0)
		{
			for(u8 i = 0; i < recordMax; i++)
			{	//清除扇区
				EraseChipOrSector(HISRECORDBASIC + 0x1000 * i,1);
			}
		}
		historyCnt = 0;
		historyPtr = 0;
	}
	
	if(alarmCnt == 0)
	{
		return;
	}
	recordMax = MAXALARM/128;
	for(u8 i = 0; i < recordMax; i++)
	{
		//清楚扇区
		EraseChipOrSector(ALARM_BASIC + 0x1000 * i,1);
	}
	alarmCnt = 0;
	alarmPtr = 0;
	SaveUserInfo();
}
/*
	保存历史记录
*/
void SaveHistory(u8 Index,u8 type,u8 mode)
{
	if(mode)
	{	
		WriteHistory(HISTORYCORD_MODE,Index,type,historyPtr);
		historyPtr += 1;
		historyCnt += 1;
		if(historyCnt > MAXRECORDNO)
		{
			historyCnt = MAXRECORDNO;
		}
		if(historyPtr >= MAXRECORDNO)
		{
			historyCnt = MAXRECORDNO;
			historyPtr = 0;
		}
	}
	else
	{
		WriteHistory(ALARM_MODE,Index,type,alarmPtr);
		alarmPtr += 1;
		alarmCnt += 1;
		if(alarmCnt > MAXALARM)
		{
			alarmCnt = MAXALARM;
		}
		if(alarmPtr >= MAXALARM)
		{
			alarmCnt = MAXALARM;
			alarmPtr = 0;
		}
	}
}
/*
	读取历史记录
*/
void ReadHistroy(u8 *InBuf,u16 ptr,u8 mode)
{
	if(mode)
	{
		ReadHistory(HISTORYCORD_MODE,InBuf,ptr);
	}
	else
	{
		ReadHistory(ALARM_MODE,InBuf,ptr);
	}
}
/*
	读取上报记录
*/
void ReadHistroyRep(u8 *InBuy,u16 ptr)
{
	ReadHistory(HISTORY_REPORTCORD_MODE,InBuy,ptr);
}
/*
	写入上报记录
*/
void SaveHistoryRep(u8 Index,u8 type,u8 mode)
{
	if(mode == 0)
	{
		historyRepPtr = historyRepCnt;
		WriteHistory(HISTORY_REPORTCORD_MODE,Index,type,historyRepPtr);
	}
	else
	{	
		WriteHistory(HISTORY_REPORTCORD_MODE,Index,type,historyRepPtr);
		historyRepPtr++;
		historyRepCnt++;
		if(historyRepCnt > MAXRECORDNO)
		{
			historyRepCnt = MAXRECORDNO;
		}
		if(historyRepPtr >= MAXRECORDNO)
		{
			historyRepCnt = MAXRECORDNO;
			historyRepPtr = 0;
		}
	}
}

/*
	历史参数参数
*/
void SetHistory(HISTORY *his,u8 Index,u8 type,u16 ptr)
{
	his->seq = ptr;
	his->year = calendar.w_year - 2000;
	his->mon = calendar.w_month;
	his->day = calendar.w_day;
	his->hour = calendar.hour;
	his->min = calendar.min;
	his->sec = calendar.sec;
		
	his->BatVal = LocalBatVal;
	his->GsmVal = g_SigLevel;
	his->Temp = DevRegVal[Index * 7];
	his->Humi = DevRegVal[Index * 7 + 1];
	his->Illu = DevRegVal[Index * 7 + 2];
	his->Co2 = DevRegVal[Index  * 7 + 3];
	his->Tvoc = DevRegVal[Index * 7 + 4];
	his->SoilHumi = DevRegVal[Index * 7 + 5];
	his->SoilTemp = DevRegVal[Index * 7 + 6];

	his->DeviceId = Index + 1;
	his->DevType = type;
}
