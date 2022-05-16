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

//����״̬��־
u8 g_EthState = S_REGGING;
//������������
u16 g_EthFreeStamp;
//�������� ����ʱ��
u16 g_SendDataTemp = 0;
//����Ӧ��������
u8 SendTryCnt = 0;
//���ڼ�¼��̫�����Ƿ�Ͽ�
u8 Rj45Err = 0;
/*
	�ַ�ת16����
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
	�ַ�תIP
*/
u8 str2ip(u8 *dest, u8 *src)
{
	u8 i;
	u8 k = 0;
	u16 dat = 0;
	
	for(i = 0; i < 4; i++)
	{
		//���س���	
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
	���������
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
	���ڼ��
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
	����IP����
*/
void InitLocalIP(u8 mode)
{
	uip_ipaddr_t ipaddr;
	//���ñ�������IP��ַ
	uip_ipaddr(ipaddr,SysInfo.LocalIp[0],SysInfo.LocalIp[1],SysInfo.LocalIp[2],SysInfo.LocalIp[3]);
	uip_sethostaddr(ipaddr);	
	if(mode == 1)
	{
		return;
	}		
	//��������IP��ַ(��ʵ������·������IP��ַ)
	uip_ipaddr(ipaddr,SysInfo.GateIp[0],SysInfo.GateIp[1],SysInfo.GateIp[2],SysInfo.GateIp[3]);	
	uip_setdraddr(ipaddr);			

	//������������
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
	�����ʼ��
*/
void InitNet(void)
{
	//Mac��ʼ��
	tapdev_init();
	//uipЭ���ʼ��
	uip_init();		
		//ARP��ʼ��
	uip_arp_init();
	//�������ݸ�ʽ��ʼ��
	resolv_init();
	//����IP����
	InitLocalIP(0);
}
/*
	��������������

	ע��� + ������
*/
void SendEthWithReg(void)
{
	u16 i; 	
	u8 *buf = malloc(200);
	u8 RegLen = SysInfo.EthRegDat[SYS_TXT_DATLEN];
	u8 len = SysInfo.EthHeartCont[SYS_TXT_DATLEN];
	g_SendDataTemp = GetSecCnt();
	
	for(i = 0; i < len; i++)
	{  //����  ǰ������Ϊע������� ��һ��Ϊ��������
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
	�ر���������
*/
void NetConnClose(u8 Index)
{
	if((SysInfo.Rj45Mode & 0x80))
	{					//udp Э�� �ر�
		uip_udp_remove(g_mconn[Index].udp);
	}
	else
	{					//TcpClient_Close Э��ر�
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
			//���ӷ�����
	g_mconn[i].port = port;		//�˿�
			//׼�����ӷ�����
	if(IsIPaVailable(TarIp))				//�Ƿ�ΪIP
	{
		if(str2ip(ip, TarIp))		//ip�ַ���ת��������
		{
			uip_ipaddr(&ipaddr, ip[0], ip[1], ip[2], ip[3]);
			uip_ipaddr_copy(g_mconn[i].ip, ipaddr);		//��¼IP
			S282_Log("uip connecting...");
			g_mconn[i].tcp = uip_connect(&g_mconn[i].ip, htons(g_mconn[i].port)); 	//�˿�					
			g_mconn[i].state = TcpClient_Connectting;
			stateStamp = GetSecCnt();
		}								
	}
	else	//����
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
			{	//ʹ��DNS2
				dnsSel = 0;
				if(SysInfo.DnsIp2[1] != 0 && SysInfo.DnsIp2[0] != 0xff)	
				{		
					uip_ipaddr(ipaddr, SysInfo.DnsIp2[0],SysInfo.DnsIp2[1],SysInfo.DnsIp2[2],SysInfo.DnsIp2[3]);
					HaveDns = 1;
				}
			}
		}
		if(HaveDns || DhcpOk)
		{//������������
			g_mconn[i].state = TcpClient_DnsRequest;
			g_mconn[i].flag &= ~e_Dns_found;
			S282_Log("Analy Target Server ip");
			if(resolv_getserver() == NULL)
			{
				resolv_conf(ipaddr);		//����DNS������
			}
			g_mconn[i].dnsno = resolv_query((char*)&TarIp[0]);//������������
			g_mconn[i].dnsno |= 0x80;
			stateStamp = GetSecCnt();
		}						
	}
	return stateStamp;
}
/*
	�������� ѭ�����
*/
void NetRunning(void)
{
	//1.DNSѡ������	2.���ӷ�����ʧ�ܴ���	3. ������ӷ�����ʧ��  ���÷���������
	static u8 failCnt=0,failTryCnt = 0;
	//������Ӧʱ�䱣������
	static u32 stateStamp = 0;
	//������ʱ���� 
	static u16 TimDelay = 0;
	static u8 mqttConSta = 0;
	u8 i = 0;
	
	if(Rj45Err > 10 && SysInfo.NetLinkPriority == ETHER_NETWORK)
	{//����û�������� ���÷���������
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
	{//5������ʧ�� ����̫�����������ʹ�÷���������
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
	{//��ʱ���������� �����ӳɹ�
		return;
	}
	switch(g_mconn[i].state)//10msһ��
	{
		case TcpClient_FREE://û������
			CheckConn();
			if(ETHLineOut() == 0)	//û����
			{		
				if(SysInfo.Dhcp == 1)
				{//��̬����
					dhcpc_init(uip_ethaddr.addr, 6);
					dhcpc_request();
					DhcpOk = 0;
					stateStamp = GetSecCnt();
					g_mconn[i].state = TcpClient_Dhcp;
				}
				else
				{//S282��ΪTCPClient ׼�����ӷ�����
					g_mconn[i].state = TcpClient_ReadyConnect;
				}
			}
			break;
		case TcpClient_Dhcp:
			if(ABSVALUE(GetSecCnt(),stateStamp) > 5)
			{//5sδ��Ӧ
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
			{	//10�볬ʱ
				g_mconn[i].state = TcpClient_FREE;
				stateStamp = GetSecCnt();
				failCnt++;				
				
				if(failCnt >= 3)
				{//����ʧ�ܴ�������3 ��������ʧ��
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
			if(g_mconn[i].flag & e_Dns_found)	//�����ɹ�
			{
				S282_Log("uip connecting...");
				g_mconn[i].flag &= ~e_Dns_found;				
				failCnt = 0;	
				if((SysInfo.Rj45Mode & 0x80) == 0 || UpdateTime)
				{
					g_mconn[i].tcp = uip_connect(&g_mconn[i].ip, htons(g_mconn[i].port)); 	//�˿�
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
			{//5��
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
			if(g_mconn[i].flag & e_Tcp_connected)		//���ӳɹ�
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
			{	//���ӳ�ʱ
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
					if(EthFun())//����ͨ��0 �쳣1
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
	�Ƚ� ��̫�����յ�����
	��� 1 ����� 0
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
	ETH ���繦�ܺ���
	TCP���ӳɹ���  ���ڸú���ѭ��ִ��

*/
u8 EthFun(void)
{
	//����ʱ��
	static u16 HeartBeatTemp = 0;
	//�����������˴���
	static u8 ConTryCnt = 0;
	
	//��������״̬
	switch(g_EthState)
	{
		case S_REGGING:
			//����ע���
			UipClient2Server(SysInfo.EthRegDat,SysInfo.EthRegDat[SYS_TXT_DATLEN]);
			g_SendDataTemp = GetSecCnt();//10ms
			if(SysInfo.EthServerAckReg[SYS_TXT_DATLEN] > 0)
			{//ע����Ӧ��
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
			{//5s�޷�Ӧ
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
			{//��������Ϊ��   �����������ڲ���ִ��
				HeartBeatTemp = GetSecCnt();
				//����������
				UipClient2Server(SysInfo.EthHeartCont,SysInfo.EthHeartCont[SYS_TXT_DATLEN]);
				g_SendDataTemp = GetSecCnt();
				if(SysInfo.EthServerAckHeart[SYS_TXT_DATLEN] > 0)
				{//ע��������Ӧ��
					g_EthState = S_WAITHBACK;
				}
			}
			break;
		case S_WAITHBACK:
		case S_WAITHISTACK:
			if(ABSVALUE(GetSecCnt(),g_SendDataTemp) > 5)
			{//5sΪ��Ӧ
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
		{//��������ʱ��
			g_EthState = S_REGGING;
			return 1;
		}
	}
	if(g_RecEthLen)
	{
		NETLED_ENABLE;
		g_EthFreeStamp = GetSecCnt();
		
		//���߰�
		if(SysInfo.EthServerKickOff[SYS_TXT_DATLEN] > 0 && CompEthData(&SysInfo.EthServerKickOff[0]))
		{
			S282_Log("EthKickOff");
			g_EthState = S_REGGING;
			return 1;
		}
		
		if(g_EthState == S_WAITREGACK)
		{
			//ע����Ƚ�
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
			{//ע��������Ӧ��  ��Ӧ�ɹ�
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
	����NTP������ ��ȡ����ʱ��
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
	{//���ݰ�����
		S282_Log(tcp_client_Recbuf);
		SetUtcTime(tcp_client_Recbuf);
		g_RecEthLen = 0;
		
	}
	waitTim++;
	
	return waitTim;
}
