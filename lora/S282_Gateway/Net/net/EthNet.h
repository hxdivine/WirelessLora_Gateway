
#ifndef _ETHNET_H_
#define _ETHNET_H_

#include "def.h"

//client ���յ����ݳ���
extern u16 g_RecEthLen;
//client ��������buf
extern u8 tcp_client_Recbuf[500];

void InitLocalIP(u8 mode);

void InitNet(void);

void NetRunning(void);

void SetUipDbg(u8 mode);

u8 EthFun(void);

u8 EthGetServerTime(void);

#endif
