
#include "sx127x_Drive.h"
#include "sx127x_Hal.h"
#include "gd32e10x.h"
#include "spi.h"
#include "config.h"

S_LoRaConfig G_LoRaConfig;
S_LoRaPara G_LoRaPara;
float G_TsXms = 1.024;//1.024ms

/*******************************************FSK End*********************************************************************/
/*
	SPI 片选 复位
*/
void SX127X_Reset(void)
{
	LORARST_L
	for(u8 i = 0; i < 100; i++);
	LORARST_H
	for(u8 i = 0; i < 100; i++);
}


/*
 设置频点
 频点范围 401 - 510MHz
 禁用频点：416MHz,448MHz,450MHz,480MHz 485MHZ
 不建议频点： 440MHz 500MHz 510MHz
*/
u8 SX127X_SetFreq(u8 ch)
{
	uint8_t test_FRFMSB = 0, test_FRFMID = 0, test_FRFLSB = 0;
	uint32_t freq_reg = (uint32_t)(SX127X_GetFreq(ch) / FREQ_STEP);

	SX127X_StandbyMode();
	SX127X_Write( REG_LR_FRFMSB, (uint8_t)(freq_reg >> 16)); 
	SX127X_Write( REG_LR_FRFMID, (uint8_t)(freq_reg >> 8));
	SX127X_Write( REG_LR_FRFLSB, (uint8_t)(freq_reg));

	SX127X_Read(REG_LR_FRFMSB, &test_FRFMSB);
	SX127X_Read(REG_LR_FRFMID, &test_FRFMID);
	SX127X_Read(REG_LR_FRFLSB, &test_FRFLSB);
	SX127X_StartRx();
	if(test_FRFMSB != (uint8_t)(freq_reg >> 16))
	{
		return SPI_READCHECK_WRONG;
	}
	if(test_FRFMID != (uint8_t)(freq_reg >> 8))
	{
		return SPI_READCHECK_WRONG;
	}
	if(test_FRFLSB != (uint8_t)(freq_reg))
	{
		return SPI_READCHECK_WRONG;
	}
	
	return 1;
}
/*
 获取频点
*/
u32 SX127X_GetFreq(u8 ch)
{
	if(ch == 14)
	{
		ch = 100;
	}
	else if(ch == 15)
	{
		ch = 101;
	}
	else if(ch == 46)
	{
		ch = 102;
	}
	else if(ch == 47)
	{
		ch = 103;
	}
	else if(ch == 48)
	{
		ch = 104;
	}
	else if(ch == 49)
	{
		ch = 105;
	}
	else if(ch == 78)
	{
		ch = 106;
	}
	else if(ch == 79)
	{
		ch = 107;
	}
	else if(ch == 83 || ch == 84)
	{
		ch = 108;	
	}
	return 401000000 + 1000000 * ch + 365000;
}

/*
 SX127X初始化 输出 错误枚举类型
*/
u8 SX127X_Lora_init(void)
{
	uint8_t test = 0;
	           
  SX127X_Reset();              
    
	SX127X_Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );

  //SPI 验证
  SX127X_Write( REG_LR_HOPPERIOD, 0x91 ); //选一个用不到的寄存器来做验证
  SX127X_Read( REG_LR_HOPPERIOD, &test);

  if(test != 0x91)
	{
    return 0;
	}
  SX127X_Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_01);
  SX127X_Write(REG_LR_LNA, 0x23);
	//频点设置
	SX127X_SetFreq(SysInfo.Workfreq);
//  	SX127X_SetFreq(3);
	//功率设置
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

/*
 LSDRF发送数据包
*/
void SX127X_TxPacket(uint8_t* data)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_PREAMBLEMSB, 0);
	SX127X_Write( REG_LR_PREAMBLELSB, 10);
	SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

	SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
	SX127X_WriteBuffer(REG_LR_FIFO, data, G_LoRaConfig.PayloadLength);
	SX127X_Write(REG_LR_IRQFLAGS, 0xff);
	SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
	
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}

/*
 LSDRF进入接收模式
*/
void SX127X_StartRx(void)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_PREAMBLEMSB, 0);
	SX127X_Write( REG_LR_PREAMBLELSB, 10);
	SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);

	SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
	SX127X_Write(REG_LR_IRQFLAGS, 0xff);
	SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
	SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}

/*
 LSDRF接收数据包
*/
void SX127X_RxPacket(uint8_t* cbuf)
{
  unsigned char Reg_PKTRSSI, Reg_PKTSNR;
	u8 addr;
 
  SX127X_Read( REG_LR_PKTSNRVALUE, &Reg_PKTSNR);
  if((Reg_PKTSNR & 0x80) != 0)
	{
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
	if(TRUE == G_LoRaConfig.ExplicitHeaderOn) 
	{
		//有头那么从寄存器中读，否则按照G_LoRaConfig设置长度读取FIFO
		SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength);
		SX127X_Read(REG_LR_FIFORXCURRENTADDR, &addr);
		SX127X_Write( REG_LR_FIFOADDRPTR, addr);
  }
  SX127X_ReadFifo(cbuf, G_LoRaConfig.PayloadLength);
  SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}

