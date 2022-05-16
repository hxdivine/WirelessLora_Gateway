#include "uip_appcall.h"
#include "uip.h"
#include "tapdev.h"
#include <string.h>
#include <stdio.h>

/*
	获取UDP 的连接句柄
*/
u8 GetUipUdpConn(struct uip_udp_conn *uip)
{
	u8 i;
	
	for(i=0; i<2; i++)
	{
		if(g_mconn[i].udp==uip)
		{
			return i;
		}
	}
	
	return 0xff;
}

/*
	这是一个udp 客户端应用回调函数。
*/
void udp_client_appcall(void)
{		 
 	struct uip_appstate *s = (struct uip_appstate *)&uip_udp_conn->appstate;
	u8 i;
	
	i = GetUipUdpConn(uip_udp_conn);
	if(uip_newdata())					//接收新的udp数据包 	
	{
		/*
		if((udp_client_sta&(1<<6))==0)//还未收到数据
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
			strcpy((char*)udp_client_databuf,uip_appdata);
			udp_client_databuf[uip_len]=0;
			udp_client_sta|=1<<6;//表示收到客户端数据
		}
		*/
		
	} 
	
	if(uip_poll())
	{
		//当前连接空闲轮训
		if(g_mconn[i].flag & e_Tcp_poll)
		{
			s->textptr = &g_mconn[i].sbuf[2];
			s->textlen = (g_mconn[i].sbuf[0]<<8)+g_mconn[i].sbuf[1];
			g_mconn[i].flag &= ~e_Tcp_poll;//清除标记
			uip_send(s->textptr, s->textlen);//发送udp数据包
			uip_udp_send(s->textlen);
		}
	}
}

/*
	建立一个udp_client的连接
*/
void udp_client_connect(void)
{
	uip_ipaddr_t ipaddr;
	static struct uip_udp_conn *c=0;
	
	//USART_OUT(USART1,"udp connect\r\n");
	uip_ipaddr(&ipaddr,192,168,1,160);	//设置IP为192.168.1.103
	if(c!=0)
	{							//已经建立连接则删除连接
		uip_udp_remove(c);
	}
	c = uip_udp_new(&ipaddr,htons(1500)); 	//端口为1500
}
