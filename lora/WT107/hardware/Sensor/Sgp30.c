/*
	用于读取大气二氧化碳数值 以及环境质量
	前十五秒 数值固定为400 0
	需要每秒读取一次
	
	350~450ppm: 同室外环境
	350~1000ppm:	空气清新，呼吸顺畅
	>1000ppm: 感觉空气浑浊 并开始觉得昏昏欲睡
*/
#include "Sgp30.h"
#include "HC_I2c.h"
#include "stdlib.h"
#include "ddl.h"

typedef enum SGP30_CMD_EN{
	/*初始化空气质量*/
	INIT_AIR_QUALITY = 0x2003,
	/*开始空气质量测量*/
	MEASURE_AIR_QUALITY = 0x2008
}sgp30_cmd_t;

#define CRC8_POLYNOMIAL 0x31

//CO2传感器  从机地址 0101 1000
#define I2C_CO2ADDR		0x58
#define	SGP30_ADDR_WRITE	I2C_CO2ADDR << 1       //0xb0
#define	SGP30_ADDR_READ		(I2C_CO2ADDR << 1) + 1   //0xb1

/*
	CRC-8 校验函数
*/
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;	    //余数
    uint8_t  i = 0, j = 0;

    /* 初始化 */
    remainder = initial_value;

    for(j = 0; j < 2; j++)
    {
        remainder ^= message[j];

        /* 从最高开始 依次计算  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1) ^ CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /* CRC码 */
    return remainder;
}

/*
	软件复位
*/
void SGP30_Reset(void)
{
	IIC_Start(IIC1);
	IIC_SendByte(0x00,IIC1);
	IIC_ReadByte(0x06,IIC1);
	IIC_Stop(IIC1);
}

void SGP30_SendCmd(sgp30_cmd_t cmd)
{
	IIC_Start(IIC1);
//	//发送设备地址+读写控制 (w= 0 r= 1)
	IIC_SendByte(SGP30_ADDR_WRITE,IIC1);		
	IIC_SendByte((u8)(cmd >> 8),IIC1);
	IIC_SendByte((u8)cmd,IIC1);
	IIC_Stop(IIC1);	
}

/*
	SGP30 初始化
*/
void SGP30_Init(void)
{
	SGP30_Reset();
	delay1ms(100);
	SGP30_SendCmd(INIT_AIR_QUALITY);
	delay1ms(100);
}

/*
	获取CO2数据
*/
void SGP30_GetCo2Data(u16 *tempCo2,u16 *tempTvoc)
{
	static u8 failCnt = 0;
	u8 *tempBuf = malloc(6);
	if(failCnt > 5)
	{//重启
		SGP30_Init();
		failCnt = 0;
	}
	//写
	SGP30_SendCmd(MEASURE_AIR_QUALITY);
	delay1ms(110);
	IIC_Start(IIC1);
	//读
	IIC_SendByte(SGP30_ADDR_READ,IIC1);
	//CO2 检验位 TVOC 检验位
	for(u8 i = 0; i < 5; i++)
	{
		tempBuf[i] = IIC_ReadByte(1,IIC1);
	}
	tempBuf[5] = IIC_ReadByte(0,IIC1);
	
	IIC_Stop(IIC1);
	
	if(CheckCrc8(&tempBuf[0],0xFF) != tempBuf[2])
	{//CO2校验失败
		printf("Read Co2 Fail\r\n");
		failCnt++;
		return;
	}
	
	if(CheckCrc8(&tempBuf[3],0xFF) != tempBuf[5])
	{//TVOC 校验失败
		printf("Co2 Sensor Err\r\n");
		failCnt++;
		return;
	}
	*tempCo2 = tempBuf[0] << 8 | tempBuf[1];

	*tempTvoc = tempBuf[3] << 8  | tempBuf[4];

	
	if(tempBuf[0] == 0x00)
	{//CO2 数值不可能为0
		failCnt++;
	}
	else
	{
		failCnt = 0;
	}
	free(tempBuf);
}
