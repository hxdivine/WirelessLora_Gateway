
#ifndef _HC_I2_C_H_
#define _HC_I2_C_H_

#include "def.h"





#define IIC0						0
#define	IIC1						1


void IIC_Init(void);

void IIC_Start(u8 mode);

void IIC_Stop(u8 mode);

u8 IIC_SendByte(u8 txd,u8 mode);

u8 IIC_ReadByte(u8 ack,u8 mode);


void IIC_TestInit(void);
void IIC_TEST_Start(void);
void IIC_TEST_Stop(void);
u8 IIC_TEST_SendByte(u8 data);
u8 IIC_TEST_ReadByte(u8 ack);

#endif
