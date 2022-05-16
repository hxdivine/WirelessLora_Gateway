#ifndef __UIP_APPCALL_H__
#define __UIP_APPCALL_H__		 

#include "uipopt.h"
#include "psock.h"
#include "def.h"

//ͨ�ų���״̬��(�û������Լ�����)  
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
  
//����Ӧ�ó���ص����� 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_appcall //����ص�����Ϊ tcp_appcall 
#endif

#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL udp_appcall //����ص�����Ϊ udp_appcall 
#endif

//���� uip_tcp_appstate_t �������ͣ��û��������Ӧ�ó�����Ҫ�õ�
//��Ա��������Ҫ���Ľṹ�����͵����֣���Ϊ����������ᱻuip���á�
//uip.h �ж���� 	struct uip_conn  �ṹ���������� uip_tcp_appstate_t		  
struct uip_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct uip_appstate uip_tcp_appstate_t;

//�������ݻ���	
extern u8 tcp_server_databuf[],tcp_server_tempBuf[];   		 
//�����״̬ 
extern u8 tcp_server_sta;				  
//�ͻ���״̬ 
extern u8 tcp_client_sta;				 
//�������ݻ���
extern u8 tcp_client_sendbuf[500];

//tcp server ����
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
