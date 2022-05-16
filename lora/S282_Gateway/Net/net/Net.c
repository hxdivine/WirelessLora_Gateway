
#include "Net.h"
#include "uip.h"
#include "rtc.h"
#include "enc28j60.h"
#include "config.h"
#include "hardware.h"
#include "tapdev.h"
#include "dhcpc.h"
#include "uip_appcall.h"
#include "resolv.h"
#include "stdio.h"
#include "string.h"

u8 g_EthState = S_REGGING;
static u32 g_SendDataTemp = 0;
static u8 SnedTryCnt = 0;
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
		printf("phy estat: 0x40 error\n\r");
		tapdev_init();		
	}	
}

/*
	�������� ѭ�����
*/
void NetRunning(void)
{
	static u8 DnsSel=0,FailCnt=0;
	//static u32 StateStamp;
	u8 i = 0;
	
	Enc28j60Check();
	uip_polling();
	
	switch(g_mconn[i].state)//10msһ��
	{
		case TcpClient_FREE://û������
			CheckConn();
			if(ETHLineOut() == 0)
			{			//û����
				if(SysInfo.LocalIp[0] == 0 || SysInfo.LocalIp[0] == 0xff)
				{
					printf("dhcp...\r\n");
					//��̬����
					dhcpc_init(uip_ethaddr.addr, 6);
					dhcpc_request();
					DhcpOk = 0;
					//StateStamp=Sys1msCnt;
					g_mconn[i].state = TcpClient_Dhcp;
				}
				else
				{//S282��ΪTCPClient ׼�����ӷ�����
					g_mconn[i].state = TcpClient_Close;
				}
			}
			break;
		case TcpClient_Dhcp:
//			if(Sys1msCnt-StateStamp>1000*5){
//				g_mconn[i].state = TcpClient_FREE;
//			}
			if(DhcpOk)
			{
				g_mconn[i].state = TcpClient_Close;
			}
			break;
		case TcpClient_Close://2
		{
			uip_ipaddr_t ipaddr;
			u8 ip[4];
			
			g_mconn[i].flag = 0;
			//StateStamp=Sys1msCnt;
			//���ӷ�����
			g_mconn[i].port = SysInfo.EthTarTcpPort;		//�˿�
			//׼�����ӷ�����
			if( IsIPaVailable(SysInfo.EthTarIP) )				//�Ƿ�ΪIP
			{
				if(str2ip(ip, SysInfo.EthTarIP))		//ip�ַ���ת��������
				{
					uip_ipaddr(&ipaddr, ip[0], ip[1], ip[2], ip[3]);
					uip_ipaddr_copy(g_mconn[i].ip, ipaddr);		//��¼IP
					if(UipDbMode == 1)
					{	
						printf("uip connecting...\r\n");
					}
					g_mconn[i].tcp = uip_connect(&g_mconn[i].ip, htons(g_mconn[i].port)); 	//�˿�					
					g_mconn[i].state = TcpClient_Connectting;
					//StateStamp=Sys1msCnt;
				}								
			}
			else	//����
			{				
				u8 HaveDns = 0;
				if(DhcpOk == 0)
				{
					if(DnsSel == 0)
					{		
						if(SysInfo.DnsIp1[1] != 0 && SysInfo.DnsIp1[0] != 0xff)
						{
							uip_ipaddr(ipaddr, SysInfo.DnsIp1[0],SysInfo.DnsIp1[1],SysInfo.DnsIp1[2],SysInfo.DnsIp1[3]);
							HaveDns = 1;
							if(UipDbMode == 1)
							{
								printf("require dns1\r\n");
							}
						}
						if(SysInfo.DnsIp2[1] !=0 || SysInfo.DnsIp1[2] !=0 )	
						{
							DnsSel = 1;
						}
					}
					if((HaveDns == 0) && DnsSel)
					{	//ʹ��DNS2
						DnsSel = 0;
						if(SysInfo.DnsIp2[1] != 0 && SysInfo.DnsIp2[0] != 0xff)	
						{		
							uip_ipaddr(ipaddr, SysInfo.DnsIp2[0],SysInfo.DnsIp2[1],SysInfo.DnsIp2[2],SysInfo.DnsIp2[3]);
							HaveDns = 1;
							if(UipDbMode == 1)
							{
								printf("require dns2\r\n");
							}								
						}
					}
				}
				if(HaveDns || DhcpOk)
				{
					g_mconn[i].state = TcpClient_DnsRequest;
					g_mconn[i].flag &= ~e_Dns_found;					
					if(resolv_getserver() == NULL)
					{
						resolv_conf(ipaddr);		//����DNS������
					}
					g_mconn[i].dnsno = resolv_query((char*)&SysInfo.EthTarIP[0]);//������������
					g_mconn[i].dnsno |= 0x80;
					//StateStamp=Sys1msCnt;
				}				
			}			
			break;
		}			
		case TcpClient_DnsRequest://3
			//Sys1msCnt-StateStamp>1000*10
			if(1)
			{	//10�볬ʱ
				g_mconn[i].state = TcpClient_FREE;
			//	StateStamp = Sys1msCnt;
				FailCnt++;				
				
				if(FailCnt >= 3)
				{
					FailCnt = 0;
					g_mconn[i].state = TcpClient_Fail;
					if(UipDbMode == 1)
					{
						printf("Dns_Fail\r\n");
					}
				}
			}
			if(g_mconn[i].flag & e_Dns_found)	//�����ɹ�
			{
				if(UipDbMode == 1)
				{
					printf("dnsok\r\n");
					printf("uip connecting...\r\n");
				}
	
				g_mconn[i].flag &= ~e_Dns_found;				
				FailCnt = 0;	
				if((SysInfo.Rj45Mode & 0x80) == 0)
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
				
				//StateStamp=Sys1msCnt;
			}
			break;			
		case TcpClient_Connectting://5
		{		//	Sys1msCnt-StateStamp>1000*10
			if(1)
			{//5��
				printf("EthOverTime\r\n");
				//g_mconn[i].flag &= ~e_Tcp_timedout;
				if((SysInfo.Rj45Mode & 0x80))
				{//10s û�н��յ����� �ر�upd
					
					uip_udp_remove(g_mconn[i].udp);
				}
				else
				{//�ر�Tcp
					TcpClose(g_mconn[i].tcp);
				}
				g_mconn[i].state = TcpClient_FREE;
				//StateStamp = Sys1msCnt;
				FailCnt++;
				if(FailCnt >= 3)
				{
					FailCnt = 0;
					g_mconn[i].state = TcpClient_Fail;
					if(UipDbMode == 1)
					{
						printf("TcpClient_Fail\r\n");
					}
				}
			}
			if(g_mconn[i].flag & e_Tcp_connected)		//���ӳɹ�
			{
				g_mconn[i].flag = 0;
				FailCnt = 0;
				//StateStamp=Sys1msCnt;
				
				g_mconn[i].state = TcpClient_ConnectOk;
			//	if(UipDbMode==1)
			//	printf("Tcp connected\r\n");
		//		ReadMemData(GetGprsDataAddr2(e_GprsPackReg), (u8*)&g_mconn[i].DisConn, sizeof(M_GprsDataPack));//��ȡ�Ͽ�������Ϣ
		//		g_mconn[i].Reg.state=0;		//��������ע�������
			}
			break;
		}
		case TcpClient_ConnectOk:	
			if(ETHLineOut())
			{	//���ӳ�ʱ
				if((SysInfo.Rj45Mode & 0x80))
				{					
					uip_udp_remove(g_mconn[i].udp);
				}
				else
				{
					TcpClose(g_mconn[i].tcp);
				}
				g_mconn[i].state = TcpClient_FREE;	
				break;
			}				//ProcEthData()
			if(1)
			{
				if((SysInfo.Rj45Mode & 0x80))
				{					
					uip_udp_remove(g_mconn[i].udp);
				}
				else
				{
					TcpClose(g_mconn[i].tcp);
				}
				g_mconn[i].state = TcpClient_FREE;
			}
	
			if((g_mconn[i].flag & e_Tcp_closed) ||
				(g_mconn[i].flag & e_Tcp_aborted) ||
				(g_mconn[i].flag & e_Tcp_timedout)||
				(g_mconn[i].flag & e_PackDisConn))	
			{
				
				g_mconn[i].flag = 0;
				g_mconn[i].state = TcpClient_Fail;
				if((SysInfo.Rj45Mode & 0x80))
				{					
					uip_udp_remove(g_mconn[i].udp);
				}
				else
				{
					TcpClose(g_mconn[i].tcp);				
				}
			}
			
			break;		
		case TcpClient_Fail:			
			g_mconn[i].state = TcpClient_FREE;			
			break;
		default:			
			g_mconn[i].state =TcpClient_FREE;
			break;
	}
}

/*
	ETH ���繦�ܺ���
	TCP���ӳɹ���  ���ڸú���ѭ��ִ��

*/
void EthFun(void)
{
	static u32 HeartBeatTemp = 0;
	
	//��������״̬
	switch(g_EthState)
	{
		case S_REGGING:
			UipClient2Server(&SysInfo.EthRegDat[0],SysInfo.EthRegDat[SYS_TXT_DATLEN]);
			g_SendDataTemp = clock_time();//10ms
			
			if(SysInfo.EthServerAckHeart[SYS_TXT_DATLEN] > 0)
			{
				g_EthState = S_WAITREGACK;
			}
			else
			{
				g_EthState = S_REGOK;
			}
			
			HeartBeatTemp = clock_time();
			break;
		case S_WAITREGACK:
			if((clock_time() - g_SendDataTemp) > 5000)
			{//5s�޷�Ӧ
				Se
			}
			break;
	}
}
