
#ifndef __TAPDEV_H__
#define __TAPDEV_H__

#include <stdint.h>
#include "def.h"

typedef struct
{

	u16 ip[2];		//����IP
	u16 port;			//�˿�
	u16 flag;
	u8 state;			//״̬		
	u8 dnsno;			//bit7λ��������,bit0~bit6Ϊ�������
	
	u8 *sbuf;
	//u8 *rbuf;
	struct uip_conn *tcp;
	struct uip_udp_conn *udp;
	
	
} M_Conn;

extern M_Conn g_mconn[2];
extern u16 g_uiLintenPort;

u8 tapdev_init(void);		//�޸�Ϊ������ֵ�ĺ���	 
uint16_t tapdev_read(void);
void tapdev_send(void);

void UipServerAck(u8 *buf,u16 len);

void UipServer2Client(u8 *buf,u16 len);

void TcpClose(struct uip_conn * sock);

void UipClient2Server(u8 *buf,u16 len);

void uip_polling(void);

#endif /* __TAPDEV_H__ */
