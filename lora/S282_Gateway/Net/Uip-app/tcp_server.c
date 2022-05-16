#include "uip_appcall.h"
#include "uip.h"
#include <string.h>
#include <stdio.h>
#include "def.h"
#include "config.h"
#include "modbus.h"
#include "hardware.h"
#include "stdlib.h"

 //发送数据缓存	  
u8 tcp_server_databuf[250],tcp_server_tempBuf[252];  
//服务端状态
u8 tcp_server_sta;	
 	 
/*
	这是一个TCP 服务器应用回调函数。
*/
void tcp_server_appcall(void)
{
 	struct uip_appstate *s = (struct uip_appstate *)&uip_conn->appstate;

	if(uip_aborted())
	{	//连接终止
		tcp_server_sta &= ~(1<<7);	//标志没有连接
		uip_log("tcp_server aborted!\r\n");//打印log
 	}
	if(uip_timedout())
	{	//连接超时  
		tcp_server_sta &= ~(1<<7);	//标志没有连接
		uip_log("tcp_server aborted!\r\n");//打印log
	}
	if(uip_closed())
	{	//连接关闭	
		uip_log("tcp_server uip_closed\r\n");//打印log
		tcp_server_sta &= ~(1<<7);	//标志没有连接
 	}
	if(uip_connected())
	{	//连接成功
		tcp_server_sta |= 1<<7;		//标志连接成功
		uip_log("tcp_server connected!\r\n");//打印log   
	}
	if(uip_acked())
	{//发送的数据成功送达 
		s->textlen = 0;//发送清零
		uip_log("tcp_server acked!\r\n");//表示成功发送		 	
	}
		//当需要重发、通知uip发送数据 
	if(uip_poll())
	{
		if(tcp_server_sta & (1 << 5))//有数据需要发送
		{
			s->textptr = &tcp_server_tempBuf[2];
			s->textlen = (tcp_server_tempBuf[0] << 8) + tcp_server_tempBuf[1];
			tcp_server_sta &= ~(1 << 5);//清除标记
			uip_send(s->textptr, s->textlen);
		} 
	}

	//接收到一个新的TCP数据包 
	if(uip_newdata())//收到客户端发过来的数据
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
	//重发
	if(uip_rexmit())
	{
		uip_send(s->textptr, s->textlen);//发送TCP数据包	
	}
}
