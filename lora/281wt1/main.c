

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



u16 CommCnt=0,AlarmCnt;
u8 InSetMode;

void SetLowPwr(u8 mode);


void CheckSet(void)
{

    SetFreq(0);
    InSetMode=1;
    while(Sys1msCnt<1000*10){
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



int main(void)
{


    SetSystem();
    ReadSysInfo();
    
 
    Uart_Init(SysInfo.baud);//38400

    
    InitLora();        
    CheckSet();    


	while(1){	        
        ProcLora();
        ProcRfCmd();
        ProcPcCmd();
        ClrWdg; 
        
	}	
}






/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/
