/*
	读取光照强度
	转换公式 (float)*ill / 1.2)
*/
#include "BH1750.h"
#include "HC_I2c.h"
#include "ddl.h"
#include "stdlib.h"

#define BHPowDown       0x00      //关闭模块
#define BHPowOn         0x01      //打开模块
#define BHReset         0x07      //重置寄存器数据PowerOn模式有效
#define BHModeH1        0x10      //高分辨率 单位1lx 时间120ms
#define BHModeH2        0x11      //高分辨率2 单位0.5lx 时间120ms
#define BHModeL         0x13      //低分辨率 单位4lx 时间16ms
#define BHSigModeH      0x20      //一次高分辨率 转换后转至 PowerDown
#define BHSigModeH2     0x21      //同上
#define BHSigModeL      0x23      //同上

//光照传感器 从机地址 
#define I2C_ILLADDR		0x23
#define BH_WRITE_ADDR		I2C_ILLADDR << 1
#define BH_READ_ADDR		I2C_ILLADDR << 1 | 0x01

void IIC_Restart(void)
{
	IIC_Start(IIC0);
	IIC_SendByte(0x00,IIC0);
	IIC_ReadByte(0x06,IIC0);
	IIC_Stop(IIC0);
}
/*
	发送指令
*/
void BH175_SendCmd(u8 cmd)
{
	IIC_Start(IIC0);
	IIC_SendByte(BH_WRITE_ADDR,IIC0);
	IIC_SendByte(cmd,IIC0);
	IIC_Stop(IIC0);
}
/*
	光照强度初始化
*/
void BH175_Init(void)
{
	BH175_SendCmd(BHPowOn);
	delay1ms(180);
}

/*
	读指令
*/
void BH175_ReadData(u8 *buf)
{
	IIC_Start(IIC0);
	//器件地址+读信号
	IIC_SendByte(BH_READ_ADDR,IIC0);
	//应答
	buf[0] = IIC_ReadByte(1,IIC0);
	//非应答
	buf[1] = IIC_ReadByte(0,IIC0);
	
	IIC_Stop(IIC0);

}
/*
	光照传感器初始化
*/
void BH175_Ready(void)
{
	BH175_SendCmd(BHPowOn);
	BH175_SendCmd(BHModeH1);
	delay1ms(180);
}

/*
	获取光照强度
*/
void BH175_GetIllData(u16 *ill)
{
	u8 *tempBuf = malloc(3);
	BH175_Ready();
	BH175_ReadData(tempBuf);

	//ill / 1.2 
	*ill = tempBuf[0];
	*ill = (*ill << 8) + tempBuf[1];
	*ill = ((float)*ill / 1.2);
	
	if(*ill == 54612)
	{
		printf("ILLU Sensor Err\r\n");
		IIC_Restart();
		delay1ms(100);
		*ill = 0;
	}
	free(tempBuf);
}
