#include "sx127x_Drive.h"
#include "sx127x_Hal.h"
#include "gpio.h"
#include "stdio.h"
#include "SysConfig.h"
#include "lora.h"

S_LoRaConfig G_LoRaConfig;
S_LoRaPara G_LoRaPara;
float G_TsXms = 1.024;//1.024ms


/*******************************************FSK End*********************************************************************/
/*
  spi ?? ??
*/
void SX127X_Reset(void)
{
  LORARST_L
  for(u8 i = 0; i < 100; i++);
  LORARST_H
  for(u8 i = 0; i < 100; i++);
}


/*
  ????
  ???? 401 - 510 MHz
  ???? 416MHz,448MHz,450MHz,480MHz 485MHZ
  ????? 440MHz 500MHz 510MHz
*/
u8 SX127X_SetFreq(u8 ch)
{
  uint8_t test_FRFMSB = 0, test_FRFMID = 0, test_FRFLSB = 0;
  uint32_t freq_reg = (uint32_t)(SX127X_GetFreq(ch) / FREQ_STEP);
  
  printf("SetLoraCh=%d\r\n",ch);	
  
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
  ????
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
 SX127X??? ????????
*/
u8 SX127X_Lora_init(void)
{
  uint8_t test = 0;
  printf("initLora...");
          
  SX127X_Reset();              
  
  SX127X_Write( REG_LR_OPMODE, RFLR_OPMODE_SLEEP );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  
  //SPI ??
  SX127X_Write( REG_LR_HOPPERIOD, 0x91 ); //??????2?????'??÷à'????
  SX127X_Read( REG_LR_HOPPERIOD, &test);
  if(test != 0x91)
  {
    printf("LoraErr\r\n");
    return 0;
  }
  SX127X_Write( REG_LR_DIOMAPPING2, RFLR_DIOMAPPING2_DIO4_01);
  SX127X_Write(REG_LR_LNA, 0x23);
  //????
  //	SX127X_SetFreq(SysInfo.freq);
  	SX127X_SetFreq(DevInfo.workFreq);
  //????
  SX127X_PoutSet();
  SX127X_Write( REG_LR_PARAMP, RFLR_PARAMP_0100_US);

  SX127X_Write( REG_LR_OCP, 0x20 | RFLR_OCP_TRIM_240_MA); //?á÷1y?????Over Current Protection
  SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
  //?òa???Tí·????¨Implicit Header??ê±???D?ìá????¨o???????PL
  //BW??CR??HeaderóD?T??3????
  SX127X_Write( REG_LR_MODEMCONFIG1, \
    (((uint8_t)G_LoRaConfig.BandWidth) | ((uint8_t)G_LoRaConfig.CodingRate)) | (\
      (TRUE == G_LoRaConfig.ExplicitHeaderOn) ? 0x00 : 0x01));
  
  //SF CRC???
  SX127X_Write( REG_LR_MODEMCONFIG2, \
    ((uint8_t)G_LoRaConfig.SpreadingFactor) | (\
      (TRUE == G_LoRaConfig.CRCON) ? 0x04 : 0x00));
  if(SF06 == G_LoRaConfig.SpreadingFactor) { //é÷?SF = 6??Dèòa?????oüì?êa
    uint8_t temp = 0;
    SX127X_Read( 0x31, &temp);
    SX127X_Write(  0x31, (temp & 0xF8) | 0x05);
    SX127X_Write(  0x37, 0x0C);
  }
  
  //??????????? AutoAGC????
  SX127X_Write( REG_LR_MODEMCONFIG3, ((G_TsXms > 16.0f) ? \
        RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON : RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF\
        ) | RFLR_MODEMCONFIG3_AGCAUTO_ON);
  printf("LoraOk\r\n");
  return 1;
}

/*
 LSDRF?????
*/
void SX127X_TxPacket(uint8_t* data)
{
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  SX127X_Write( REG_LR_PREAMBLEMSB, 0);
  SX127X_Write( REG_LR_PREAMBLELSB, 10);
  
//  SX127X_Write(REG_LR_DIOMAPPING2,0x41);
  
  SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
  

  SX127X_Write( REG_LR_FIFOADDRPTR, 0x80);
  SX127X_WriteBuffer(REG_LR_FIFO, data, G_LoRaConfig.PayloadLength);
  SX127X_Write(REG_LR_IRQFLAGS, 0xff);
  SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
  //    SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER );
}

/*
 LSDRF??????
*/
void SX127X_StartRx(void)
{
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  SX127X_Write( REG_LR_PREAMBLEMSB, 0);
  SX127X_Write( REG_LR_PREAMBLELSB, 10);
  SX127X_Write( REG_LR_PAYLOADLENGTH, G_LoRaConfig.PayloadLength);
  
  //SX127XWriteRxTx(FALSE);//LSD4RF-2F717N10??°LSD4RF-2F717N01ê±Dèòa????????D????
  
  SX127X_Write( REG_LR_FIFOADDRPTR, 0x00);
  SX127X_Write(REG_LR_IRQFLAGS, 0xff);
  SX127X_Write( REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_RXDONE));
  SX127X_Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00 );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER );
}

