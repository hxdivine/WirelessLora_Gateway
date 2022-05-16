#include "spi_flash.h"
#include "gd32e10x.h"
#include "spi.h"
#include "stdio.h"
#include "stdlib.h"
#include "enc28j60.h"
#include "systick.h"
#include "iwdg.h"

/*
	W25Qx64初始化
*/
void W25Q_Init(void)
{
	u8 tryCnt = 0;
	gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_0);
	FLASHCS_H;
	//需要一小段延迟  不然芯片读取不上
	Spi1Init();
	do{
		delayMs(400);
		tryCnt++;
	}while(W25Q_ReadID() == 0 && tryCnt < 3);

}
/*
	flash写使能
*/
static void W25Q_Write_Enable(void)
{
	FLASHCS_L;
	Spi1ReadWriteByte(C_WREN);
	FLASHCS_H;
}
/*
	flash写关闭
*/
static void W25Q_Write_Disable(void)
{//MX25与TS25同 写后擦除后自动加锁

//	Spi1ReadWriteByte(0x04);
}

/*
 读状态寄存器
 BIT7  6   5   4   3   2   1   0
 SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 SPR: 默认0x00
*/
u8 W25Q_ReadSR(void)
{
	u8 byte = 0;
	FLASHCS_L
	Spi1ReadWriteByte(C_RDSR);//读状态
	byte = Spi1ReadWriteByte(0x00);
	FLASHCS_H
	return byte;
}

/*
	等待空闲
*/
void W25Q_Wait_Busy(void)
{
	//等待busy位清空
	while((W25Q_ReadSR() & 0x01) == 0x01);
}
/*
	擦除芯片
*/
void W25Q_Erase_Chip(void)
{
	W25Q_Write_Enable();	
	
	FLASHCS_L;
	Spi1ReadWriteByte(C_CERA);	
	FLASHCS_H;
	
	W25Q_Wait_Busy();
	W25Q_Write_Disable();
}
/*
	擦除扇区
*/
void W25Q_Erase_Sector(u32 Dst_Addr)//扇区内任一地址即可
{
	SpiAsFlash();

	W25Q_Write_Enable();	
	
	FLASHCS_L;	
	Spi1ReadWriteByte(C_SERA);
	Spi1ReadWriteByte((u8)(Dst_Addr >> 16));
	Spi1ReadWriteByte((u8)(Dst_Addr >> 8));
	Spi1ReadWriteByte(0);
	FLASHCS_H;
	
	W25Q_Wait_Busy();
	W25Q_Write_Disable();
	SpiAsNet();
}

/*
	读flash
	连续读时，可自动跨页
*/
void W25Q_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
	SpiAsFlash();
	FLASHCS_L;
	Spi1ReadWriteByte(C_FASTREAD);
	Spi1ReadWriteByte((u8)((ReadAddr&0xFF0000) >> 16));
	Spi1ReadWriteByte((u8)((ReadAddr&0xFF00) >> 8));
	Spi1ReadWriteByte(ReadAddr & 0xff);
	Spi1ReadWriteByte(0x00);//dummy
	for (u16 i=0; i < NumByteToRead; i++)
	{
		pBuffer[i] = Spi1ReadWriteByte(0x00);
	}
	FLASHCS_H;
	SpiAsNet();
}
/*
	写flash
*/
void W25Q_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i,ptr=0;
	u16 WrLen;
	SpiAsFlash();
	
	if((WriteAddr & 0xfff) == 0)
	{	
		W25Q_Erase_Sector(WriteAddr);
	}
	while(1)
	{
		WatchDogFeed();
		W25Q_Write_Enable();	
		if(NumByteToWrite > 256)
		{//一次写页写不完
			WrLen = 256;
		}
		else 
		{//一次可以写完
			WrLen = NumByteToWrite;
		}
		FLASHCS_L
		//写页命令
		Spi1ReadWriteByte(C_PP);//256字节
		Spi1ReadWriteByte((u8)((WriteAddr&0x00FF0000) >> 16));
		Spi1ReadWriteByte((u8)(((WriteAddr + ptr)&0x0000FF00) >> 8));
		Spi1ReadWriteByte(WriteAddr & 0xff);
	
		for (i = 0; i < WrLen; i++)
		{
			Spi1ReadWriteByte(pBuffer[ptr++]);
		}
		W25Q_Write_Disable();
		FLASHCS_H;
		W25Q_Wait_Busy();//必须经过一个片选，取消上个状态
		
		NumByteToWrite -= WrLen;
		if(NumByteToWrite == 0)
		{//数据写完
			break;
		}
	}
	SpiAsNet();
}
/*
	读取芯片ID
*/
u16 W25Q_ReadID(void)
{
	u32 ID;
	SpiAsFlash();

	FLASHCS_L;
	Spi1ReadWriteByte(C_RDID);
	ID = Spi1ReadWriteByte(0x00);
	ID = (ID << 8) | Spi1ReadWriteByte(0x00);
	ID = (ID << 8) | Spi1ReadWriteByte(0x00);	
	FLASHCS_H;

	SpiAsNet();
	return ID;
}	
