
#include "gd32e10x.h"
#include "sx127x_Hal.h"
#include "spi.h"

/*
	��loraģ��Ĵ�����ַд��������
*/
void SX127X_WriteBuffer(uint8_t addr,uint8_t *buffer,uint8_t size)
{
	LORACS_L
	Spi1ReadWriteByte(addr | 0x80);
	for(u8 i = 0; i < size; i++)
	{
		Spi1ReadWriteByte(buffer[i]);
	}
	LORACS_H
}

/*
	��loraģ��Ĵ�����ȡ�������
*/
void SX127X_ReadBuffer(uint8_t addr,uint8_t *buffer,uint8_t size)
{
	LORACS_L
	Spi1ReadWriteByte(addr & 0x7F);
	for(u8 i = 0; i < size; i++)
	{
		buffer[i] = Spi1ReadWriteByte(0x00);
	}
	LORACS_H
}
