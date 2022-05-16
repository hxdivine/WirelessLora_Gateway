#include "stm8l15x.h"
#include "hw_config.h" 
#include "pub.h"  
#include "stdio.h"
#include "flash_eeprom.h"
#include "rf433.h"
#include "lora\lora.h"

#define BUFLEN 250

#define PACKLEN  5           

void WaitSendOk(void);
void PutinUart1(u8 dat);


u8 RecU1[BUFLEN];
u8 SendU1[BUFLEN];




u8 PackSeq;
u8 MbSrc=0;
u8 RecLen=0;
u16 MbSrcStamp;



void SendPackSet(u8 seq)//返回设置项
{
    u8 i,sum=0;

    PACKHEAD *head=(PACKHEAD *)&SendU1;
    head->len=sizeof(PACKHEAD)+sizeof(SysInfo)+1;
    head->src=101;
    head->tar=0;
    head->PackSeq=seq;
    
    
    SendU1[sizeof(PACKHEAD)] =SysInfo.baud;
    SendU1[sizeof(PACKHEAD)+1] =SysInfo.freq;   
  
     for(i=0;i<sizeof(PACKHEAD)+sizeof(SysInfo);i++){
		sum^=SendU1[i]; 
    }     
    SendU1[i]=sum;  
    SendLoraPack(SendU1,sizeof(PACKHEAD)+sizeof(SysInfo)+1);
    CommCnt=200;
    
}

void SendPackDat(u8 *buf,u8 len)//
{
    u8 i,sum=0;

    PACKHEAD *head=(PACKHEAD *)&SendU1;
    head->len=sizeof(PACKHEAD)+len+1;
    head->src=101;
    head->tar=0;
    head->PackSeq=MbSrc;
    sum=head->len^head->src^head->tar^head->PackSeq;
    for(i=0;i<len;i++){
        SendU1[sizeof(PACKHEAD)+i] =buf[i];
        sum^=buf[i]; 
    }
  
      
    SendU1[sizeof(PACKHEAD)+i]=sum;  
    SendLoraPack(SendU1,sizeof(PACKHEAD)+len+1);
    CommCnt=200;
    MbSrc=0;

}
void ProcRfCmd(void)
{

	u8 i,sum=0;
    PACKHEAD *Rechead=(PACKHEAD *)&RecU1;
    if(MbSrc){
        if(Sys1msCnt-MbSrcStamp>2000){
                MbSrc=0;
        }      
        
    }
	if(RecLen ){//
        printf("RecaPack:");
		for(i=0;i<RecLen;i++){
			sum^=RecU1[i];
		}
		if(sum==0){//校验通过
            
			//if(Rechead->src==0){
				//if(101==Rechead->tar||Rechead->tar==0xff){//是本机地址
                    if(InSetMode&&((Rechead->PackSeq==255)||(Rechead->PackSeq==254))){
                        if(Rechead->PackSeq==255){//是设置包
                           if(RecLen==sizeof(PACKHEAD)+sizeof(WTSYSINFO)+1){                            
                                SysInfo.baud=RecU1[4];
                                SysInfo.freq=RecU1[5];
                               
                                SaveSysInfo();                           
                                InSetMode=0;
                           }
                        }
                        SendPackSet(Rechead->PackSeq);
                        WaitSendOk();
                    }
                    else{
                        MbSrc=Rechead->PackSeq;
                        MbSrcStamp=Sys1msCnt;
                        EN485;
                        for(i=0;i<RecLen-sizeof(PACKHEAD)-1;i++){
                            PutinUart1(RecU1[sizeof(PACKHEAD)+i]);
                        }
                       
                    }
				//}
			//}
		}
		RecLen=0;
	}	
}

void uart1Isr(u8 dat)
{	
   
}


void SendRf433Raw(u8 *buf,u8 len)//用于透传
{
	u8 i;

	for(i=0;i<len;i++){		
    	UART1_SendByte(buf[i]);
    }    
}


