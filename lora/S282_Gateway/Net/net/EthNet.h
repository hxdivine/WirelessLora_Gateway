
#ifndef _ETHNET_H_
#define _ETHNET_H_

#include "def.h"

//client 接收的数据长度
extern u16 g_RecEthLen;
//client 接收数据buf
extern u8 tcp_client_Recbuf[500];

void InitLocalIP(u8 mode);

void InitNet(void);

void NetRunning(void);

void SetUipDbg(u8 mode);

u8 EthFun(void);

u8 EthGetServerTime(void);

#endif
