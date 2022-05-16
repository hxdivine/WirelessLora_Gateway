

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "hw_config.h" 
#include "pub.h"
#include "intrinsics.h"
#include "flash_eeprom.h"
#include "stdio.h"
#include "rf433.h"
#include "stdio.h"
#include "lora\SX127X_Driver.h"
#include "lora\lora.h"

#define TICKPERMIN  16*60
//#define TICKPERMIN  16*6

u8 BatVal,TstSend=0;
u8 Intst=0;
u16 CommCnt=0,AlarmCnt;
u8 InSetMode;

u8 IsGap;//空隙计数

u16 GapMsTime=0;//收到主机数据后的ms计数
u8 GapCnt=0;//当前为第一个空隙gap

u8 AinState[2];//0常态，1报警，2报警成功


void SetLowPwr(u8 mode);

s16 g_Temp;
s16 g_humid;

void CheckSet(void)
{

    SetFreq(0);
    InSetMode=1;
    while(SysTick<16*10){
        ClrWdg
        ProcLora();    
        ProcPcCmd();
        ProcRfCmd();
        SetLowPwr(0);
        if(InSetMode==0){
            LedCnt=20;
            break;
        }
    }
    printf("SetTimeOver\r\n");        
    InSetMode=0;   
    WORKLEDOFF
    SetFreq(SysInfo.freq);
}
void GetSensor(void)
{

    ENSENSOR
    delayTick(3);
    StartAdc();
    g_Temp=((s32)PTvolt-SysInfo.PT100_0C)*970/(SysInfo.PT100_97C-SysInfo.PT100_0C);
    printf("temp=%d(0.1c)\r\n",g_Temp);
    
    if(BatVal<60)
        DISSENSOR  
}



void WaitTxdOver(void)
{
    while(1){
        if(TxSav1 ==0) break;
        SetLowPwr(0);
        if(TxSav1 ==0) break;
    }
}
void CheckVal(void)
{
    u8 i;
    u8 alarm;
    GetSensor();
	//return;
    for(i=0;i<1;i++){   
        alarm=0;
        if(AinState[i]==1) 
            continue;
        if(i==0){         
            if((SysInfo.TempHigh>SysInfo.TempLow)&&(g_Temp>SysInfo.TempHigh||g_Temp<SysInfo.TempLow)){
                printf("TempAlarm\r\n");
                alarm=1;
                AlarmCnt=200;               
            }
        }
       
        
        
        if(alarm){ 
            if(AinState[i]==0)
                AinState[i]=1;
        }
        else
            if(AinState[i]==2)
                AinState[i]=0;            
    } 
    
}
void WorkMode(void)
{
    u8 i,j,alarm=0;
    static u8 MinCnt;
    static u16 BackTick;
    u8 SendMin;
#ifndef WT1   
    Uart_Init(2);//38400
#endif    
    
    CheckVal();   
    MinCnt++;
    if(BatVal<60){
       SendMin=SysInfo.TranMinute ;
    }
    else
        SendMin=1;
    if(AinState[0]==1||AinState[1]==1||SysTick-BackTick>=TICKPERMIN*SendMin||TstSend){      
		TstSend=0;
        if(AinState[0]==1||AinState[1]==1)
            alarm=1;
        LedCnt=20;
        BackTick=SysTick;
        for(i=0;i<5;i++){
            printf("TrySend%d@%d\r\n",i,SysTick);
            InitLora();
            SendPackDat(alarm); 
            WaitTxdOver();
                
            
            for(j=0;j<8;j++){//等待3秒
                ProcLora();
                delayTick(2);
                
                ProcRfCmd();
                ClrWdg;
                if(AckOk) break;
            }
            if(AckOk) {
                AckOk=0;
                break;
            }
            SX127X_SleepMode();
            delayTick((SysInfo.id%4)*2+(SysInfo.id&0x1f)*2+SysInfo.id/2+i*10);
            
        }
        for(i=0;i<1;i++){                                
            if(AinState[i]==1)AinState[i]=2;//0常态，1报警，2报警成功
        }
    
    }
       
}



void GointoStandby(void)
{
    WaitTxdOver();
 //   WORKLEDOFF
    ALARMLEDOFF
    SX127X_SleepMode();
    
    TxPtr1 =0;
	TxSav1=0;
    CLK_PeripheralClockConfig( CLK_Peripheral_SPI1, DISABLE);
    SPI_Cmd( SPI1,DISABLE );
    USART_Cmd(USART1, DISABLE);
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
    SetLowPwr(1); 
}
u16 tst;
int main(void)
{

    u16 BackTick=0;
    u16 Nowtick;
    SetSystem();
	
	//for(tst=0;tst<60000;tst++)
	//	PutinUart1(tst&0xff);
    ReadSysInfo();
    GetSensor();
	
	printf("ref=%d,",AdcVref);
    printf("Adc1=%d,",AdcVal1);//pT电压
    printf("Adc2=%d,",AdcVal2);//电池分压电压
	
    InitLora();      
    
    CheckSet();    
 //   SysInfo.TranMinute=1;
    printf("Id=%d\r\n",SysInfo.id);  //设备通讯编号1-100	
	printf("TranMinute=%d\r\n",SysInfo.TranMinute);//上发间隔时间 分为单位	取值为5,10,30,60,240
    printf("Temp:%d->%d\r\n",SysInfo.TempLow,SysInfo.TempHigh);
    printf("cal:%d  %d\r\n",SysInfo.PT100_0C,SysInfo.PT100_97C);   
    printf("freq=%d\r\nbegin......",SysInfo.freq);   
    GetSensor();
    
    WaitTxdOver();
    
    
    if(BatVal<60){
        GointoStandby(); 
    }
    
	while(1){	
        Nowtick=SysTick;
        if(BatVal<60){
            if(Nowtick-BackTick>=TICKPERMIN){
                BackTick=Nowtick-(Nowtick-BackTick-TICKPERMIN);
                WorkMode();                
                GointoStandby();
            }
            ClrWdg;
            SetLowPwr(1); 
        }
        else {
            if(Nowtick-BackTick>=TICKPERMIN/60&&Intst==0){
				while(Nowtick-BackTick>=TICKPERMIN/60)//持续直到只剩一
                	BackTick=Nowtick-(Nowtick-BackTick-TICKPERMIN/60);
                WorkMode();
            }
            ClrWdg;
            ProcPcCmd();
        }
	}	
}






/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/
