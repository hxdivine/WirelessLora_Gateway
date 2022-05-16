/*
	用于读取大气温湿度
	温度转换公式	T = 175 * temp / (2^16 - 1) - 45 两位小数
	湿度转换公式	RH = humi * 100 / (2^16 - 1) * 10  实际扩展10倍
*/
#include "Gxht3l.h"
#include "HC_I2c.h"
#include "stdlib.h"
#include "ddl.h"

//大气温湿度 从机地址
#define I2C_TEMPADDR  0x44
#define I2C_TEMP_WRITE_ADDR  I2C_TEMPADDR << 1
#define I2C_TEMP_READ_ADDR  (I2C_TEMPADDR << 1) | 0x01

/*
	软复位
*/
void Gxht_Restart(void)
{
	IIC_Start(IIC0);
	IIC_SendByte(I2C_TEMP_WRITE_ADDR,IIC0);
	//读指令
	IIC_SendByte(0x30,IIC0);
	IIC_SendByte(0xA2,IIC0);
	IIC_Stop(IIC0);
}

/*
	Gxht30 写读指令
*/
void Gxht_WriteCmd(void)
{
	IIC_Start(IIC0);
	IIC_SendByte(I2C_TEMP_WRITE_ADDR,IIC0);
	//读指令
	IIC_SendByte(0x2C,IIC0);
	IIC_SendByte(0x10,IIC0);
	IIC_Stop(IIC0);
}

/*
	读取温湿度
*/
void Gxht_ReadDat(u16 *temp,u16 *humi)
{
	u8 *tempBuf = malloc(6);
	int temp1 = 0;
	for(u8 i = 0; i < 5; i++)
	{
		tempBuf[i] = IIC_ReadByte(1,IIC0);
	}
	
	tempBuf[5] = IIC_ReadByte(0,IIC0);
	IIC_Stop(IIC0);

	*temp = tempBuf[0] << 8 | tempBuf[1];
	*humi = tempBuf[3] << 8 | tempBuf[4];
	//T = 175 * temp / (2^16 - 1) - 45 两位小数  四舍五入 保留一位小数
	temp1 = (175.0 * (float) *temp / 65535.0 - 45.0 + 0.05) * 10;
	if(temp1 < 0)
	{
		*temp = (0 - temp1) | 0x8000;
	}
	else
	{
		*temp = temp1;
	}
	

	//RH = humi * 100 / (2^16 - 1)  两位小数
	*humi = 100 * ((float) *humi / 65535.0) * 10;
	free(tempBuf);
}

/*
	GXHT3L模块
	获取温湿度
*/
void Gxht_GetTempAndHumi(u16 *temp,u16 *humi)
{
	static u8 setFlag = 0;
	if(setFlag > 3)
	{
		setFlag = 0;
		Gxht_Restart();
		delay1ms(30);
	}
	Gxht_WriteCmd();
	
	delay1ms(4);
	
	IIC_Start(IIC0);
	if(IIC_SendByte(I2C_TEMP_READ_ADDR,IIC0) == 0)
	{
		Gxht_ReadDat(temp,humi);
	}
	else
	{//温湿度转换没有完成 继续转换
		IIC_Stop(IIC0);
		delay1ms(1);
		if(IIC_SendByte(I2C_TEMP_READ_ADDR,IIC0) == 0)
		{
			Gxht_ReadDat(temp,humi);
		}
	}
	if((*temp == 206 && *humi == 127) || *temp == 0 && *humi == 0)
	{
		printf("temperature & humidity Sensor Err\r\n");
		setFlag++;
	}
}
