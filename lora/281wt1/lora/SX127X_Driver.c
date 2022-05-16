/**
  ******************************************************************************
  * 文件名 ：   SX127X_Driver.c
  * 作者   ：   LSD RF Team
  * 版本   ：   V1.0.0
  * 时间   ：   15-Aug-2018
  * 文件描述：
  *     该文件为SX127X模块的驱动层，包含对SX127X模块的工作模式控制，FIFO操作，发
  *射功率、频率等设置，以及一些数据处理的公共函数
  *    客户在使用SX127X模块时候需要移植该文件，如果SX127X_HAL.c中各个函数功能都已
  *实现且函数名等都没有改动，那本文件可以不用做更改直接使用，客户在应用层直接调
  *用本文件函数就可以实现各种操作。
*******************************************************************************/
#include "stm8l15x.h"
#include "hw_config.h" 
#include "SX127X_Driver.h"
#include "flash_eeprom.h"
#include "stdio.h"

void SX127X_Init();


float G_TsXms = 1.024;//1.024ms

S_LoRaConfig G_LoRaConfig ;
S_LoRaPara G_LoRaPara;


void SX127X_Write( uint8_t addr, uint8_t data )
{
    SX127X_WriteBuffer( addr, &data, 1 );
}
/**
  * @简介：RF  向寄存器地址读1字节数据
  * @参数：uint8_t addr,寄存器地址 uint8_t *data读数据存储地址
  * @返回值：无
  */
void SX127X_Read( uint8_t addr, uint8_t *data )
{
    SX127X_ReadBuffer( addr, data, 1 );
}
/**
  * @简介：SX127X  向FIFO写数据
  * @参数：uint8_t *buffer,数组指针 uint8_t size长度
  * @返回值：无
  */
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_WriteBuffer( 0, buffer, size );
}
/**
  * @简介：SX127X  向FIFO读数据
  * @参数：uint8_t *buffer,数组指针 uint8_t size长度
  * @返回值：uint8_t *buffer 存储读取内容
  */
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX127X_ReadBuffer( 0, buffer, size );
}

//-------------------------SX127X 基本设置-----------------------//
//该部分函数为对SX127X模块复位、发射功率、发射频率等参数设置，以
//及SX127X工作模式设置、数据包读写等
//--------------------------------------------------------------//

/**
  * @简介：SX127X  复位设置
  * @参数：无
  * @返回值：无
  */
void SX127X_Reset(void)
{
    u8 i;
    LORARST_L
    for(i=0;i<100;i++);
    LORARST_H
    for(i=0;i<100;i++);
}



//401-510：禁用频点416MHz、448MHz、450MHz、480MHz、485MHz、。
//不建议频点：440MHz、500MHz、510MHz
u32 GetFreq(u8 ch)//ch=0-99
{
	if(ch==14)ch=100;
	else if(ch==15)ch=101;
	
	else if(ch==46)ch=102;
	else if(ch==47)ch=103;
	
	else if(ch==48)ch=104;
	else if(ch==49)ch=105;
	
	else if(ch==78)ch=106;
	else if(ch==79)ch=107;
	
	else if(ch==83||ch==84)ch=108;	
	
	return (u32)(401000000+1000000*ch+365000);
}
u8 SetFreq(u8 ch)
{
    //Register values must be modified only when device is in SLEEP or STAND-BY mode.
   
    uint32_t freq_reg = (uint32_t)(GetFreq(ch) / FREQ_STEP);
    uint8_t test_FRFMSB = 0, test_FRFMID = 0, test_FRFLSB = 0;
    printf("SetFreq=%d\r\n",ch);
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_FRFMSB, (uint8_t)(freq_reg >> 16)); 
    SX127X_Write( REG_LR_FRFMID, (uint8_t)(freq_reg >> 8) );
    SX127X_Write( REG_LR_FRFLSB, (uint8_t)(freq_reg) );

    SX127X_Read(REG_LR_FRFMSB, &test_FRFMSB);
    SX127X_Read(REG_LR_FRFMID, &test_FRFMID);
    SX127X_Read(REG_LR_FRFLSB, &test_FRFLSB);
SX127X_StartRx();
    if(test_FRFMSB != (uint8_t)(freq_reg >> 16))
        return SPI_READCHECK_WRONG;
    if(test_FRFMID != (uint8_t)(freq_reg >> 8))
        return SPI_READCHECK_WRONG;
    if(test_FRFLSB != (uint8_t)(freq_reg))
        return SPI_READCHECK_WRONG;
    return 1;
}
/**
* @简介:SX127X初始化
  * @参数：无
  * @返回值：tSX127xError   错误枚举内容
  */