/*
 获取当前信道中的RSSI
*/
int16_t SX127X_Current_RSSI(void)
{
	unsigned char Reg_RSSIValue;
	///int16_t temp;
	SX127X_Read( REG_LR_RSSIVALUE, &Reg_RSSIValue);
	G_LoRaPara.Current_RSSI = -164 + Reg_RSSIValue; 
	return G_LoRaPara.Current_RSSI;
}
/*
 SX127X进入LORA睡眠模式
*/
void SX127X_SleepMode(void)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
}
/*
 SX127X进入待命模式
*/
void SX127X_StandbyMode(void)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
}
/*
 SX127X配置功率
 输出 错误枚举
*/
tSX127xError SX127X_PoutSet(void)
{
	uint8_t test = 0;
	if(G_LoRaConfig.PowerCfig > 15)
	{
		return PARAMETER_INVALID;
	}
	SX127X_StandbyMode();
	SX127X_Write( REG_LR_PACONFIG, 0xf0 | G_LoRaConfig.PowerCfig);

	SX127X_Read(REG_LR_PACONFIG, &test);
	if((0xf0 | G_LoRaConfig.PowerCfig) != test)
	{
		return SPI_READCHECK_WRONG;
	}
	if(TRUE == G_LoRaConfig.MaxPowerOn)
	{
		SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_ON );
	}
	else
	{
		SX127X_Write( REG_LR_PADAC, 0x80 | RFLR_PADAC_20DBM_OFF );
	}
	return NORMAL;
}

/*
 LSD4RF模块 向寄存器地址写1个字节数据
*/
void SX127X_Write( uint8_t addr, uint8_t data )
{
	SX127X_WriteBuffer(addr,&data,1);
}	
/*
 LSD4RF模块 从寄存器读取1字节数据
*/
void SX127X_Read( uint8_t addr, uint8_t *data )
{
  SX127X_ReadBuffer(addr,data,1);
}


/*
 LSD4RF模块向FIFO写数据
*/
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size )
{
	SX127X_WriteBuffer(0,buffer,size);
}
/*
 LSD4RF模块 从FIFO读数据
*/
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size )
{
 SX127X_ReadBuffer(0,buffer,size);
}
/*
 SX127X进入 CAD 初始化
*/
void SX127X_CADinit(void)
{
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
	SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
	SX127X_Write( REG_LR_IRQFLAGSMASK, \
								~(RFLR_IRQFLAGS_CADDONE | RFLR_IRQFLAGS_CADDETECTED));
	SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_10);
//	SX127XWriteRxTx(FALSE);     //set RF switch to RX path
	SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}
/*
 WOR初始化
*/
void SX127X_WORInit(void)
{
	SX127X_CADinit();
}
/*
 SX127X 启动CAD，采样信道情况一次   采样时间约为(2^SF+32)/BW  
*/
void SX127X_CAD_Sample(void)
{
  //SX127XWriteRxTx(FALSE);     //set RF switch to RX path
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_CAD );
}
/*
 执行WOR操作
*/
void SX127X_WOR_Execute(uint8_t cclen)
{
	switch(cclen)
	{
		case 0:   //启动睡眠
		{
			SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
			SX127X_SleepMode();      //进入睡眠模式
		}
		break;
		case 1:   //进入CAD检测模式
		{
				SX127X_CAD_Sample();     //启动CAD一次
		}
		break;
		default:
				break;
	}
}
/*
 WOR到RX
*/
void SX127X_WOR_Exit(void)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_PREAMBLEMSB, 0xf0);
	SX127X_Write( REG_LR_PREAMBLELSB, 0xff);
	SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
	SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
	SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
//	SX127XWriteRxTx(FALSE);                                    
	SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
	SX127X_Write(REG_LR_IRQFLAGS, 0xff);
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}

/*
 SX127X 发送唤醒包
*/
void SX127X_Awake(uint8_t*cbuf, uint16_t Preamble_Length)
{
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
	SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
	//SX127XWriteRxTx(TRUE);
	SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
	SX127X_WriteBuffer(REG_LR_FIFO, cbuf, G_LoRaConfig.PayloadLength);
	SX127X_Write(REG_LR_IRQFLAGS, 0xff);
	SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
	SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
	SX127X_Write( REG_LR_PREAMBLEMSB, (uint8_t)(Preamble_Length >> 8)); //set preamble length
	SX127X_Write( REG_LR_PREAMBLELSB, (uint8_t)Preamble_Length); //set preamble length
	SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}
