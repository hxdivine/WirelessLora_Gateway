#include "uip_appcall.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "def.h"
#include "config.h"
#include "modbus.h"
#include "hardware.h"
#include "stdlib.h"

 //�������ݻ���	  
u8 tcp_server_databuf[250],tcp_server_tempBuf[252];  
//�����״̬
u8 tcp_server_sta;	
 	 
/*
	����һ��TCP ������Ӧ�ûص�������
*/
void tcp_server_appcall(void)
{
 	struct uip_appstate *s = (struct uip_appstate *)&uip_conn->appstate;

	if(uip_aborted())
	{	//������ֹ
		tcp_server_sta &= ~(1<<7);	//��־û������
		uip_log("tcp_server aborted!\r\n");//��ӡlog
 	}
	if(uip_timedout())
	{	//���ӳ�ʱ  
		tcp_server_sta &= ~(1<<7);	//��־û������
		uip_log("tcp_server aborted!\r\n");//��ӡlog
	}
	if(uip_closed())
	{	//���ӹر�	
		uip_log("tcp_server uip_closed\r\n");//��ӡlog
		tcp_server_sta &= ~(1<<7);	//��־û������
 	}
	if(uip_connected())
	{	//���ӳɹ�
		tcp_server_sta |= 1<<7;		//��־���ӳɹ�
		uip_log("tcp_server connected!\r\n");//��ӡlog   
	}
	if(uip_acked())
	{//���͵����ݳɹ��ʹ� 
		s->textlen = 0;//��������
		uip_log("tcp_server acked!\r\n");//��ʾ�ɹ�����		 	
	}
		//����Ҫ�ط���֪ͨuip�������� 
	if(uip_poll())
	{
		if(tcp_server_sta & (1 << 5))//��������Ҫ����
		{
			s->textptr = &tcp_server_tempBuf[2];
			s->textlen = (tcp_server_tempBuf[0] << 8) + tcp_server_tempBuf[1];
			tcp_server_sta &= ~(1 << 5);//������
			uip_send(s->textptr, s->textlen);
		} 
	}

	//���յ�һ���µ�TCP���ݰ� 
	if(uip_newdata())//�յ��ͻ��˷�����������
	{
		if(uip_len > 5)
		{
			S282_LogInt("TcpServerRecLen",uip_len);

			if(SysInfo.Rj45Mode & 0x0F)
			{
				NETLED_ENABLE;
				AnalyModBus(uip_appdata,uip_len,2,tcp_server_databuf);
			}
		}
		else
		{
			uip_len = 0;
		}
	}
	//�ط�
	if(uip_rexmit())
	{
		uip_send(s->textptr, s->textlen);//����TCP���ݰ�	
	}
}
