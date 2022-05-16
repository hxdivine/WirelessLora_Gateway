#include "stm8l15x.h"
#include "hw_config.h" 
#include "SX127X_Driver.h"
#include "pub.h"  
#include "flash_eeprom.h"

#define BUFLEN 300
extern u8 RecU1[BUFLEN];
extern u8	RecLen;



u8 InitLora(void)
{
   
    G_LoRaConfig.BandWidth = BW125KHZ;    //BW = 125KHz  BW125KHZ
    G_LoRaConfig.SpreadingFactor = SF09;  //SF = 9
    G_LoRaConfig.CodingRate = CR_4_6;     //CR = 4/6
    G_LoRaConfig.PowerCfig = 15;          //19±1dBm
    G_LoRaConfig.MaxPowerOn = TRUE;       //最大功率开启
    G_LoRaConfig.CRCON = TRUE;            //CRC校验开启
    G_LoRaConfig.ExplicitHeaderOn = TRUE; //Header开启
    G_LoRaConfig.PayloadLength = 10;      //数据包长度
    printf("initLora...");
    if(SX127X_Lora_init() != NORMAL){	 //无线模块初始化
        SX127X_StartRx();
        return 1;
    }
    printf("err");
    return 0;
}
void WaitSendOk(void)
{
    u8 i;
    u8 flag;
    for(i=0;i<20;i++){
        SX127X_Read(REG_LR_IRQFLAGS, &flag);
        if(flag){
            
            if(flag & RFLR_IRQFLAGS_TXDONE)
            {
                SX127X_StartRx();//发送完成转接收
                //communication_states = TX_DONE;
            }
        }
        delayTick(1);
    }
}
u8 r1,r2,r3;
void ProcLora(void)
{
    u8 flag;
    SX127X_Read(REG_LR_IRQFLAGS, &flag);
    if(flag){
        
        if(flag & RFLR_IRQFLAGS_TXDONE)
        {
            SX127X_StartRx();//发送完成转接收
            //communication_states = TX_DONE;
        }
        if(flag & RFLR_IRQFLAGS_RXDONE)
        {
            CommCnt=100;			
            SX127X_Read(REG_LR_FIFOADDRPTR, &r1);
            SX127X_Read(REG_LR_FIFORXBASEADDR, &r2);
            SX127X_Read(REG_LR_FIFORXCURRENTADDR, &r3);
            SX127X_RxPacket(RecU1);//长度在G_LoRaConfig.PayloadLength;
            RecLen=G_LoRaConfig.PayloadLength;
            
           // communication_states = RX_DONE;
        }
        SX127X_Write(REG_LR_IRQFLAGS, 0xff); //clear flags
        SX127X_Read(REG_LR_IRQFLAGS, &flag);
        if(flag){ 
            InitLora();
        }
            
    }
}
void SendLoraPack(u8 *buf,u8 len)
{
	G_LoRaConfig.PayloadLength=len;
    printf("SendPack\r\n");
	SX127X_TxPacket(buf);
}
