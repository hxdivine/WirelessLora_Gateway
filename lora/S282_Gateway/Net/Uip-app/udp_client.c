#include "uip_appcall.h"
#include "uip.h"
#include "tapdev.h"
#include <string.h>
#include <stdio.h>

/*
	��ȡUDP �����Ӿ��
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
	����һ��udp �ͻ���Ӧ�ûص�������
*/
void udp_client_appcall(void)
{		 
 	struct uip_appstate *s = (struct uip_appstate *)&uip_udp_conn->appstate;
	u8 i;
	
	i = GetUipUdpConn(uip_udp_conn);
	if(uip_newdata())					//�����µ�udp���ݰ� 	
	{
		/*
		if((udp_client_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
			strcpy((char*)udp_client_databuf,uip_appdata);
			udp_client_databuf[uip_len]=0;
			udp_client_sta|=1<<6;//��ʾ�յ��ͻ�������
		}
		*/
		
	} 
	
	if(uip_poll())
	{
		//��ǰ���ӿ�����ѵ
		if(g_mconn[i].flag & e_Tcp_poll)
		{
			s->textptr = &g_mconn[i].sbuf[2];
			s->textlen = (g_mconn[i].sbuf[0]<<8)+g_mconn[i].sbuf[1];
			g_mconn[i].flag &= ~e_Tcp_poll;//������
			uip_send(s->textptr, s->textlen);//����udp���ݰ�
			uip_udp_send(s->textlen);
		}
	}
}

/*
	����һ��udp_client������
*/
void udp_client_connect(void)
{
	uip_ipaddr_t ipaddr;
	static struct uip_udp_conn *c=0;
	
	//USART_OUT(USART1,"udp connect\r\n");
	uip_ipaddr(&ipaddr,192,168,1,160);	//����IPΪ192.168.1.103
	if(c!=0)
	{							//�Ѿ�����������ɾ������
		uip_udp_remove(c);
	}
	c = uip_udp_new(&ipaddr,htons(1500)); 	//�˿�Ϊ1500
}
