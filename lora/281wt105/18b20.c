#include "stm8l15x.h"
#include "hw_config.h" 
#include "pub.h"

#include <STDIO.H>	



#define DS18B20_PIN_H       SDA_HIGH 
#define DS18B20_PIN_L		SDA_LOW 
#define DS18B20_PIN_IN      SDA_IN

/////////////////////////
//STM8Ｌ片机ＯＤ输出，从0-1过程比较慢，10k上拉，需要近10uS，5Ｋ上拉需要5uS
u8 Reset_18b20(void)
{
	u8 u,i;
	DS18B20_PIN_H;
	delay1us();
	DS18B20_PIN_L;
	delayus(600);  //480-960 us
	DS18B20_PIN_H;
	delayus(10); //等待15-60
	for(i=0;i<20;i++)  {
		if(DS18B20_PIN_IN)break;//外部拉高
		delay1us();
	}
	for(i=0;i<15;i++)  {//响应60-240us的低
		if(DS18B20_PIN_IN==0)break;
		delay1us(); 	
	}
	if(i<15){
		for(i=0;i<25;i++)  {
		  if(DS18B20_PIN_IN)break;//再变为高
		  delayus(10); 	
		}
		if(i<25)  {
			delayus(400); 	
		  	u=1;
		}
		else
		  	u=0;
	}
	else
  		u=0;
  /*delay(14);  //delay 100us
  if(TEMPDATA==0)
  u=1;
  else
  u=0;
  delay(20);
  DS18B20_PIN_H;*/
	return u;
}
/////////////////////////////////
void WriteCmd18b20 (u8 wr)
{
  u8 i,j;
  for(i=0;i<8;i++)
  {
    DS18B20_PIN_L;
    delayus(5);//>1 <15
    if(wr&0x01)			DS18B20_PIN_H
	else 	DS18B20_PIN_L;
    delayus(50);  //delay 45 us
    DS18B20_PIN_H;
    
    //延时为ＳＴＭ8Ｌ特有
    for(j=0;j<20;j++)  {
        if(DS18B20_PIN_IN)break;//再变为高
		 	
	}
    delay1us();delay1us();delay1us();
    
    
    wr>>=1;
  }
}
////////////////////////////////
u8 Read_Data_18b20(void)
{
  u8 i,u=0;
  for(i=0;i<8;i++)
  {
    DS18B20_PIN_L;
    delay1us();
    delay1us();
    u>>=1;
    DS18B20_PIN_H;
    
    //延时为ＳＴＭ8Ｌ特有
    delayus(4);
    
    if(DS18B20_PIN_IN)
    	u|=0x80;
    delayus(50);
  }
  return u;
}


//计算出crc值                           //pass
u8 Crc8( u8* buf ,u8 len)
{
    u8 i = 0;
    u8 crc = 0;
    while ( len-- != 0 )    {
        for ( i = 1 ; i != 0 ; i *= 2)        {
            if ( ( crc & 1 ) != 0 )    {
                crc /= 2;
                crc ^= 0x8c;
            }
            else     {
                crc /= 2;
            }
            if ( ( *buf & i ) != 0 )
            {
                crc ^= 0x8c;
            }   
        }
        buf++;   
    }
    return crc;
} 
////////////////////////////////////////
u8 Init18b20(void)
{
   u8  i;
   u8  intbuf[3];
   u8  tmpbuf[4];  
   for(i=0;i<5;i++)
   {
      if(Reset_18b20())break;
   }
   if(i>=5) return 0;
   WriteCmd18b20(0xcc);//Skip ROM
   WriteCmd18b20(0x4e);//Write Scratchpad	
	intbuf[0]=0xaa;
	intbuf[1]=0xaa;
	#ifdef AD9BIT
   		intbuf[2]=0x1f;//9bit  //有的片子9位不灵
	#else
   		intbuf[2]=0x7f;//12bit
    #endif
   for(i=0;i<3;i++)
   {
      WriteCmd18b20(intbuf[i]);    	
   }
   for(i=0;i<5;i++)
   {
      if(Reset_18b20())      break;
   }
   if(i>=5) return 0;
   WriteCmd18b20(0xcc);//Skip ROM
   WriteCmd18b20(0xbe);//Read Scratchpad
   for(i=0;i<4;i++)
   {
     tmpbuf[i]=Read_Data_18b20();
   }  
 
   if((intbuf[0]==tmpbuf[2]) && (intbuf[1]==tmpbuf[3]))
   {
      for(i=0;i<5;i++)
      {
         if(Reset_18b20())      break;
      }
      if(i>=5) return 0;
      return 1;
   }   
   return 0;
}
///////////////////////////////////////////////////////
u8 Start18b20Ad(void)
{
   u8 i;
   if(Init18b20()==0) return 0;

   for(i=0;i<5;i++)
   {
      if(Reset_18b20())    break;
   }
   if(i>=5) return 0;
   //EA=0;
   WriteCmd18b20(0xcc);//Skip ROM
   WriteCmd18b20(0x44);//Convert T
   return 1;	
   //EA=1;
}
u8 Read_Thermometer(s16 *Temper)  //12bit 750ms   9bit  93ms 
{
	u8 i;  
	u8 databuf[10];
	s16 temp;
   	if(!Reset_18b20())return 0;
	//EA=0;
	WriteCmd18b20(0xcc);//Skip ROM
	WriteCmd18b20(0xbe);//Read Scratchpad
	for(i=0;i<9;i++)   {
		databuf[i]=Read_Data_18b20();
	}
	i=Crc8( databuf ,8);
	if(i==databuf[8]){		
		temp=((databuf[1]&0x00ff)<<8)+(databuf[0]&0x00ff);
//		if(temp<0) temp=0;
					
		*Temper=temp*10/16;//每LSI 0.0625度
	

		return 1;
	}
	else{	
		return 0;
	}
}



