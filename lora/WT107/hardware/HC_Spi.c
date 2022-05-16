
#include "HC_Spi.h"
#include "gpio.h"
#include "spi.h"
#include "ddl.h"

//#define SPI_BY_SOFTWARE 

void SpiSoftWareInitPort(void)
{
	stc_gpio_cfg_t GpioInitStruct;
	DDL_ZERO_STRUCT(GpioInitStruct);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
	
	GpioInitStruct.enDrv = GpioDrvH;
	GpioInitStruct.enDir = GpioDirOut;
	//CS
	Gpio_Init(GpioPortA,GpioPin4,&GpioInitStruct);
	//M0SI
	Gpio_Init(GpioPortA,GpioPin7,&GpioInitStruct);
	//SCK
	Gpio_Init(GpioPortA,GpioPin5,&GpioInitStruct);
	//RST
	Gpio_Init(GpioPortB,GpioPin1,&GpioInitStruct);
	//MISO
	GpioInitStruct.enDir = GpioDirIn;
	GpioInitStruct.enPu = GpioPuEnable;
	Gpio_Init(GpioPortA,GpioPin6,&GpioInitStruct);
//	//CS
	Gpio_WriteOutputIO(GpioPortA,GpioPin4,TRUE);
	//SCK 0
	Gpio_WriteOutputIO(GpioPortA,GpioPin5,FALSE);
	//MOSI 0
	Gpio_WriteOutputIO(GpioPortA,GpioPin7,FALSE);	
}

/*
	SPI端口初始化
	PA4 5 6 7
*/
void SpiPortInit(void)
{
	stc_gpio_cfg_t GpioInitStruct;
	DDL_ZERO_STRUCT(GpioInitStruct);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
	//SPI0
	GpioInitStruct.enDrv = GpioDrvH;
	GpioInitStruct.enDir = GpioDirOut;

	Gpio_Init(GpioPortA, GpioPin7,&GpioInitStruct);
	Gpio_SetAfMode(GpioPortA, GpioPin7,GpioAf1);       	 //SPI0_MOSI

//	Gpio_Init(GpioPortA, GpioPin4,&GpioInitStruct);
//	Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);         //SPI0_CS

	Gpio_Init(GpioPortA, GpioPin5,&GpioInitStruct);
	Gpio_SetAfMode(GpioPortA, GpioPin5,GpioAf1);         //SPI0_SCK

	GpioInitStruct.enDir = GpioDirIn;
//	GpioInitStruct.enPu = GpioPuEnable;
	Gpio_Init(GpioPortA, GpioPin6,&GpioInitStruct);			//SPI0_MISO
	Gpio_SetAfMode(GpioPortA, GpioPin6,GpioAf1);
	
}
/*
	硬件SPI初始化
*/
void SpiHardWareInit(void)
{
	stc_spi_cfg_t  SpiInitStruct;

	Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);
	SpiInitStruct.enSpiMode = SpiMskMaster;  
	SpiInitStruct.enPclkDiv = SpiClkMskDiv32; 
	SpiInitStruct.enCPHA    = SpiMskCphafirst;
	SpiInitStruct.enCPOL    = SpiMskcpollow;
	Spi_Init(M0P_SPI0, &SpiInitStruct);
	SpiPortInit();
}

/*
	SPI0 初始化
*/
void SpiInit(void)
{
	#if defined(SPI_BY_SOFTWARE)
	SpiSoftWareInitPort();
	#else
	SpiHardWareInit();
	#endif
}
u8 SpiSoftWareRWByte(u8 data)
{
	u8 temp = 0;
	for(u8 i = 0; i < 8; i++)
	{
		Gpio_WriteOutputIO(GpioPortA,GpioPin5,TRUE);
		if(data & 0x80)
		{
			Gpio_WriteOutputIO(GpioPortA,GpioPin7,TRUE);
		}
		else
		{
			Gpio_WriteOutputIO(GpioPortA,GpioPin7,TRUE);
		}
		data = data << 1;
		delay1us(1);
		Gpio_WriteOutputIO(GpioPortA,GpioPin5,FALSE);
		temp = temp << 1;
		temp |= Gpio_GetInputIO(GpioPortA,GpioPin6);
		delay1us(1);
	}
	return temp;
}
/*
	使用硬件SPI
*/
u8 SpiHardWareRWByte(u8 data)
{
	u8 ret = 0;
	while(Spi_GetStatus(M0P_SPI0, SpiTxe) == FALSE)
	{
		ret++;
		if(ret > 200)
		{
			return 0;
		}
	}
	Spi_SendData(M0P_SPI0,data);
	ret = 0;
	while(Spi_GetStatus(M0P_SPI0, SpiRxne) == FALSE)
	{
		ret++;
		if(ret > 200)
		{
			return 0;
		}
	}

	return Spi_ReceiveData(M0P_SPI0);
}

/*
  spi 读写寄存器
*/
u8 Spi1ReadWriteByte(u8 data)
{
	#if defined(SPI_BY_SOFTWARE)
	return SpiSoftWareRWByte(data);
	#else
	return SpiHardWareRWByte(data);
	#endif
}

/*
	SPI片选开启
*/
void SPI_Start(void)
{
	#if defined(SPI_BY_SOFTWARE)
	Gpio_WriteOutputIO(GpioPortA,GpioPin4,FALSE);
	#else
	Spi_SetCS(M0P_SPI0,FALSE);
	#endif
}
/*
	SPI片选关闭
*/
void SPI_Stop(void)
{
	#if defined(SPI_BY_SOFTWARE)
	Gpio_WriteOutputIO(GpioPortA,GpioPin4,TRUE);
	#else
	Spi_SetCS(M0P_SPI0,TRUE);
	#endif
}
