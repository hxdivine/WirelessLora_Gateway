
#include "HC_I2c.h"
#include "gpio.h"
#include "ddl.h"
#include "i2c.h"

#define IIC0_PORT					GpioPortD
#define IIC1_PORT 				GpioPortA

#define IIC0_SDA_PIN			GpioPin0
#define IIC0_SCL_PIN			GpioPin1

#define IIC1_SDA_PIN			GpioPin12
#define IIC1_SCL_PIN			GpioPin11

#define IIC_SCL_H(m)			{	if(m == 0){Gpio_WriteOutputIO(IIC0_PORT,IIC0_SCL_PIN,TRUE);} else {Gpio_WriteOutputIO(IIC1_PORT,IIC1_SCL_PIN,TRUE);}}
#define IIC_SCL_L(m)			{	if(m == 0){Gpio_WriteOutputIO(IIC0_PORT,IIC0_SCL_PIN,FALSE);} else {Gpio_WriteOutputIO(IIC1_PORT,IIC1_SCL_PIN,FALSE);}}

#define IIC_SDA_H(m)			{	if(m == 0){Gpio_WriteOutputIO(IIC0_PORT,IIC0_SDA_PIN,TRUE);} else{ Gpio_WriteOutputIO(IIC1_PORT,IIC1_SDA_PIN,TRUE);}}
#define IIC_SDA_L(m)			{	if(m == 0){Gpio_WriteOutputIO(IIC0_PORT,IIC0_SDA_PIN,FALSE);}else{ Gpio_WriteOutputIO(IIC1_PORT,IIC1_SDA_PIN,FALSE);}}
		
#define READ_SDA(m)				Read_SDA(m)
/*
	?¨¢SDA¨ºy?Y
*/
boolean_t Read_SDA(u8 mode)
{
	if(mode)
	{
		return Gpio_GetInputIO(IIC1_PORT,IIC1_SDA_PIN);
	}
	else
	{
		return Gpio_GetInputIO(IIC0_PORT,IIC0_SDA_PIN);
	}
}

/*
	IIC 3?¨º??¡¥6
*/
void IIC_Init(void)
{
	stc_gpio_cfg_t GpioInitStruct;
	DDL_ZERO_STRUCT(GpioInitStruct);
	
	GpioInitStruct.enDrv = GpioDrvH;
	GpioInitStruct.enDir = GpioDirOut;
	GpioInitStruct.enOD = GpioOdEnable;
	GpioInitStruct.enPu = GpioPuEnable;
	
	Gpio_Init(IIC0_PORT,IIC0_SCL_PIN,&GpioInitStruct);
	Gpio_Init(IIC0_PORT,IIC0_SDA_PIN,&GpioInitStruct);
	
	Gpio_Init(IIC1_PORT,IIC1_SDA_PIN,&GpioInitStruct);
	Gpio_Init(IIC1_PORT,IIC1_SCL_PIN,&GpioInitStruct);
	
	IIC_SCL_H(0)
	IIC_SDA_H(0)
	
	IIC_SCL_H(1)
	IIC_SDA_H(1)
}
/*
	SDA¨º?3?????
mode:0	IIC0
		 1	IIC1
*/
void SDA_OUT(u8 mode)
{
	stc_gpio_cfg_t GpioInitStruct;
	DDL_ZERO_STRUCT(GpioInitStruct);
	
	GpioInitStruct.enDrv = GpioDrvH;
	GpioInitStruct.enDir = GpioDirOut;
	GpioInitStruct.enOD = GpioOdEnable;
	GpioInitStruct.enPu = GpioPuEnable;
	
	if(mode == 0)
	{
		Gpio_Init(IIC0_PORT,IIC0_SDA_PIN,&GpioInitStruct);
	}
	else
	{
		Gpio_Init(IIC1_PORT,IIC1_SDA_PIN,&GpioInitStruct);
	}
}

