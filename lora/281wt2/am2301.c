#include "stm8l15x.h"
#include "hw_config.h" 
#include "pub.h"


unsigned char Sensor_Data[5];;
s16 g_Temp;
s16 g_humid;
//[0]ʪ�ȸ�8 [1]ʪ�ȵ�8  0.1Ϊ��λ
//[2]�¶ȸ�7 BIT7����λ  [3]�¶ȵ�8  0.1Ϊ��λ  
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
            if(SDA_IN)break;//����ϴε͵�ƽ�Ƿ����
		}		
		//��ʱMin=26us Max50us ��������"0" �ĸߵ�ƽ
		delayus(40);	 //��ʱ40us 	
		if(SDA_IN)  buffer |= 1;		
		
		for(j=0;j<500;j++){            
            if(!SDA_IN)break;//���ߵ�ƽ�Ƿ����
		}	
		
			
	}
	return buffer;
}
unsigned char Read_Sensor(void)
{
	u8 i;
	//��������(Min=800US Max=20Ms) 
   s16 tmp;
   u8 cnt;
   
   for(cnt=0;cnt<3;cnt++){
   
        SDA_LOW;
          
        delayus(1000);
        SDA_HIGH; 	//�ͷ����� ��ʱ(Min=30us Max=50us)
        delayus(30);  
        if(SDA_IN ==0){	   //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	    
            for(i=0;i<250;i++){
               if(SDA_IN) break;//�ӻ��ĵ��Ƿ����
            }
            if(i>=250) return 0;
            for(i=0;i<250;i++){
                if(!SDA_IN) break;//��ʼ�͵�ƽ��������
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

  