/*
 LSDRF?????
*/
void SX127X_RxPacket(uint8_t* cbuf)
{
  unsigned char Reg_PKTRSSI, Reg_PKTSNR;
  u8 addr;
  SX127X_Read( REG_LR_PKTSNRVALUE, &Reg_PKTSNR);
  if((Reg_PKTSNR & 0x80) != 0)
  {
    Reg_PKTSNR = ((~Reg_PKTSNR + 1) & 0xff) >> 2;
    //SNR???? ??
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
    //???????????,????G_LoRaConfig??FIFO
    SX127X_Read(REG_LR_NBRXBYTES, &G_LoRaConfig.PayloadLength);
    SX127X_Read(REG_LR_FIFORXCURRENTADDR, &addr);
    SX127X_Write( REG_LR_FIFOADDRPTR, addr);
  }
  SX127X_ReadFifo(cbuf, G_LoRaConfig.PayloadLength);
  SX127X_Write(REG_LR_IRQFLAGS, 0xff);
}

/*
  ???????RSSI?
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
 SX127X??LORA????
*/
void SX127X_SleepMode(void)
{
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP );
}
/*
 SX127X ??????
*/
void SX127X_StandbyMode(void)
{
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
}
/*
  SX127X????
  ?? ????
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
 LSD4RF?? ????????1?????
*/
void SX127X_Write( uint8_t addr, uint8_t data )
{
  SX127X_WriteBuffer(addr,&data,1);
}	
/*
 LSD4RF?? ?????????????
*/
void SX127X_Read( uint8_t addr, uint8_t *data )
{
  SX127X_ReadBuffer(addr,data,1);
}


/*
 LSD4RF???FIFO???
*/
void SX127X_WriteFifo( uint8_t *buffer, uint8_t size )
{
  SX127X_WriteBuffer(0,buffer,size);
}
/*
 LSD4RF ?FIFO???
*/
void SX127X_ReadFifo( uint8_t *buffer, uint8_t size )
{
 SX127X_ReadBuffer(0,buffer,size);
}
/*
 SX127X??CAD???
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
 WOR???
*/
void SX127X_WORInit(void)
{
  SX127X_CADinit();
}
/*
 SX127X ??CAD ???????? ?????(2^SF + 32) / BW
*/
void SX127X_CAD_Sample(void)
{
  //SX127XWriteRxTx(FALSE);     //set RF switch to RX path
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY );
  SX127X_Write( REG_LR_OPMODE, 0x80 | RFLR_OPMODE_CAD );
}
/*
 ???WOR??
*/
void SX127X_WOR_Execute(uint8_t cclen)
{
  switch(cclen)
  {
  case 0:   //????
    {
      SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
      SX127X_SleepMode();      //??????
    }
    break;
  case 1:   //??CAD????
    {
      SX127X_CAD_Sample();     //??CAD??
    }
    break;
  default:
    break;
  }
}
/*
  WOR?RX
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
 SX127X ?????
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
