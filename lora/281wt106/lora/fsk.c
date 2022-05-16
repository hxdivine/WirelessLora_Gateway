

/***********************************************FSK********************************************************************/

/**
  * @��飺SX127X  FSK��ʼ��
  * @��������
  * @����ֵ����
  */
unsigned char SX127x_Fsk_init(void)
{
    SX127X_InitIo();                // PAIO�ڳ�ʼ��
    SX127X_Reset();                 //��λRF
    SX127X_SPI_Init();              //SPI��ʼ��

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
  * @��飺SX127X  дFSK���ñ�
  * @��������
  * @����ֵ����
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
  * @��飺SX127X  ����FSK��standbyģʽ
  * @��������
  * @����ֵ����
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
  * @��飺SX127X  ����FSK��Sleepģʽ
  * @��������
  * @����ֵ����
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
  * @��飺SX127X  FSKģʽ����FIFO
  * @��������
  * @����ֵ����
  */
void LSD_RF_ClearFIFO(void)
{
    SX127X_Write(REG_FSK_IRQFLAGS2, 0x10);
}
/**
  * @��飺SX127X FSK�������ݰ�
  * @������txBuffer�������ݴ洢���ݵ��׵�ַ��size���ݰ�����
  * @����ֵ����
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
  * @��飺SX127X  ����FSK��RXģʽ
  * @��������
  * @����ֵ����
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
  * @��飺SX127X  FSK��ȡ���ݰ�
  * @������cRxBuf�������ݴ洢���ݵ��׵�ַ��c���ݰ�����
  * @����ֵ����
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
