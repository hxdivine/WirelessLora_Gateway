#include "uip_appcall.h"#include "uip.h"#include <string.h>#include <stdio.h>#include "hardware.h"u8 udp_server_databuf[200]="whj udp server data test";   	//发送数据缓存	  u8 udp_server_sta;				//服务端状态 	   /*	这是一个udp 服务器应用回调函数。*/void udp_server_appcall(void){ 	struct uip_appstate *s = (struct uip_appstate *)&uip_udp_conn->appstate;		//接收到一个新的udp数据包	if (uip_newdata())//收到客户端发过来的数据	{		///*		if((udp_server_sta & (1 << 6)) == 0)//还未收到数据		{			NETLED_ENABLE;			if(uip_len > 199)			{				((u8*)uip_appdata)[199] = 0;			}	    strcpy((char*)udp_server_databuf,uip_appdata);			udp_server_sta|=1<<6;//表示收到客户端数据		}		//*/	}	if(uip_poll())	{		if(udp_server_sta & (1 << 5))//有数据需要发送		{			s->textptr = udp_server_databuf;			s->textlen=strlen((const char*)udp_server_databuf);			udp_server_sta &= ~(1 << 5);//清除标记			uip_send(s->textptr, s->textlen);//发送udp数据包				uip_udp_send(s->textlen);		}	}}	  /*	建立一个udp_server连接*/void udp_server_connect(void){	uip_ipaddr_t ipaddr;	static struct uip_udp_conn *c = 0;		//将远程IP设置为 255.255.255.255 具体原理见uip.c的源码	uip_ipaddr(&ipaddr,0xff,0xff,0xff,0xff);		if(c != 0)	{	//已经建立连接则删除连接								uip_udp_remove(c);	}	c = uip_udp_new(&ipaddr,0); 	//远程端口为0	if(c)	{		uip_udp_bind(c, HTONS(1600));	}}