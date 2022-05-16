/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: tapdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */	    
#include "tapdev.h"
#include "uip.h"
#include "enc28j60.h"
#include "stdio.h"
#include "dhcpc.h"
#include "config.h"
#include "uip_arp.h"
#include "uip_appcall.h"

u8 DhcpOk = 0;
u8 DhcpIp[4] = {0};
M_Conn g_mconn[2];

u16 g_uiLintenPort;

//配置网卡硬件，并设置MAC地址 
//返回值：0，正常；1，失败；
u8 tapdev_init(void)
{   	 
	u8 i,res = 0;		
	u8 mymac[6]={0xe0,0x05, 0xc5, 0xd4, 0x35, 0x3a};		
	res = ENC28J60_Init((u8*)mymac);	//初始化ENC28J60					  
	//把IP地址和MAC地址写入缓存区
 	for (i = 0; i < 6; i++)
	{
		uip_ethaddr.addr[i] = mymac[i];  
	} 
	ENC28J60_PHY_Write(PHLCON,0x0476);
	return res;	
}
//读取一包数据  
uint16_t tapdev_read(void)
{	
	return  ENC28J60_Packet_Receive(MAX_FRAMELEN,uip_buf);
}
//发送一包数据  
void tapdev_send(void)
{
	ENC28J60_Packet_Send(uip_len,uip_buf);
}

//作为服务发到客户端
void UipServerAck(u8 *buf,u16 len)
{
	struct uip_appstate *s = (struct uip_appstate *)&uip_conn->appstate;
	s->textptr = buf;
	s->textlen = len;
	uip_send(s->textptr, s->textlen);//发送TCP数据包
}


void UipServer2Client(u8 *buf,u16 len)
{
	u8 i;
	tcp_server_sta |= (1<<5);//有数据需要发送
	for(i = 0; i < 250 && i < len; i++)
	{	
		tcp_server_tempBuf[2 + i] = buf[i];
	}
	tcp_server_tempBuf[0] = len >> 8;
	tcp_server_tempBuf[1] = len & 0xff;
}

/*
	关闭TCP连接
*/
void TcpClose(struct uip_conn * sock)
{
	if(sock == NULL)
	{
		return;
	}
	if(sock->tcpstateflags == 0)
	{//tcp连接断开
		return;
	}
	uip_Disconn(sock);
	if(uip_len > 0)
	{
		//加以太网结构，在主动连接时可能要构造ARP请求
		uip_arp_out();
		//发送数据到以太网
		tapdev_send();
	}
}

/*
	UDP发送数据
*/
void UdpSendData(struct uip_udp_conn *udp)
{
	uip_udp_periodic_conn(udp);
	if(uip_len > 0)
	{
		uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
		tapdev_send();//发送数据到以太网
	}
}

/*
	tcp连接发送数据
*/
void TcpSendData(struct uip_conn *sock)
{
	//sock connect
	uip_poll_conn(sock);
	if(uip_len > 0)
	{
		//加以太网结构，在主动连接时可能要构造ARP请求
		uip_arp_out();
		//发送数据到以太网
		tapdev_send();
	}
}

/*
	作为client 发送数据到server
*/
void UipClient2Server(u8 *buf,u16 len)
{
	M_Conn *con;
	con = &g_mconn[0];
	//没有连接上服务器
	if(con->tcp->tcpstateflags != UIP_ESTABLISHED)
	{
		return;
	}
	tcp_client_sendbuf[0]=len >> 8;
	tcp_client_sendbuf[1]=len & 0xff;
	//sendbuf ： len + data
	u8cpy(&tcp_client_sendbuf[2], buf, len);
	con->flag &= ~e_Tcp_acked;
	con->flag |= e_Tcp_poll;
	if((SysInfo.Rj45Mode & 0x80) == 0)		//tcp
	{
		TcpSendData(con->tcp);
	}
	else
	{
		UdpSendData(con->udp);
	}
}
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	 

/*
	uip事件处理函数
	必须将该函数插入到主函数中
*/
void uip_polling(void)
{
	u32 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;
	
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}
	
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义

	if(uip_len>0) 			//有数据
	{
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		    
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}
	else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}
