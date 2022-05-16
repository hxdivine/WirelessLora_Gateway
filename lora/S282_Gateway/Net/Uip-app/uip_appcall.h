#ifndef __UIP_APPCALL_H__
#define __UIP_APPCALL_H__		 

#include "uipopt.h"
#include "psock.h"
#include "def.h"

//通信程序状态字(用户可以自己定义)  
enum
{
	TcpClient_FREE 	= 0,
	TcpClient_Dhcp,
	TcpClient_ReadyConnect,		//	
	TcpClient_DnsRequest,	
	TcpClient_Connectting,
	TcpClient_ConnectOk,
	TcpClient_Fail,
};

typedef u16_t uip_ip4addr_t[2];

enum
{
	e_Tcp_aborted	 	=0x01,		//
	e_Tcp_closed		=0x02,
	e_Tcp_timedout	=0x04,
	e_Tcp_connected	=0x08,
	e_Tcp_acked			=0x10,
	e_Tcp_newdata		=0x20,
	e_Tcp_poll			=0x40, 
	e_Dns_found			=0x80,
	e_PackHeartResq	=0x100,
	e_PackDisConn		=0x200,
};
  
//定义应用程序回调函数 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_appcall //定义回调函数为 tcp_appcall 
#endif

#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL udp_appcall //定义回调函数为 udp_appcall 
#endif

//定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
//成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。
//uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t		  
struct uip_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct uip_appstate uip_tcp_appstate_t;

//发送数据缓存	
extern u8 tcp_server_databuf[],tcp_server_tempBuf[];   		 
//服务端状态 
extern u8 tcp_server_sta;				  
//客户端状态 
extern u8 tcp_client_sta;				 
//发送数据缓存
extern u8 tcp_client_sendbuf[500];

//tcp server 函数
void tcp_server_appcall(void);

void tcp_client_appcall(void);

void tcp_client_senddata(void);

u16 u8cpy(u8 *dest, u8 *src, u16 max);

void tcp_appcall(void);

void udp_appcall(void);

void udp_client_appcall(void);

void udp_server_connect(void);

void udp_client_connect(void);

void udp_server_appcall(void);

#endif
