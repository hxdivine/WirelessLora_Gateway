#include "systick.h"
#include "stdio.h"
#include "stdlib.h"

#include "EthNet.h"
#include "enc28j60.h"

#include "tapdev.h"
#include "dhcpc.h"
#include "uip_arp.h"
#include "resolv.h"
#include "uip_appcall.h"

#include "modbus.h"
#include "GprsNet.h"
#include "mqtt.h"
#include "netLink.h"
#include "dealDat.h"
#include "iwdg.h"
#include "config.h"
#include "hardware.h"

//网络状态标志
u8 g_EthState = S_REGGING;
//空闲重连次数
u16 g_EthFreeStamp;
//空闲重连 发送时间
u16 g_SendDataTemp = 0;
//无响应重连次数
u8 SendTryCnt = 0;
//用于记录以太网口是否断开
u8 Rj45Err = 0;
/*
	字符转16进制
*/
u8 str2hex(u16 *dest, u8 *src)
{
	u8 i;
	u16 dat=0;
	
	for(i = 0; i < 10; i++)
	{
		if(src[i] >= 0x30 && src[i] <= 0x39)
		{
			dat = dat * 10 + src[i] - '0';
		}
		else 
		{
			break;
		}
	}
	*dest = dat;
	return i;
}
/*
	字符转IP
*/
u8 str2ip(u8 *dest, u8 *src)
{
	u8 i;
	u8 k = 0;
	u16 dat = 0;
	
	for(i = 0; i < 4; i++)
	{
		//返回长度	
		k += str2hex(&dat, &src[k]);
		if( i == 3 && src[k] == 0)
		{
			dest[i] = dat;
			return 1;
		}
		if(src[k] != '.' || dat > 255)
		{
			break;
		}
		k++;
		dest[i] = dat;
	}
	return 0;
}


/*
	检测句柄连接
*/
void CheckConn(void)
{
	u8 c = 0; 
	for(c = 0; c < UIP_CONNS; c++)
	{
		if(uip_conns[c].tcpstateflags == UIP_CLOSED ) 
		{
			break;
		}
	}
	if(c >= UIP_CONNS)
	{
			InitNet();
	}
}

/*
	网口检测
*/
void Enc28j60Check(void)
{
	if(ENC28J60_Read(ESTAT) & 0x40)
	{
		S282_Log("phy estat: 0x40 error");
		
		tapdev_init();		
		Rj45Err++;
	}
	else
	{
		Rj45Err = 0;
	}
}

/*
	本地IP设置
*/
void InitLocalIP(u8 mode)
{
	uip_ipaddr_t ipaddr;
	//设置本地设置IP地址
	uip_ipaddr(ipaddr,SysInfo.LocalIp[0],SysInfo.LocalIp[1],SysInfo.LocalIp[2],SysInfo.LocalIp[3]);
	uip_sethostaddr(ipaddr);	
	if(mode == 1)
	{
		return;
	}		
	//设置网关IP地址(其实就是你路由器的IP地址)
	uip_ipaddr(ipaddr,SysInfo.GateIp[0],SysInfo.GateIp[1],SysInfo.GateIp[2],SysInfo.GateIp[3]);	
	uip_setdraddr(ipaddr);			

	//设置网络掩码
	uip_ipaddr(ipaddr,SysInfo.SubNetMask[0],SysInfo.SubNetMask[1],SysInfo.SubNetMask[2],SysInfo.SubNetMask[3]);
	uip_setnetmask(ipaddr);

	for(u8 i = 0; i < 2; i++)
	{
		g_mconn[i].tcp = 0;
		g_mconn[i].udp = 0;
		g_mconn[i].dnsno = 0;
		g_mconn[i].ip[0] = 0;
		g_mconn[i].ip[1] = 0;
		g_mconn[i].port = 0;
		
		g_mconn[i].state = TcpClient_FREE;	
		g_mconn[i].sbuf = tcp_client_sendbuf;
	}
	
	g_uiLintenPort = SysInfo.LocalPort;
	uip_listen(HTONS(g_uiLintenPort));
}
/*
	网络初始化
*/
void InitNet(void)
{
	//Mac初始化
	tapdev_init();
	//uip协议初始化
	uip_init();		
		//ARP初始化
	uip_arp_init();
	//解析数据格式初始化
	resolv_init();
	//本机IP设置
	InitLocalIP(0);
}
/*
	附加心跳包数据

	注册包 + 心跳包
*/
void SendEthWithReg(void)
{
	u16 i; 	
	u8 *buf = malloc(200);
	u8 RegLen = SysInfo.EthRegDat[SYS_TXT_DATLEN];
	u8 len = SysInfo.EthHeartCont[SYS_TXT_DATLEN];
	g_SendDataTemp = GetSecCnt();
	
	for(i = 0; i < len; i++)
	{  //后移  前段数据为注册包数据 后一段为发送数据
		buf[RegLen + i] = SysInfo.EthHeartCont[i];
	}	
	if(SysInfo.EthRegMode >= 1 && RegLen > 0)
	{	
		for(i = 0; i < RegLen; i++)
		{
			buf[i]= SysInfo.EthRegDat[i];
		} 	
		UipClient2Server(buf,RegLen + len);
	}
	else 
	{
	  UipClient2Server(&buf[RegLen],len);
	}
	free(buf);
}