u8 SX127X_Lora_init(void)
{

    SX127X_Init();              
    SX127X_Reset();              
    

    SX127X_Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );

    //SPI 验证
    uint8_t test = 0;
    SX127X_Write( REG_LR_HOPPERIOD, 0x91 ); //选一个用不到的寄存器来做验证
    SX127X_Read( REG_LR_HOPPERIOD, &test);
    if(test != 0x91)
        return 0;
    SX127X_Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_01);
    SX127X_Write(REG_LR_LNA, 0x23);
    SetFreq(SysInfo.freq); 
    SX127X_PoutSet();
    SX127X_Write( REG_LR_PARAMP, RFLR_PARAMP_0100_US);
    //  ↑PA Ramp的时间，如果用户LDO不能快速输出大电流（泵能力），适当增加PA Ramp时间
    //  ↑如果Ramp时间过短超过了LDO的能力时，会出现进入TX后，系统电流为发射电流，但是RF信号不出现的现象
    SX127X_Write( REG_LR_OCP, 0x20 | RFLR_OCP_TRIM_240_MA); //电流过载保护 Over Current Protection
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
    //注意，无头模式（Implicit Header）时，必须提前规定好收发双方的PL

    //BW、CR、Header有无，初始化
    SX127X_Write( REG_LR_MODEMCONFIG1, \
                  (((uint8_t)G_LoRaConfig.BandWidth) | ((uint8_t)G_LoRaConfig.CodingRate)) | (\
                          (TRUE == G_LoRaConfig.ExplicitHeaderOn) ? 0x00 : 0x01));

    //SF、CRC初始化
    SX127X_Write( REG_LR_MODEMCONFIG2, \
                  ((uint8_t)G_LoRaConfig.SpreadingFactor) | (\
                          (TRUE == G_LoRaConfig.CRCON) ? 0x04 : 0x00));
    if(SF06 == G_LoRaConfig.SpreadingFactor) { //慎用SF = 6，需要的配置很特殊
        uint8_t temp = 0;
        SX127X_Read( 0x31, &temp);
        SX127X_Write(  0x31, (temp & 0xF8) | 0x05);
        SX127X_Write(  0x37, 0x0C);
    }

    //低速率优化功能是否开启、AutoAGC默认开启
    SX127X_Write( REG_LR_MODEMCONFIG3, ((G_TsXms > 16.0f) ? \
                                        RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON : RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF\
                                       ) | RFLR_MODEMCONFIG3_AGCAUTO_ON);
    return 1;
}
/**
  * @简介：RF发送数据包
  * @参数：uint8_t*data：发送数据指针
  * @返回值：无
  */
void SX127X_TxPacket(uint8_t*data)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

//    SX127XWriteRxTx(true);//LSD4RF-2F717N10以及LSD4RF-2F717N01时需要用到，高频切换；

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
    SX127X_WriteBuffer(REG_LR_FIFO, data, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}
/**
  * @简介：RF进入接收状态
  * @参数：无
  * @返回值：无
  */
void SX127X_StartRx(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
    SX127X_Write( REG_LR_PREAMBLEMSB, 0);
    SX127X_Write( REG_LR_PREAMBLELSB, 10);
    SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

//    SX127XWriteRxTx(false);//LSD4RF-2F717N10以及LSD4RF-2F717N01时需要用到，高频切换；

    SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
    SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}
/**
  * @简介：RF接收数据包
  * @参数：uint8_t*cbuf接收数组指针
  * @返回值：无
  */
void SX127X_RxPacket(uint8_t*cbuf)
{
    unsigned char Reg_PKTRSSI, Reg_PKTSNR;
    u8 addr;
    SX127X_Read( REG_LR_PKTSNRVALUE, &Reg_PKTSNR);

    if((Reg_PKTSNR & 0x80) != 0) {
        Reg_PKTSNR = ((~Reg_PKTSNR + 1) & 0xff) >> 2;
        //SNR前显示加“-”
        G_LoRaPara.Packet_SNR = -Reg_PKTSNR;
    }
    else
    {
        Reg_PKTSNR = (Reg_PKTSNR & 0xff) >> 2;
        G_LoRaPara.Packet_SNR = Reg_PKTSNR;
    }

    SX127X_Read( REG_LR_PKTRSSIVALUE, &Reg_PKTRSSI);

    if(G_LoRaPara.Packet_SNR < 0)
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI - G_LoRaPara.Packet_SNR;
    }
    else
    {
        G_LoRaPara.Packet_RSSI = -164 + Reg_PKTRSSI * 16 / 15;
    }

    if(TRUE == G_LoRaConfig.ExplicitHeaderOn) {
        //有头那么从寄存器中读，否则按照G_LoRaConfig设置长度读取FIFO
        SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength);
        SX127X_Read(REG_LR_FIFORXCURRENTADDR, &addr);
        SX127X_Write( REG_LR_FIFOADDRPTR, addr);
    }
    SX127X_ReadFifo(cbuf, G_LoRaConfig.PayloadLength);
    SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/**
  * @简介：获取当前信道中的RSSI值
  * @参数：无
  * @返回值：RSSI值
  */
int16_t SX127X_Current_RSSI(void)
{
    unsigned char Reg_RSSIValue;
    ///int16_t temp;
    SX127X_Read( REG_LR_RSSIVALUE, &Reg_RSSIValue);
    G_LoRaPara.Current_RSSI = -164 + Reg_RSSIValue; 
    return G_LoRaPara.Current_RSSI;
}
void SX127X_SleepMode(void)
{
   SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP ); 
}
void SX127X_StandbyMode(void)
{
    SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
}
tSX127xError SX127X_PoutSet(void)
{
	uint8_t test = 0;
    if(G_LoRaConfig.PowerCfig > 15)
        return PARAMETER_INVALID;
    SX127X_StandbyMode();
    SX127X_Write( REG_LR_PACONFIG, 0xf0 | G_LoRaConfig.PowerCfig);
    
    SX127X_Read(REG_LR_PACONFIG, &test);
    if((0xf0 | G_LoRaConfig.PowerCfig) != test)
        return SPI_READCHECK_WRONG;
    if(TRUE == G_LoRaConfig.MaxPowerOn)
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_ON );
    else
        SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_OFF );
    return NORMAL;
}