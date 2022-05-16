
#include "HC_Usart.h"
#include "dmac.h"
#include "stdio.h"
#include "ddl.h"

USART_REC usartRec = {0,0};

u8 RxBuf[30] = {0};
u8 UartLen = 0;


void PCUsart1(u8 data)
{
	Uart_SendDataIt(M0P_UART1, data);
	
	while(Uart_GetStatus(M0P_UART1,UartTxe) == FALSE);
}

void SendStr(u8 *InBuf,u8 len)
{
	for(u8 i = 0; i < len; i++)
	{
		PCUsart1(InBuf[i]);
	}
}

/*
	串口引脚配置

	PA 2 3
	PA 9 10
*/
void UsartPortInit(void)
{
	stc_gpio_cfg_t stcGpioCfg;
	DDL_ZERO_STRUCT(stcGpioCfg);
	
	stcGpioCfg.enDir = GpioDirOut;
	Gpio_Init(GpioPortA,GpioPin2,&stcGpioCfg);
	Gpio_SetAfMode(GpioPortA,GpioPin2,GpioAf1);             //PA02 ?UART1 TX
	stcGpioCfg.enDir = GpioDirIn;
	stcGpioCfg.enPu = GpioPuEnable;
	Gpio_Init(GpioPortA,GpioPin3,&stcGpioCfg);
  Gpio_SetAfMode(GpioPortA,GpioPin3,GpioAf1);             //PA03 ?UART1 RX

	stcGpioCfg.enDir = GpioDirOut;
	Gpio_Init(GpioPortA,GpioPin9,&stcGpioCfg);
	Gpio_SetAfMode(GpioPortA,GpioPin9,GpioAf1);             //PA9 ?UART0 TX
	stcGpioCfg.enDir = GpioDirIn;
	Gpio_Init(GpioPortA,GpioPin10,&stcGpioCfg);
  Gpio_SetAfMode(GpioPortA,GpioPin10,GpioAf1);             //PA10 ?UART0 RX
	
}
/*
	串口1配置
*/
void Usart1Init(void)
{
	stc_uart_cfg_t stcCfg;
	stc_uart_baud_t stcBaud;
	
  DDL_ZERO_STRUCT(stcCfg);
  DDL_ZERO_STRUCT(stcBaud);	
	/*使能外设时钟门控开关*/
  ///<UART Init
  stcCfg.enRunMode        = UartMskMode1;  				 //模式1   设置无校验位 需要将校验位去掉
  stcCfg.enStopBit        = UartMsk1bit;   				 //1 位停止位
	//stcCfg.enMmdorCk        =  UartMskEven;  				 //偶校验
  stcCfg.stcBaud.u32Baud  = 9600;                //波特率9600
  stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;    	 //16分频采集
  stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); //获取外设时钟
  Uart_Init(M0P_UART1, &stcCfg);                   //串口初始化

	Uart_ClrStatus(M0P_UART1,UartRC);//清除请求
	Uart_ClrStatus(M0P_UART1,UartTC);
 
	Uart_EnableIrq(M0P_UART1,UartRxIrq); 
  EnableNvic(UART1_IRQn, IrqLevel3, TRUE);   
	UsartPortInit();
}
/*
	串口中断服务函数
*/
void UART1_IRQHandler(void)
{
	if(Uart_GetStatus(M0P_UART1,UartRC))
	{
		Uart_ClrStatus(M0P_UART1, UartRC);
		usartRec.mRepBuf[usartRec.len++] = Uart_ReceiveData(M0P_UART1);
	}
}
/*
	用于串口打印信息
*/
void Usart0Init(void)
{
	stc_uart_cfg_t stcCfg;
	stc_uart_baud_t stcBaud;
	
  DDL_ZERO_STRUCT(stcCfg);
  DDL_ZERO_STRUCT(stcBaud);	
	/*??????????*/
  ///<UART Init
  stcCfg.enRunMode        = UartMskMode1;  				 //??1   ?????? ????????
  stcCfg.enStopBit        = UartMsk1bit;   				 //1 ????
	//stcCfg.enMmdorCk        =  UartMskEven;  				 //???
  stcCfg.stcBaud.u32Baud  = 115200;                //???9600
  stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;    	 //16????
  stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); //??????
  Uart_Init(M0P_UART0, &stcCfg);                   //?????

	Uart_ClrStatus(M0P_UART0,UartRC);//????
	Uart_ClrStatus(M0P_UART0,UartTC);
 
	Uart_EnableIrq(M0P_UART0,UartRxIrq);	//使能 接收串口
	EnableNvic(UART0_IRQn, IrqLevel3, TRUE);
}

void CloseUartRx(void)
{
	Uart_DisableIrq(M0P_UART0,UartRxIrq);
}

int fputc(int ch, FILE *f)
{
	Uart_SendDataIt(M0P_UART0, ch);
	
	while(Uart_GetStatus(M0P_UART0,UartTxe) == FALSE);
	
	return (ch);
}

void UART0_IRQHandler(void)
{
	if(Uart_GetStatus(M0P_UART0,UartRC) == TRUE)
	{
		Uart_ClrStatus(M0P_UART0,UartRC);
			if(UartLen < 30)
			RxBuf[UartLen++] = Uart_ReceiveData(M0P_UART0);

	}
}