/*
	SDA¨º?¨¨?????
mode:0	IIC0
		 1	IIC1
*/
void SDA_IN(u8 mode)
{
	stc_gpio_cfg_t GpioInitStruct;
	DDL_ZERO_STRUCT(GpioInitStruct);
	
	GpioInitStruct.enDrv = GpioDrvH;
	GpioInitStruct.enDir = GpioDirIn;
	GpioInitStruct.enPu = GpioPuEnable;
	
	if(mode == 0)
	{
		Gpio_Init(IIC0_PORT,IIC0_SDA_PIN,&GpioInitStruct);
	}
	else
	{
		Gpio_Init(IIC1_PORT,IIC1_SDA_PIN,&GpioInitStruct);
	}
}
/*
	2¨²¨¦¨²IIC?e¨º?D?o?
mode:0	IIC0
		 1	IIC1
*/
void IIC_Start(u8 mode)
{
	SDA_OUT(mode);
	IIC_SDA_H(mode);
	IIC_SCL_H(mode);
	delay1us(5);
	IIC_SDA_L(mode);
	delay1us(5);
	IIC_SCL_L(mode);
	delay1us(5);
}
/*
	¨ª¡ê?1D?o?
*/
void IIC_Stop(u8 mode)
{
	SDA_OUT(mode);
	IIC_SCL_L(mode);
	IIC_SDA_L(mode);
	IIC_SCL_H(mode);
	delay1us(7);
	IIC_SDA_H(mode);
	delay1us(7);
}
/*
	¦Ì¨¨¡äy¨®|¡äeD?o?
*/
u8 IIC_Wait_Ack(u8 mode)
{
	u8 tempTime = 0;
	SDA_IN(mode);
	IIC_SDA_H(mode);
	delay1us(1);
	IIC_SCL_H(mode);
	delay1us(5);

	while(READ_SDA(mode))
	{
		tempTime++;
		if(tempTime > 250)
		{
			IIC_Stop(mode);
//			printf("NO Ack\r\n");
			return 1;
		}
	}
	IIC_SCL_L(mode);
	delay1us(5);
	return 0;
}
/*
	2¨²¨¦¨²ACK¨®|¡äe
*/
void IIC_ACK(u8 mode)
{
	IIC_SCL_L(mode);
	SDA_OUT(mode);
	IIC_SDA_L(mode);
	delay1us(2);
	IIC_SCL_H(mode);
	delay1us(5);
	IIC_SCL_L(mode);
	delay1us(5);
}
/*
	2¨²¨¦¨²¡¤?¨®|¡äeD?o?
*/
void IIC_NAck(u8 mode)
{
	IIC_SCL_L(mode);
	SDA_OUT(mode);
	IIC_SDA_H(mode);
	delay1us(2);
	IIC_SCL_H(mode);
	delay10us(5);
	IIC_SCL_L(mode);
	delay1us(5);
}
/*
	IIC¡¤¡é?¨ª¨°???¡Á??¨²
*/
u8 IIC_SendByte(u8 txd,u8 mode)
{
	SDA_OUT(mode);
	IIC_SCL_L(mode);
	
	for(u8 i = 0; i < 8; i++)
	{
		if(txd & 0x80)
		{
			IIC_SDA_H(mode);
		}
		else
		{
			IIC_SDA_L(mode);
		}
		
		txd <<= 1;
		delay1us(2);
		IIC_SCL_H(mode);
		delay1us(5);
		IIC_SCL_L(mode);
		delay1us(5);
	}
	return IIC_Wait_Ack(mode);
}

/*
	IIC?¨¢¨¨?¨°???¡Á??¨²
*/
u8 IIC_ReadByte(u8 ack,u8 mode)
{
	u8 rec = 0;
	SDA_IN(mode);
	
	for(u8 i = 0; i < 8; i++)
	{
		IIC_SCL_L(mode);
		delay1us(5);
		IIC_SCL_H(mode);
		rec <<= 1;
		
		if(READ_SDA(mode))
		{
			rec++;
		}
		delay1us(5);
	}
	if(!ack)
	{
		IIC_NAck(mode);
	}
	else
	{
		IIC_ACK(mode);
	}
	return rec;
}
