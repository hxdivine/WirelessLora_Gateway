
#include "gd32e10x.h"
#include "usart.h"
#include "stdio.h"
#include "hardware.h"
#include "GSM234G.h"

u8 TxBuffer0[SENDBUFF_MAX];
u8 TxBuffer1[SENDBUFF_MAX];

__IO u16 TxPtr0,TxCnt0;
__IO u16 TxPtr1,TxCnt1;

USART_FRAME_STRUCT usart_Frame;
u8 sendStop = 0;

/*
 串口1初始化  PC端输出
*/
void InitUsart0(u32 baudRate)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	//A9  Tx
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
	//A10 Rx
	gpio_init(GPIOA,GPIO_MODE_IPD,GPIO_OSPEED_50MHZ,GPIO_PIN_10);

	usart_deinit(USART0);
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);
	usart_baudrate_set(USART0,115200U);
	usart_word_length_set(USART0,USART_WL_8BIT);
	usart_stop_bit_set(USART0,USART_STB_1BIT);
	usart_parity_config(USART0,USART_PM_NONE);
	usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
	
	usart_enable(USART0);
	usart_interrupt_enable(USART0,USART_INT_IDLE);
	usart_interrupt_enable(USART0,USART_INT_RBNE);
}


void USART0_IRQHandler(void)
{
	if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE) != RESET)
	{
		usart_Frame.recBuf[usart_Frame.recLen++] = usart_data_receive(USART0);
	}
	else if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE) == SET)        
	{
		usart_Frame.recFlag = 1;
		usart_data_receive(USART0); 
		usart_Frame.recBuf[usart_Frame.recLen] = '\0';
		if(usart_Frame.recLen == 4)
		{
			if(usart_Frame.recBuf[0] == 'S' && usart_Frame.recBuf[3] == 'P')
			{//STOP
				sendStop = 1;
			}
		}
  }	
	else if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_TBE) != RESET)
	{
		if(TxCnt0 == 0)
		{//数据发送完毕
			TxPtr0 =0;
			//关闭串口发送
			usart_interrupt_disable(USART0,USART_INT_TBE);		
	  }   
		else
		{	  
			usart_data_transmit(USART0,TxBuffer0[TxPtr0]);
			//发送buf指针后移
			TxPtr0++;	
			//确保指针位置
			TxPtr0 = TxPtr0 & (SENDBUFF_MAX - 1);
			//待发送数据个数--
			TxCnt0--;
		}
	}
	else if(usart_interrupt_flag_get(USART0,USART_INT_FLAG_ERR_ORERR) != RESET)
	{
		usart_data_receive(USART0);
	}
}

/*
	串口2初始化 GSM通信
*/
void InitUsart1(u32 baudRate)
{
	rcu_periph_clock_enable(RCU_USART1);
	//Tx A2 复用推挽
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_2);
	//Rx A3 下拉输入
	gpio_init(GPIOA,GPIO_MODE_IPD,GPIO_OSPEED_10MHZ,GPIO_PIN_3);
	
	usart_transmit_config(USART1,USART_TRANSMIT_ENABLE);
	usart_receive_config(USART1,USART_RECEIVE_ENABLE);
	usart_baudrate_set(USART1,baudRate);
	usart_word_length_set(USART1,USART_WL_8BIT);
	usart_stop_bit_set(USART1,USART_STB_1BIT);
	usart_parity_config(USART1,USART_PM_NONE);
	usart_hardware_flow_rts_config(USART1,USART_RTS_DISABLE);
	usart_hardware_flow_cts_config(USART1,USART_CTS_DISABLE);
	
	usart_enable(USART1);
	usart_interrupt_enable(USART1,USART_INT_RBNE);
}

void USART1_IRQHandler(void)
{
	u8 dat;
	
	if(usart_interrupt_flag_get(USART1,USART_INT_FLAG_RBNE) != RESET)
	{
		dat = usart_data_receive(USART1);
		UartToGSM(dat);
	}
	else if(usart_interrupt_flag_get(USART1,USART_INT_FLAG_TBE) != RESET)
	{
		if(TxCnt1 == 0)
		{
			TxPtr1 = 0;
			usart_interrupt_disable(USART1,USART_INT_TBE);	
		}
		else
		{
			usart_data_transmit(USART1,TxBuffer1[TxPtr1]);
			//发送指针后移
			TxPtr1++;
			TxPtr1 &= (SENDBUFF_MAX - 1);
			//待发送数据个数--
			TxCnt1--;
		}
	}
	else if(usart_interrupt_flag_get(USART1,USART_INT_FLAG_ERR_ORERR))
	{
		usart_data_receive(USART1);
	}
}

/*
	重定义putc函数，这样可以使用printf函数从串口打印输出
*/
int fputc(int ch, FILE *f)
{
	PutInUart0((u8)ch);
//	usart_data_transmit(USART0, (uint8_t)ch);
//	while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	return ch;
}
