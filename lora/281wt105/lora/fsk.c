

/***********************************************FSK********************************************************************/

/**
  * @简介：SX127X  FSK初始化
  * @参数：无
  * @返回值：无
  */
unsigned char SX127x_Fsk_init(void)
{
    SX127X_InitIo();                // PAIO口初始化
    SX127X_Reset();                 //复位RF
    SX127X_SPI_Init();              //SPI初始化

    LSD_RF_SleepMode_FSK();
    LSD_RF_StandbyMode_FSK();
    LSD_RF_Config();                    
    // to test SPI
    uint8_t test = 0;
    SX127X_Write(REG_FSK_SYNCVALUE8, 0x55);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0x55)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    SX127X_Write(REG_FSK_SYNCVALUE8, 0xD3);
    SX127X_Read(REG_FSK_SYNCVALUE8, &test);
    if(test != 0xD3)
    {
        return SPI_READCHECK_WRONG;// something wrong with SPI
    }
    return NORMAL;

}

/**
  * @简介：SX127X  写FSK配置表
  * @参数：无
  * @返回值：无
  */
void LSD_RF_Config(void)
{
    _SX12XX_REG const *p;
    unsigned char i;
    p = LSD_RFregConfig;
    for(i = sizeof(LSD_RFregConfig) / 2; i > 0; i--)
    {
        SX127X_Write(p->addr, p->val);
        p++;
    }
}
/**
  * @简介：SX127X  进入FSK的standby模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_StandbyMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_STANDBY );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_STANDBY) && nTimes);
}
/**
  * @简介：SX127X  进入FSK的Sleep模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_SleepMode_FSK(void)
{
    unsigned char cData;
    unsigned int nTimes = 65535;
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_SLEEP );  
    do
    {
        SX127X_Read(REG_FSK_OPMODE, &cData);
        nTimes--;
    }
    while(((cData & 0x07) != RFFSK_OPMODE_SLEEP) && nTimes);
}
/**
  * @简介：SX127X  FSK模式下晴FIFO
  * @参数：无
  * @返回值：无
  */
void LSD_RF_ClearFIFO(void)
{
    SX127X_Write(REG_FSK_IRQFLAGS2, 0x10);
}
/**
  * @简介：SX127X FSK发送数据包
  * @参数：txBuffer发送数据存储数据的首地址，size数据包长度
  * @返回值：无
  */
void LSD_RF_SendPacket_FSK(uint8_t *txBuffer, uint8_t size)
{
    LSD_RF_StandbyMode_FSK();
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00); 
    SX127XWriteRxTx(true);
    LSD_RF_ClearFIFO();    
    SX127X_Write(REG_FSK_FIFO, size);
    for(unsigned int i = 0; i < size; i++)
    {
        SX127X_Write(REG_FSK_FIFO, txBuffer[i]);
    }
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_TRANSMITTER );
}
/**
  * @简介：SX127X  进入FSK的RX模式
  * @参数：无
  * @返回值：无
  */
void LSD_RF_RXmode_FSK(void)
{
    LSD_RF_StandbyMode_FSK();                     
    SX127X_Write(REG_FSK_DIOMAPPING1, RFFSK_DIOMAPPING1_DIO0_00);
    LSD_RF_ClearFIFO();        
    SX127XWriteRxTx(false);
    SX127X_Write(REG_FSK_OPMODE, 0x08 | RFFSK_OPMODE_RECEIVER); 
}

/**
  * @简介：SX127X  FSK读取数据包
  * @参数：cRxBuf发送数据存储数据的首地址，c数据包长度
  * @返回值：无
  */
void LSD_RF_RxPacket_FSK(uint8_t *cRxBuf, uint8_t *cLength)
{
    uint8_t i;
    uint8_t test = 0;
    LSD_RF_StandbyMode_FSK();     
    SX127X_Read(REG_FSK_FIFO, &test);
    *cLength = test;
    for(i = 0; i < *cLength; i++)
        SX127X_Read(REG_FSK_FIFO, &cRxBuf[i]);
    LSD_RF_ClearFIFO();   

}

/***********************************************FSK End********************************************************************/