/*
	关闭网络连接
*/
void NetConnClose(u8 Index)
{
	if((SysInfo.Rj45Mode & 0x80))
	{					//udp 协议 关闭
		uip_udp_remove(g_mconn[Index].udp);
	}
	else
	{					//TcpClient_Close 协议关闭
		TcpClose(g_mconn[Index].tcp);
	}
	g_mconn[Index].state = TcpClient_FREE;
}

u16 AnalyTarIp(u8 *TarIp,u16 port,u8 i)
{
	static u8 dnsSel = 0;
	u16 stateStamp;
	uip_ipaddr_t ipaddr;
	u8 ip[4];
			
	g_mconn[i].flag = 0;
	stateStamp = GetSecCnt();
			//连接服务器
	g_mconn[i].port = port;		//端口
			//准备连接服务器
	if(IsIPaVailable(TarIp))				//是否为IP
	{
		if(str2ip(ip, TarIp))		//ip字符串转换成数字
		{
			uip_ipaddr(&ipaddr, ip[0], ip[1], ip[2], ip[3]);
			uip_ipaddr_copy(g_mconn[i].ip, ipaddr);		//记录IP
			S282_Log("uip connecting...");
			g_mconn[i].tcp = uip_connect(&g_mconn[i].ip, htons(g_mconn[i].port)); 	//端口					
			g_mconn[i].state = TcpClient_Connectting;
			stateStamp = GetSecCnt();
		}								
	}
	else	//域名
	{				
		u8 HaveDns = 0;
		if(DhcpOk == 0)
		{
			if(dnsSel == 0)
			{		
				if(SysInfo.DnsIp1[1] != 0 && SysInfo.DnsIp1[0] != 0xff)
				{
					uip_ipaddr(ipaddr, SysInfo.DnsIp1[0],SysInfo.DnsIp1[1],SysInfo.DnsIp1[2],SysInfo.DnsIp1[3]);
					HaveDns = 1;
				}
				if(SysInfo.DnsIp2[1] !=0 || SysInfo.DnsIp1[2] !=0 )	
				{
					dnsSel = 1;
				}
			}
			if((HaveDns == 0) && dnsSel)
			{	//使用DNS2
				dnsSel = 0;
				if(SysInfo.DnsIp2[1] != 0 && SysInfo.DnsIp2[0] != 0xff)	
				{		
					uip_ipaddr(ipaddr, SysInfo.DnsIp2[0],SysInfo.DnsIp2[1],SysInfo.DnsIp2[2],SysInfo.DnsIp2[3]);
					HaveDns = 1;
				}
			}
		}
		if(HaveDns || DhcpOk)
		{//进行域名解析
			g_mconn[i].state = TcpClient_DnsRequest;
			g_mconn[i].flag &= ~e_Dns_found;
			S282_Log("Analy Target Server ip");
			if(resolv_getserver() == NULL)
			{
				resolv_conf(ipaddr);		//配置DNS服务器
			}
			g_mconn[i].dnsno = resolv_query((char*)&TarIp[0]);//请求域名解析
			g_mconn[i].dnsno |= 0x80;
			stateStamp = GetSecCnt();
		}						
	}
	return stateStamp;
}
/*
	网络运行 循环检测
*/
void NetRunning(void)
{
	//1.DNS选用设置	2.连接服务器失败次数	3. 多次连接服务器失败  改用蜂窝网连接
	static u8 failCnt=0,failTryCnt = 0;
	//用于响应时间保存设置
	static u32 stateStamp = 0;
	//用于延时设置 
	static u16 TimDelay = 0;
	static u8 mqttConSta = 0;
	u8 i = 0;
	
	if(Rj45Err > 10 && SysInfo.NetLinkPriority == ETHER_NETWORK)
	{//网口没有连接上 设置蜂窝网优先
		NetSelected = 1;
	}
	Enc28j60Check();
	uip_polling();

	if(TimDelay)
	{
		TimDelay--;
		return;
	}
	
	if(failTryCnt > 5 && SysInfo.NetLinkPriority == ETHER_NETWORK)
	{//5次连接失败 在以太网优先情况下使用蜂窝网连接
		failTryCnt = 0;
		NetSelected = 1;
		TimDelay = 7000;
		//CheckConn();
		if(ISASCCHAR(SysInfo.EthTarIP[0]))
		{
			InitNet();
		}
		return;
	}
	
	if(SysInfo.NetLinkPriority == CELLULAR_NETWORK && NetSelected != 2)
	{//此时蜂窝网优先 且连接成功
		return;
	}
	switch(g_mconn[i].state)//10ms一次
	{
		case TcpClient_FREE://没有连接
			CheckConn();
			if(ETHLineOut() == 0)	//没掉线
			{		
				if(SysInfo.Dhcp == 1)
				{//动态设置
					dhcpc_init(uip_ethaddr.addr, 6);
					dhcpc_request();
					DhcpOk = 0;
					stateStamp = GetSecCnt();
					g_mconn[i].state = TcpClient_Dhcp;
				}
				else
				{//S282作为TCPClient 准备连接服务器
					g_mconn[i].state = TcpClient_ReadyConnect;
				}
			}
			break;
		case TcpClient_Dhcp:
			if(ABSVALUE(GetSecCnt(),stateStamp) > 5)
			{//5s未响应
				g_mconn[i].state = TcpClient_FREE;
			}
			if(DhcpOk)
			{
				g_mconn[i].state = TcpClient_ReadyConnect;
				SysInfo.LocalIp[0] = DhcpIp[0];
				SysInfo.LocalIp[1] = DhcpIp[1];
				SysInfo.LocalIp[2] = DhcpIp[2];
				SysInfo.LocalIp[3] = DhcpIp[3];
			}
			break;
		case TcpClient_ReadyConnect://2
		{
			if(UpdateTime == 1)
			{
				stateStamp = AnalyTarIp("time.nist.gov",13,i);
			}
			else
			{
				if(SysInfo.AliyunSet & 0x10)
				{
					stateStamp = AnalyTarIp(SysInfo.HWServer,1883,i);
				}
				else
				{
					stateStamp = AnalyTarIp(SysInfo.EthTarIP,SysInfo.EthTarTcpPort,i);
				}
			}
			break;
		}			
		case TcpClient_DnsRequest:
			if(ABSVALUE(GetSecCnt(),stateStamp) > 10)
			{	//10秒超时
				g_mconn[i].state = TcpClient_FREE;
				stateStamp = GetSecCnt();
				failCnt++;				
				
				if(failCnt >= 3)
				{//连接失败次数大于3 设置连接失败
					if(UpdateTime == 1)
					{
						UpdateTime = 0;
						g_mconn[i].state = TcpClient_FREE;
					}
					else
					{
						g_mconn[i].state = TcpClient_Fail;
						failTryCnt++;
					}
					failCnt = 0;
				}
			}
			if(g_mconn[i].flag & e_Dns_found)	//解析成功
			{
				S282_Log("uip connecting...");
				g_mconn[i].flag &= ~e_Dns_found;				
				failCnt = 0;	
				if((SysInfo.Rj45Mode & 0x80) == 0 || UpdateTime)
				{
					g_mconn[i].tcp = uip_connect(&g_mconn[i].ip, htons(g_mconn[i].port)); 	//端口
					g_mconn[i].state = TcpClient_Connectting;
				}
				else
				{
					if(g_mconn[i].udp != 0)
					{
						uip_udp_remove(g_mconn[i].udp);
					}
					g_mconn[i].udp = uip_udp_new(&g_mconn[i].ip, htons(g_mconn[i].port)); 
					g_mconn[i].state = TcpClient_ConnectOk;					
				}
				
				stateStamp = GetSecCnt();
			}
			break;			
		case TcpClient_Connectting:
		{	
			if(ABSVALUE(GetSecCnt(),stateStamp) > 5)
			{//5秒
				NetConnClose(i);
				stateStamp = GetSecCnt();
				failCnt++;
				
				if(failCnt >= 3)
				{
					failTryCnt++;
					failCnt = 0;
					g_mconn[i].state = TcpClient_Fail;
					g_EthState = S_REGGING;
					
					S282_Log("TcpClient_Fail");
				}
			}
			if(g_mconn[i].flag & e_Tcp_connected)		//连接成功
			{
				g_mconn[i].flag = 0;
				failCnt = 0;
				failTryCnt = 0;
				stateStamp = GetSecCnt();
				mqttConSta = 0;
				g_mconn[i].state = TcpClient_ConnectOk;
				S282_Log("Target IP Connected");
				if(SysInfo.NetLinkPriority == ETHER_NETWORK)
				{
					NetSelected = 2;
				}
			}
			else if(SysInfo.Rj45Mode & 0x02)
			{
				MqttOfflineToSaveRegDat();
			}
			break;
		}
		case TcpClient_ConnectOk:	
			if(ETHLineOut())
			{	//连接超时
				NetConnClose(i);
				g_EthState = S_REGGING;				
				break;
			}	
			if(UpdateTime == 1)
			{
				if(EthGetServerTime() > 10 || UpdateTime == 0)
				{
					UpdateTime = 0;
					S282_Log("NTP Server DIS");
					TcpClose(g_mconn[i].tcp);
					g_mconn[i].state = TcpClient_FREE;
				}
			}
			else
			{
				if(SysInfo.Rj45Mode & 0x01)
				{
					if(EthFun())//正常通信0 异常1
					{
						NetConnClose(i);
					}
				}
				else if(SysInfo.Rj45Mode & 0x02)
				{
					if(MqttPro(mqttConSta,1))
					{
						NetConnClose(i);
						S282_Log("Eth Dis");
						MqttOfflineToSaveRegDat();
						mqttConSta = 0;
					}
					else
					{
						//MqttOfflineToSaveRegDat();
						mqttConSta = 1;
					}
				
					WatchDogFeed();
				}
				
			}
			if((g_mconn[i].flag & e_Tcp_closed) ||
				(g_mconn[i].flag & e_Tcp_aborted) ||
				(g_mconn[i].flag & e_Tcp_timedout)||
				(g_mconn[i].flag & e_PackDisConn))	
			{
				g_mconn[i].flag = 0;
				NetConnClose(i);
				g_EthState = S_REGGING;
				g_mconn[i].state = TcpClient_Fail;
			}
			break;		
		case TcpClient_Fail:			
			g_mconn[i].state = TcpClient_FREE;		
			break;
		default:			
			g_mconn[i].state = TcpClient_FREE;
			break;
	}
}


