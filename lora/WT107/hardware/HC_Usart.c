
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
	������������

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
	����1����
*/
void Usart1Init(void)
{
	stc_uart_cfg_t stcCfg;
	stc_uart_baud_t stcBaud;
	
  DDL_ZERO_STRUCT(stcCfg);
  DDL_ZERO_STRUCT(stcBaud);	
	/*ʹ������ʱ���ſؿ���*/
  ///<UART Init
  stcCfg.enRunMode        = UartMskMode1;  				 //ģʽ1   ������У��λ ��Ҫ��У��λȥ��
  stcCfg.enStopBit        = UartMsk1bit;   				 //1 λֹͣλ
	//stcCfg.enMmdorCk        =  UartMskEven;  				 //żУ��
  stcCfg.stcBaud.u32Baud  = 9600;                //������9600
  stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;    	 //16��Ƶ�ɼ�
  stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); //��ȡ����ʱ��
  Uart_Init(M0P_UART1, &stcCfg);                   //���ڳ�ʼ��

	Uart_ClrStatus(M0P_UART1,UartRC);//�������
	Uart_ClrStatus(M0P_UART1,UartTC);
 
	Uart_EnableIrq(M0P_UART1,UartRxIrq); 
  EnableNvic(UART1_IRQn, IrqLevel3, TRUE);   
	UsartPortInit();
}
/*
	�����жϷ�����
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
	���ڴ��ڴ�ӡ��Ϣ
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
 
	Uart_EnableIrq(M0P_UART0,UartRxIrq);	//ʹ�� ���մ���
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
