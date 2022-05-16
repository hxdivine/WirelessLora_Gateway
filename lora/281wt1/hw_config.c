#include "stm8l15x.h"


#include "hw_config.h" 
#include "pub.h"

u8 TxBuffer1[UART_SENDBUF_SIZE1];
u16 TxPtr1,TxCnt1;

#define ENWDG


void  EnWatchDog(void)
{
#ifdef ENWDG
    CLK_LSICmd(ENABLE);
    while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
    IWDG_Enable();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetPrescaler(IWDG_Prescaler_256);//38K/256 6.72ms一个计数
	IWDG_SetReload(254);  		//= (254 + 1) * 256 / 38 000=1.71s
	IWDG_ReloadCounter();		//将RLR值写入看门狗计数初值
	
#endif	
}

void delayTick(u16 t)
{  	
	u16 tick=SysTick; 	
	while(SysTick-tick<t){
        ClrWdg;
        SetLowPwr(1);
	}
}

void delayTickWfi(u16 t)
{  	
	u16 tick=SysTick; 	
	while(SysTick-tick<t){
        ClrWdg;
        SetLowPwr(0);
	}
}

void SetLowPwr(u8 mode)
{
 
//Wait mode: CPU clock is stopped, but selected peripherals keep running. An internal
//or external interrupt or a Reset can be used to exit the microcontroller from Wait mode(WFE or WFI mode).
 //  
 /**/	 
     return;
    if(mode==0)//450uA @8M  实测板子480uA @4M ,而用外部8M，2分频，715uA
        wfi(); 
	
    if(mode==1)//1uA
        halt(); 

}
void Tim1_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
    
    TIM1_TimeBaseInit(16-1,TIM1_CounterMode_Up,1000,0);//16分频，
    TIM1->EGR = (uint8_t)TIM1_PSCReloadMode_Immediate;//分频立即生效

	TIM1_ITConfig(TIM1_IT_Update ,ENABLE );
	TIM1_Cmd(ENABLE);
   
}


void StopAdc(void)
{
    ADC_VrefintCmd(DISABLE);//VREFINTON复位
    ADC_ChannelCmd(ADC1, ADC_Channel_0, DISABLE);
   
	
    ADC_Cmd(ADC1, DISABLE);		
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);
    
	DISDETBAT	
    GPIO_Init( GPIOA, GPIO_Pin_6, GPIO_Mode_Out_OD_HiZ_Fast);
    
}
void StartAdc()
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
    ENDETBAT
       
    GPIO_Init(GPIOA,(GPIO_Pin_6),GPIO_Mode_In_FL_No_IT );//ad0
    ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
    ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
    ADC_VrefintCmd(ENABLE);//TSON 
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);		
    ADC_ChannelCmd(ADC1,ADC_Channel_Vrefint ,ENABLE);  // ADC_Channel_0 
    delayTickWfi(2);//32hz tick REF开启需要时间，且不能在HALT模式，
    AdcCh=0;
  
    ADC_SoftwareStartConv(ADC1);
    
    SetLowPwr(0);
    while(AdcCh!=255);

          
    StopAdc();      
}
void TranAdValue(void)
{
    
    __disable_interrupt();
    
    __enable_interrupt();
    //AdRes1=AdRes1*3300*2.5/4/1024;AdRes1=AdRes1*3300*5/(8*1024);8次累加
    
        
}

