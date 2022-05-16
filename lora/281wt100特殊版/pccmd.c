#include "stm8l15x.h"
#include "pub.h"  
#include "stdio.h"
#include "hw_config.h" 
#include "flash_eeprom.h"


#define MANUALLEN	200
void SendPackDat(u8 *buf,u8 len);
void PutinUart1(u8 dat);



u8 RecBuf[MANUALLEN];//记录与短信兼容指令,ASC符格式

u16 ptr=0;
u16 LastRecStamp=0;

//const u32 BAUDMODE[]={115200,57600,38400,19200,9600,4800,2400,1200};
const  u8 UARTFREE[]= { 3,      3   ,3      ,3, 4, 6,10,20};
u8 ChangeStr_u8(u8 *str,u8 *dat)
{
	u8 i;
	*dat=0;
	for(i=0;i<3;i++){
		if(!ISNUM(str[i])) break;
		*dat=*dat*10+str[i]-0x30;
	}
	return i;
}
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
void transmit (u8 *dat)
{
	while (*dat) {
		PutinUart1 (*dat++);
	}

}
void ProcPcCmd(void)
{
	u8 dat;
	
	if(ptr>0&&Sys1msCnt-LastRecStamp>UARTFREE[SysInfo.baud]){		
		if(InSetMode){
			if(ptr>4&&RecBuf[ptr-1]==0x0a){
				if(CompStr(RecBuf,"BAUD=" ,5)){
					SysInfo.baud=RecBuf[5]-0x30;
					SaveSysInfo();  
					transmit("set Baud ok\r\n");
				}
				if(CompStr(RecBuf,"FREQ=" ,5)){
					ChangeStr_u8(&RecBuf[5],&dat);
					SysInfo.freq=dat;
					SaveSysInfo(); 
					transmit("setfreq ok\r\n");
				}
			}
		}	
        else
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




