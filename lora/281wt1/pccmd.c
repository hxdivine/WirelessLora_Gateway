#include "stm8l15x.h"
#include "pub.h"  
#include "stdio.h"
#include "hw_config.h" 
#define MANUALLEN	200
void SendPackDat(u8 *buf,u8 len);

u8 RecBuf[MANUALLEN];//记录与短信兼容指令,ASC符格式

u16 ptr=0;
u16 LastRecStamp=0;

//const u32 BAUDMODE[]={115200,57600,38400,19200,9600,4800,2400,1200};
const  u8 UARTFREE[]= { 3,      3   ,3      ,3, 4, 6,10,20};
void ProcPcCmd(void)
{
	
	
	if(ptr>0&&Sys1msCnt-LastRecStamp>UARTFREE[SysInfo.baud]){			
        SendPackDat(RecBuf,ptr);
        ptr=0;
	}	
				
	
	
}

void pccmdIsr(u8 dat)
{
	
	LastRecStamp=Sys1msCnt;	
	if(ptr<MANUALLEN){
		RecBuf[ptr++]=dat;//若超出缓冲则忽略
	}
		
}




