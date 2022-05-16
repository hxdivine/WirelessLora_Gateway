
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "def.h"

#define FLASHCS_H	gpio_bit_set(GPIOB,GPIO_PIN_0);
#define FLASHCS_L	gpio_bit_reset(GPIOB,GPIO_PIN_0);

#define C_WREN			0x06	//Ð´Ê¹ÄÜ
#define C_WRDI			0x04 	//½ûÖ¹Ð´
#define C_RDID			0x9f	//¶ÁID
#define C_RDSR			0x05 	//¶Á×´Ì¬
#define C_WRSR			0x01	//Ð´×´Ì¬
#define C_READ			0x03	//¶ÁÊý¾Ý
#define C_FASTREAD	0x0b	//¿ì¶Á
#define C_SERA			0x20	//²ÁÉÈÇø
#define C_CERA			0x60	//²ÁÐ¾Æ¬
#define C_PP				0x02	//Ð´

//³õÊ¼»¯
void W25Q_Init(void);
//¶ÁFlashId
u16 W25Q_ReadID(void);

//Ð´Ê¹ÄÜ
void W25Q_Write_Enable(void); 
//Ð´±£»¤
void W25Q_Write_Disable(void);		
//¶Áflash
void W25Q_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead); 
//Ð´flash
void W25Q_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
//ÕûÆ¬²Á³ý
void W25Q_Erase_Chip(void);   
////ÉÈÇø²Á³ý
void W25Q_Erase_Sector(u32 Dst_Addr);	
//µÈ´ý¿ÕÏÐ
void W25Q_Wait_Busy(void);   

#endif
