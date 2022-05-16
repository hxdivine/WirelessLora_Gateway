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

u8 RecU1[BUFLEN];
u8 SendU1[sizeof(PACKHEAD)+sizeof(WTSYSINFO)+PACKLEN+1];

u8	RecPtr1;

u16 LastSendStamp=0;
u8 PackSeq,AckOk=0;


u8 RecLen=0;


void SendPackDat(u8 IsAlarm)
{
    u8 i,sum=0;
    PACKHEAD *head=(PACKHEAD *)&SendU1;
    head->len=sizeof(PACKHEAD)+PACKLEN+1;
    head->src=SysInfo.id;
    head->tar=0;
    
    if(PackSeq>250) PackSeq=0;
    head->PackSeq=PackSeq;
    PackSeq++;
    SendU1[sizeof(PACKHEAD)+0] =g_Temp>>8;
    SendU1[sizeof(PACKHEAD)+1] =g_Temp&0xff;
    SendU1[sizeof(PACKHEAD)+2] =g_humid>>8;
    SendU1[sizeof(PACKHEAD)+3] =g_humid&0xff; 
    SendU1[sizeof(PACKHEAD)+4] =BatVal;
    if(IsAlarm)
        SendU1[sizeof(PACKHEAD)+4] |=0x80;

    for(i=0;i<sizeof(PACKHEAD)+PACKLEN;i++){
		sum^=SendU1[i];		
    }
    SendU1[sizeof(PACKHEAD)+PACKLEN] =sum;
    SendLoraPack(SendU1,sizeof(PACKHEAD)+PACKLEN+1);
    CommCnt=200;
}
void SendPackSet(u8 seq)//返回设置项
{
    u8 i,sum=0;
    u8 *ptr=(u8*)&SysInfo;
    PACKHEAD *head=(PACKHEAD *)&SendU1;
    head->len=sizeof(PACKHEAD)+sizeof(SysInfo)+1;
    head->src=SysInfo.id;
    head->tar=0;
    head->PackSeq=seq;
    
    for(i=0;i<sizeof(WTSYSINFO);i++){
        SendU1[sizeof(PACKHEAD)+i] =*ptr;
        ptr++;
    }    
  
     for(i=0;i<sizeof(PACKHEAD)+sizeof(SysInfo);i++){
		sum^=SendU1[i]; 
    }     
    SendU1[i]=sum;  
    SendLoraPack(SendU1,sizeof(PACKHEAD)+sizeof(SysInfo)+1);
    CommCnt=200;
}


void ProcRfCmd(void)
{

	u8 i,sum=0;
    u16 back1,back2;
    PACKHEAD *Rechead=(PACKHEAD *)&RecU1;
	if(RecLen ){//
        printf("RecaPack:");
		for(i=0;i<RecLen;i++){
			sum^=RecU1[i];
		}
		if(sum==0){//校验通过
            
			if(Rechead->src==0){
				if(SysInfo.id==Rechead->tar||Rechead->tar==0xff){//是本机地址
                    if(InSetMode&&((Rechead->PackSeq==255)||(Rechead->PackSeq==254))){
                        if(Rechead->PackSeq==255){//是设置包
                           if(RecLen==sizeof(PACKHEAD)+sizeof(WTSYSINFO)+1){                            
                                u8 *ptr=(u8*)&SysInfo;
                                back1=SysInfo.PT100_0C;
                                back2=SysInfo.PT100_97C;
                                for(i=0;i<sizeof(WTSYSINFO);i++){//stm8S IAR是大端，高位在前
                                    *ptr=RecU1[sizeof(PACKHEAD)+i];//上位机调整为大端
                                    ptr++;
                                }
                                LastSendStamp=0;//不需重发
                                SysInfo.PT100_0C=back1;
                                SysInfo.PT100_97C=back2;
                                SaveSysInfo();                           
                                InSetMode=0;
                           }
                        }
                        SendPackSet(Rechead->PackSeq);
                        WaitSendOk();
                    }
                    else{
                       
                            printf("AckOk\r\n");
                            AckOk=1;
						
                    }
				}
			}
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


