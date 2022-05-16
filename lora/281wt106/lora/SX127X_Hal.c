#include "stm8l15x.h"
#include <stdbool.h>
#include "SX127X_Hal.h"
#include "hw_config.h" 



u8 SPI_RW(u8 Addr)
{
  
    while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) == RESET);
	SPI_SendData(SPI1,Addr );
    while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE) == RESET);
    return SPI_ReceiveData(SPI1 );
    
}

void SX127X_Init()
{
    
    CLK_PeripheralClockConfig( CLK_Peripheral_SPI1, ENABLE);

    SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2,
        SPI_Mode_Master,SPI_CPOL_Low, SPI_CPHA_1Edge,
        SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft,07 );
  
    SPI_Cmd( SPI1,ENABLE );

}

//-----------------------SX127X Read and Write-------------------//
//�ò��ֺ���ΪMCU��SX127Xģ��Ĵ������ж�д
//--------------------------------------------------------------//

/**
  * @��飺SX127X  ��Ĵ�����ַ������������
  * @������uint8_t addr,�Ĵ�����ַ uint8_t *buffer,��������ָ�� uint8_t sizeָ�볤��
  * @����ֵ����
  */
unsigned char SX127X_ReadWriteByte(unsigned char data)
{
    
    return SPI_RW(data);
    
}
/**
  * @��飺SX127X  ��Ĵ�����ַ������������
  * @������uint8_t addr,�Ĵ�����ַ uint8_t *buffer,��������ָ�� uint8_t sizeָ�볤��
  * @����ֵ����
  */
void SX127X_WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    LORACS_L
    SPI_RW(addr | 0x80);
    for( i = 0; i < size; i++ )
    {
        SPI_RW(buffer[i]);
    }
    LORACS_H
}
/**
  * @��飺SX127X  ��Ĵ�����ַ����������
  * @������uint8_t addr,�Ĵ�����ַ uint8_t *buffer,��������ָ�� uint8_t sizeָ�볤��
  * @����ֵ�����ݷ��ص�*buffer��
  */
void SX127X_ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    LORACS_L;
    SX127X_ReadWriteByte(addr & 0x7F);
    for( i = 0; i < size; i++ )
    {
        buffer[i] = SX127X_ReadWriteByte(0x00);
    }

    LORACS_H;
}