/*
	比较 以太网接收的数据
	相等 1 不相等 0
*/
u8 CompEthData(u8 *buf)
{
	u8 len;
	len = buf[SYS_TXT_DATLEN];
	if(len > g_RecEthLen)
	{
		return 0;
	}
	for(u8 i = 0; i < len; i++)
	{
		if(tcp_client_Recbuf[i] != buf[0])
		{
			return 0;
		}
	}
	return 1;
}
/*
	ETH 网络功能函数
	TCP连接成功后  会在该函数循环执行

*/
u8 EthFun(void)
{
	//心跳时间
	static u16 HeartBeatTemp = 0;
	//服务器重连此次数
	static u8 ConTryCnt = 0;
	
	//网络连接状态
	switch(g_EthState)
	{
		case S_REGGING:
			//发送注册包
			UipClient2Server(SysInfo.EthRegDat,SysInfo.EthRegDat[SYS_TXT_DATLEN]);
			g_SendDataTemp = GetSecCnt();//10ms
			if(SysInfo.EthServerAckReg[SYS_TXT_DATLEN] > 0)
			{//注册响应包
				g_EthState = S_WAITREGACK;
				if(SysInfo.EthRegMode > 0)
				{
					g_EthState = S_WAITHISTACK;
				}
			}
			else
			{
				g_EthState = S_REGOK;
			}
			
			HeartBeatTemp = GetSecCnt();
			g_EthFreeStamp = GetSecCnt();
			break;
		case S_WAITREGACK:
			if(ABSVALUE(GetSecCnt(),g_SendDataTemp) > 5)
			{//5s无反应
				SendTryCnt++;
				if(SendTryCnt > SysInfo.EthRetryCnt)
				{
					g_EthState = S_REGGING;
					return 1;
				}
			}
			break;
		case S_REGOK: 
			if((SysInfo.EthHeartCont[SYS_TXT_DATLEN] > 0) && ABSVALUE(GetSecCnt(),HeartBeatTemp) > SysInfo.EthBeatHeart)
			{//心跳包不为空   大于心跳周期才能执行
				HeartBeatTemp = GetSecCnt();
				//发送心跳包
				UipClient2Server(SysInfo.EthHeartCont,SysInfo.EthHeartCont[SYS_TXT_DATLEN]);
				g_SendDataTemp = GetSecCnt();
				if(SysInfo.EthServerAckHeart[SYS_TXT_DATLEN] > 0)
				{//注册心跳响应包
					g_EthState = S_WAITHBACK;
				}
			}
			break;
		case S_WAITHBACK:
		case S_WAITHISTACK:
			if(ABSVALUE(GetSecCnt(),g_SendDataTemp) > 5)
			{//5s为反应
				SendTryCnt++;
				
				if(SendTryCnt > SysInfo.EthRetryCnt)
				{
					g_EthState = S_REGGING;
					ConTryCnt++;
					return 1;
				}
			}
			else
			{
				if(g_EthState == S_WAITHBACK)
				{
					g_EthState = S_REGOK;
					g_EthFreeStamp = GetSecCnt();
				}
				else
				{
					SendEthWithReg();
				}
			}
			break;
		default:
			g_EthState = S_REGGING;
		break;
	}
	
	if(g_EthState >= S_REGOK)
	{
		if(ABSVALUE(GetSecCnt(),g_EthFreeStamp) > SysInfo.EthFreeOffline)
		{//空闲下线时间
			g_EthState = S_REGGING;
			return 1;
		}
	}
	if(g_RecEthLen)
	{
		NETLED_ENABLE;
		g_EthFreeStamp = GetSecCnt();
		
		//下线包
		if(SysInfo.EthServerKickOff[SYS_TXT_DATLEN] > 0 && CompEthData(&SysInfo.EthServerKickOff[0]))
		{
			S282_Log("EthKickOff");
			g_EthState = S_REGGING;
			return 1;
		}
		
		if(g_EthState == S_WAITREGACK)
		{
			//注册包比较
			if(CompEthData(&SysInfo.EthServerAckReg[0]))
			{
				if(SysInfo.EthServerAckHeart[SYS_TXT_DATLEN] == 0)
				{
					ConTryCnt = 0;
				}
				SendTryCnt = 0;
				g_EthState = S_REGOK;
				HeartBeatTemp = GetSecCnt();
			}
		}
		else if(g_EthState >= S_REGOK)
		{
			if(g_EthState >= S_WAITHBACK && CompEthData(&SysInfo.EthServerAckHeart[0]))
			{//注册心跳响应包  响应成功
				ConTryCnt = 0;
				SendTryCnt = 0;
				g_EthState = S_REGOK;
			}
			else if((SysInfo.Rj45Mode & 0x0F) == CT_MODBUS)
			{//mobus
				if(tcp_client_Recbuf[0] == 0x7b && tcp_client_Recbuf[1] == 0x81 && 
					tcp_client_Recbuf[2] == 0x00 && tcp_client_Recbuf[3] == 0x10 )
				{
					return 0;
				}
				S282_LogInt("EthRec",g_RecEthLen);
				WatchDogFeed();
				//modbus - Tcp
				//modbud - Rtu
				AnalyModBus((u8*)tcp_client_Recbuf,g_RecEthLen,3,mReplyBuf);
			}
			
		}
		g_RecEthLen = 0;
	}
	return 0;
}

/*
	连接NTP服务器 获取网络时间
*/
u8 EthGetServerTime(void)
{
	static u8 waitTim = 0;
	if(waitTim > 10)
	{
		waitTim = 0;
	}
	delayMs(10);
	if(g_RecEthLen > 0)
	{//数据包到来
		S282_Log(tcp_client_Recbuf);
		SetUtcTime(tcp_client_Recbuf);
		g_RecEthLen = 0;
		
	}
	waitTim++;
	
	return waitTim;
}
