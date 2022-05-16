
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "def.h"

#define FLASHCS_H	gpio_bit_set(GPIOB,GPIO_PIN_0);
#define FLASHCS_L	gpio_bit_reset(GPIOB,GPIO_PIN_0);

#define C_WREN			0x06	//дʹ��
#define C_WRDI			0x04 	//��ֹд
#define C_RDID			0x9f	//��ID
#define C_RDSR			0x05 	//��״̬
#define C_WRSR			0x01	//д״̬
#define C_READ			0x03	//������
#define C_FASTREAD	0x0b	//���
#define C_SERA			0x20	//������
#define C_CERA			0x60	//��оƬ
#define C_PP				0x02	//д

//��ʼ��
void W25Q_Init(void);
//��FlashId
u16 W25Q_ReadID(void);

//дʹ��
void W25Q_Write_Enable(void); 
//д����
void W25Q_Write_Disable(void);		
//��flash
void W25Q_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead); 
//дflash
void W25Q_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
//��Ƭ����
void W25Q_Erase_Chip(void);   
////��������
void W25Q_Erase_Sector(u32 Dst_Addr);	
//�ȴ�����
void W25Q_Wait_Busy(void);   

#endif
