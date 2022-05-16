#include "stm8l15x.h"
#include "pub.h"  
#include "stdio.h"
#include "flash_eeprom.h"
#include "hw_config.h" 


#define MANUALLEN	100
u8 ManualSetBuf[MANUALLEN];//记录与短信兼容指令,ASC符格式
u16 ManualSetCmdLen=0;//ASC符格式命令长度

extern u8 Intst,TstSend;;

u8 CompStr(u8 *src,u8 * dst,u8 len)
{
	u8 i;
	for(i=0;i<len;i++){
		if(!ISVALIDCHAR(*dst)) break;
		if(*src!=*dst) return 0;
		src++;
		dst++;		
	}
	return 1;
}

void AnaSetCmd(u8 *buf,u16 len)  
{
	//u8 i;
	
	
    if(CompStr(buf,"TEM=0"  ,5)){
		GetSensor();	
        SysInfo.PT100_0C=PTvolt;
        SaveSysInfo();
		printf("cal:%d  %d\r\n",SysInfo.PT100_0C,SysInfo.PT100_97C);  
		g_Temp=(s32)(PTvolt-SysInfo.PT100_0C)*970/(SysInfo.PT100_97C-SysInfo.PT100_0C);
		printf("temp=%d(0.1c)\r\n",g_Temp);
        return;
    }
    else if(CompStr(buf,"TEM=97"  ,6)){
		GetSensor();
        SysInfo.PT100_97C=PTvolt;
        SaveSysInfo();
		printf("cal:%d  %d\r\n",SysInfo.PT100_0C,SysInfo.PT100_97C);   
        return;
    }    
	else if(CompStr(buf,"ENSOR"  ,5)){
		ENSENSOR
		Intst=1;	
		printf("ok\r\n");
	}
    else if(CompStr(buf,"DISSOR"  ,6)){
		DISSENSOR			
		printf("ok\r\n");
	}
	else if(CompStr(buf,"SEND"  ,4)){
		TstSend=1;	
		WorkMode();
		printf("ok\r\n");
	}
}



void ProcPcCmd(void)
{
	
	
	if(ManualSetCmdLen){
		AnaSetCmd( ManualSetBuf,ManualSetCmdLen);
		ManualSetCmdLen=0;
	}	
				
	
	
}

void pccmdIsr(u8 dat)
{
	static u16 ptr=0;
	static u8 last;
	static u16 LastStamp=0;
	if(SysTick-LastStamp>2){ 
	 	ptr=0;
	}
	LastStamp=SysTick;
	if(ManualSetCmdLen==0){
		if(ptr<MANUALLEN){
			ManualSetBuf[ptr++]=dat;//若超出缓冲则忽略
		}
		if(last==0x0d&&dat==0x0a){
			if(ptr>2)			
				ManualSetCmdLen=ptr;
			ptr=0;
		}
		last=dat;
	}
}