void InitGpio(void)
{


	GPIO_Init(GPIOA,(GPIO_Pin_5),GPIO_Mode_Out_PP_High_Slow );//tx1
	GPIO_Init(GPIOA,(GPIO_Pin_4),GPIO_Mode_In_PU_No_IT );//rx1
    GPIO_Init( GPIOA, GPIO_Pin_6, GPIO_Mode_Out_OD_HiZ_Slow);
    
      
    GPIO_Init(GPIOB,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Slow );//led 2
    GPIO_Init(GPIOB,(GPIO_Pin_2),GPIO_Mode_Out_OD_HiZ_Fast );//   
    GPIO_Init(GPIOB,(GPIO_Pin_TypeDef)(GPIO_Pin_3),GPIO_Mode_Out_PP_Low_Slow);
   
    GPIO_Init( GPIOB, GPIO_Pin_7,GPIO_Mode_In_PU_No_IT );
    GPIO_Init( GPIOB,  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_Out_PP_High_Slow );
    
    
    GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6),GPIO_Mode_Out_PP_Low_Slow );

    GPIO_Init(GPIOD,(GPIO_Pin_TypeDef)(GPIO_Pin_3),GPIO_Mode_Out_PP_Low_Slow );//led 1  
    GPIO_Init(GPIOD,GPIO_Pin_4,GPIO_Mode_In_FL_No_IT);
    
    
    GPIO_Init( GPIOD, (GPIO_Pin_TypeDef)(GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7  ), GPIO_Mode_Out_PP_High_Slow );
    
    
    //没用到的脚
    GPIO_Init(GPIOA,GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4,GPIO_Mode_Out_PP_Low_Slow );
    GPIO_Init(GPIOD,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_Out_PP_Low_Slow );
    GPIO_Init(GPIOB,GPIO_Pin_1,GPIO_Mode_Out_PP_Low_Slow );
  //  GPIO_Init(GPIOC,GPIO_Pin_0|GPIO_Pin_1,GPIO_Mode_Out_PP_Low_Slow );
    
    DIS485; 

}

const  u32 BAUDMODE[]={115200,57600,38400,19200,9600,4800,2400,1200};
void Uart_Init(u8 BaudMode)
{

    USART_WordLength_TypeDef    WordLength;
    USART_StopBits_TypeDef      StopBits;
    USART_Parity_TypeDef        Parity;
    CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
    GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_Pin_3),GPIO_Mode_Out_PP_High_Fast );
    GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_Pin_2),GPIO_Mode_In_PU_No_IT ); 
    USART_DeInit(USART1);
    if(BaudMode&0x40)	{
		WordLength = USART_WordLength_9b;  
		if(BaudMode&0x8)
			Parity =   USART_Parity_Odd;
		else
			Parity =   USART_Parity_Even;
	}
	else{
		WordLength = USART_WordLength_8b;
        Parity = USART_Parity_No;
	}
	
	if(BaudMode&0x20)	{	
        StopBits = USART_StopBits_2;
	}
	else
		StopBits = USART_StopBits_1;
    
    USART_Init(USART1, (u32)BAUDMODE[BaudMode&7], WordLength, StopBits, 
                Parity, (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
    
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
    USART_Cmd(USART1, ENABLE);

  
}

void PutinUart1(u8 dat)
{
    EN485;
	if((USART1->CR2 & USART_CR2_TEN)==0)return;
	ClrWdg;
	while(TxCnt1+TxPtr1>=UART_SENDBUF_SIZE1);
	ClrWdg;
	USART_ITConfig(USART1, USART_IT_TC, DISABLE); 	
	TxBuffer1[(TxCnt1+TxPtr1)&(UART_SENDBUF_SIZE1-1)]=dat&0xff;
	TxCnt1++;
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
}

int putchar (int ch)  
{
#ifndef WT1    
    PutinUart1((u8) ch);
#endif    
	return (ch);
}

u16 GetSysTick(void)//32hz tick
{
    u16 i;
    __disable_interrupt();
    i=SysTick;
    __enable_interrupt();
    if(i==0) i=1;
    return i;
}
void InitRtc()
{
    CLK_LSEConfig(CLK_LSE_ON);
    while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET); 
    RTC_DeInit();
    CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); //允许RTC时钟 
    CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, CLK_RTCCLKDiv_64); //
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div4); //128hz
    
    RTC_SetWakeUpCounter(8-1);
   
    RTC_WakeUpCmd(ENABLE);
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    
}
void PrintInfo(void)
{
	printf("\r\n\r\n\r\n");
	printf("/***************************************/\r\n");
	printf("/*    copyright 2019 WWW.GSM-M2M.COM   */\r\n");
	printf("/*        All rights Reserved          */\r\n");	
	printf("/*    latest  build@%s       */\r\n",__DATE__);
	printf("/****  king Pigeon Hi-Tech.Co.,ltd. ****/\r\n");
}
void SetSystem(void)
{
    PWR_UltraLowPowerCmd(ENABLE);  //开启电源的低功耗模式
    CLK_HSEConfig(CLK_HSE_OFF);          
    CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);//16M
	InitGpio();
	InitRtc();
    EnWatchDog();
    

   Tim1_Init();

    Uart_Init(SysInfo.baud);
   
    __enable_interrupt();
	
 //   PrintInfo();
  //  StartAdc();
}
