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

//��������Ӳ����������MAC��ַ 
//����ֵ��0��������1��ʧ�ܣ�
u8 tapdev_init(void)
{   	 
	u8 i,res = 0;		
	u8 mymac[6]={0xe0,0x05, 0xc5, 0xd4, 0x35, 0x3a};		
	res = ENC28J60_Init((u8*)mymac);	//��ʼ��ENC28J60					  
	//��IP��ַ��MAC��ַд�뻺����
 	for (i = 0; i < 6; i++)
	{
		uip_ethaddr.addr[i] = mymac[i];  
	} 
	ENC28J60_PHY_Write(PHLCON,0x0476);
	return res;	
}
//��ȡһ������  
uint16_t tapdev_read(void)
{	
	return  ENC28J60_Packet_Receive(MAX_FRAMELEN,uip_buf);
}
//����һ������  
void tapdev_send(void)
{
	ENC28J60_Packet_Send(uip_len,uip_buf);
}

//��Ϊ���񷢵��ͻ���
void UipServerAck(u8 *buf,u16 len)
{
	struct uip_appstate *s = (struct uip_appstate *)&uip_conn->appstate;
	s->textptr = buf;
	s->textlen = len;
	uip_send(s->textptr, s->textlen);//����TCP���ݰ�
}


void UipServer2Client(u8 *buf,u16 len)
{
	u8 i;
	tcp_server_sta |= (1<<5);//��������Ҫ����
	for(i = 0; i < 250 && i < len; i++)
	{	
		tcp_server_tempBuf[2 + i] = buf[i];
	}
	tcp_server_tempBuf[0] = len >> 8;
	tcp_server_tempBuf[1] = len & 0xff;
}

/*
	�ر�TCP����
*/
void TcpClose(struct uip_conn * sock)
{
	if(sock == NULL)
	{
		return;
	}
	if(sock->tcpstateflags == 0)
	{//tcp���ӶϿ�
		return;
	}
	uip_Disconn(sock);
	if(uip_len > 0)
	{
		//����̫���ṹ������������ʱ����Ҫ����ARP����
		uip_arp_out();
		//�������ݵ���̫��
		tapdev_send();
	}
}

/*
	UDP��������
*/
void UdpSendData(struct uip_udp_conn *udp)
{
	uip_udp_periodic_conn(udp);
	if(uip_len > 0)
	{
		uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
		tapdev_send();//�������ݵ���̫��
	}
}

/*
	tcp���ӷ�������
*/
void TcpSendData(struct uip_conn *sock)
{
	//sock connect
	uip_poll_conn(sock);
	if(uip_len > 0)
	{
		//����̫���ṹ������������ʱ����Ҫ����ARP����
		uip_arp_out();
		//�������ݵ���̫��
		tapdev_send();
	}
}

/*
	��Ϊclient �������ݵ�server
*/
void UipClient2Server(u8 *buf,u16 len)
{
	M_Conn *con;
	con = &g_mconn[0];
	//û�������Ϸ�����
	if(con->tcp->tcpstateflags != UIP_ESTABLISHED)
	{
		return;
	}
	tcp_client_sendbuf[0]=len >> 8;
	tcp_client_sendbuf[1]=len & 0xff;
	//sendbuf �� len + data
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
	uip�¼�������
	���뽫�ú������뵽��������
*/
void uip_polling(void)
{
	u32 i;
	static struct timer periodic_timer, arp_timer;
	static u8 timer_ok=0;
	
	if(timer_ok==0)//����ʼ��һ��
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //����1��0.5��Ķ�ʱ��
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//����1��10��Ķ�ʱ�� 
	}
	
	uip_len=tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���.uip_len��uip.c�ж���

	if(uip_len>0) 			//������
	{
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��? 
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)		    
			if(uip_len>0)//��Ҫ��Ӧ����
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
 			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
 			if(uip_len>0)tapdev_send();//��Ҫ��������,��ͨ��tapdev_send����	 
		}
	}
	else if(timer_expired(&periodic_timer))	//0.5�붨ʱ����ʱ
	{
		timer_reset(&periodic_timer);		//��λ0.5�붨ʱ�� 
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�  
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
	 		if(uip_len>0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#if UIP_UDP	//UIP_UDP 
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
	 		//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
#endif 
		//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}
