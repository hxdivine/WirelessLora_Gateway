#include "stm8l15x.h"

#include "hw_config.h" 
#include "pub.h"
#include "flash_eeprom.h"


vu16 SysTick=0;
u8 AdcCh=0;
u8 LedCnt;
u8 TimeUp;
u16 AdcVal,AdcVref;
void IsrTime()
{
    
	SysTick++;
    
    if(LedCnt){
        LedCnt--;        
        if(LedCnt)
           WORKLEDON 
        else
           WORKLEDOFF 
    }
    else{
        if(InSetMode){
           // if((SysTick&0x07)==0)
                WORKLEDON
           // else  if((SysTick&0x07)>2)        
           //    WORKLEDOFF
        }
        else{
            if((SysTick&0x7f)==0)
                WORKLEDON
            else if((SysTick&0x7f)>=1)         
               WORKLEDOFF
        }      
    }
            
/*    if(AlarmCnt){
       AlarmCnt--;
       ALARMLEDON
    }
    else 
        ALARMLEDOFF*/
}
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
    RTC_ClearITPendingBit(RTC_IT_WUT);
    IsrTime();
}

INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler, 5){}
INTERRUPT_HANDLER(EXTIB_G_IRQHandler, 6){}
INTERRUPT_HANDLER(EXTID_H_IRQHandler, 7){}
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
    EXTI_ClearITPendingBit(EXTI_IT_Pin0);   
}
INTERRUPT_HANDLER(EXTI1_IRQHandler, 9){}
INTERRUPT_HANDLER(EXTI2_IRQHandler, 10){}
INTERRUPT_HANDLER(EXTI3_IRQHandler, 11){}
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12){}
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13){}
INTERRUPT_HANDLER(EXTI6_IRQHandler, 14){}
INTERRUPT_HANDLER(EXTI7_IRQHandler, 15){}
INTERRUPT_HANDLER(LCD_AES_IRQHandler, 16){}
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler, 17){}

INTERRUPT_HANDLER(ADC1_COMP_IRQHandler, 18)
{
   
      
     //       AdVal=0;            
       
     //   else ADC_SoftwareStartConv(ADC1);
     if(AdcCh<8){//1.224V
        AdcCh++;
        if(AdcCh>4)
        AdcVref+=  ADC_GetConversionValue(ADC1);       
        if(AdcCh==8){
            AdcVref>>=2;
            ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,DISABLE);
            ADC_ChannelCmd(ADC1, ADC_Channel_0, ENABLE);
        }
         ADC_SoftwareStartConv(ADC1);
    }
    else  { //
        
        AdcCh++;
        if(AdcCh>12)
        AdcVal+=  ADC_GetConversionValue(ADC1); 
        if(AdcCh>=16){
            AdcVal>>=2;
            AdcCh=255;
        }
        else 
            ADC_SoftwareStartConv(ADC1);
    }   
    
  /*  
    AdcCh++;//间隔采样，相邻会有影响，电容没释放？
    if(AdcCh&1){
        AdcVref+=  ADC_GetConversionValue(ADC1);
        ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint,DISABLE);
        ADC_ChannelCmd(ADC1, ADC_Channel_0, ENABLE);
        ADC_SoftwareStartConv(ADC1);
    }
    else  { 
        
        AdcVal+=  ADC_GetConversionValue(ADC1); 
        ADC_ChannelCmd(ADC1,ADC_Channel_0,DISABLE);
        ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint , ENABLE);
        
        if(AdcCh>=16){
           AdcVal>>=3;
           AdcVref>>=3;
           AdcCh=255;
        }
        else
            ADC_SoftwareStartConv(ADC1);
    }  
    */   
   
}

INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{

    
}

INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
{
	
}

INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler, 21)
{
	
}

INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler, 22)
{
   
}

INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler, 23)
{
    TIM1->SR1 = (uint8_t)(~TIM1_IT_Update);
    
}
INTERRUPT_HANDLER(TIM1_CC_IRQHandler, 24){}
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25){}
INTERRUPT_HANDLER(SPI1_IRQHandler, 26){}

INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler, 27)
{

	if(USART1->SR&USART_FLAG_TC){	    
        if(TxCnt1 ==0){
			TxPtr1 =0;   
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	    }   
		else {
            USART1->DR = TxBuffer1[TxPtr1];			
			TxPtr1++;
			TxPtr1=TxPtr1&(UART_SENDBUF_SIZE1-1);
			TxCnt1--;
		}
	}
}

u8 tmp;
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler, 28)
{
   	if(USART1->SR&USART_FLAG_RXNE){
		
        pccmdIsr(USART1->DR);
	} 
    else if(USART1->SR&USART_FLAG_OR){    
		tmp=USART1->DR;
	}
}


INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29){}
