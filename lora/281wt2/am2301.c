#include "stm8l15x.h"
#include "hw_config.h" 
#include "pub.h"


unsigned char Sensor_Data[5];;
s16 g_Temp;
s16 g_humid;
//[0]湿度高8 [1]湿度低8  0.1为单位
//[2]温度高7 BIT7符号位  [3]温度低8  0.1为单位  
void delayus(u16 us)
{
    
    while(us--){
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
      __no_operation();
    }
}


unsigned char Read_SensorData(void)
{
	u16 i,j;
	unsigned char buffer;
	buffer = 0;
	for(i=0;i<8;i++){
        buffer <<=1;
		for(j=0;j<500;j++){            
            if(SDA_IN)break;//检测上次低电平是否结束
		}		
		//延时Min=26us Max50us 跳过数据"0" 的高电平
		delayus(40);	 //延时40us 	
		if(SDA_IN)  buffer |= 1;		
		
		for(j=0;j<500;j++){            
            if(!SDA_IN)break;//检测高电平是否结束
		}	
		
			
	}
	return buffer;
}
unsigned char Read_Sensor(void)
{
	u8 i;
	//主机拉低(Min=800US Max=20Ms) 
   s16 tmp;
   u8 cnt;
   
   for(cnt=0;cnt<3;cnt++){
   
        SDA_LOW;
          
        delayus(1000);
        SDA_HIGH; 	//释放总线 延时(Min=30us Max=50us)
        delayus(30);  
        if(SDA_IN ==0){	   //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	    
            for(i=0;i<250;i++){
               if(SDA_IN) break;//从机的低是否结束
            }
            if(i>=250) return 0;
            for(i=0;i<250;i++){
                if(!SDA_IN) break;//开始低电平输入数据
            }
            if(i>=250) return 0;
            __disable_interrupt();
            for(i=0;i<5;i++) {
                Sensor_Data[i] = Read_SensorData();
            }
            __enable_interrupt();
            if(Sensor_Data[4]==(u8)(Sensor_Data[3]+Sensor_Data[2]+Sensor_Data[1]+Sensor_Data[0])){
                g_humid=((u16)Sensor_Data[0]*256+Sensor_Data[1]);
                tmp= ((u16)(Sensor_Data[2]&0x7f)*256+Sensor_Data[3]); 
                if(Sensor_Data[2]&0x80) {                       
                    tmp=tmp*-1;
                }    
                printf("temp=%d,humi=%d\r\n",tmp,g_humid );
                
                if(tmp>-40) {
                    g_Temp=tmp;
                    return 1;
                }
            }
                           
        }
        if(cnt<2) {
            printf("read again\r\n");
            delayTick(2);
        }
   }
    return 0;
}

  