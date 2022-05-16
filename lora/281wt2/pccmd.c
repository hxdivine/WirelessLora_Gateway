#include "stm8l15x.h"
#include "pub.h"  
#include "stdio.h"

#define MANUALLEN	100
u8 ManualSetBuf[MANUALLEN];//记录与短信兼容指令,ASC符格式
u16 ManualSetCmdLen=0;//ASC符格式命令长度

extern u8 TstSend;;
void WorkMode(void);

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
	
	
		//if(CompStr(buf,(u8*)DON  ,sizeof(DON)-1)){
			
			//return;
		//}
        
	if(CompStr(buf,"SEND"  ,4)){
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




