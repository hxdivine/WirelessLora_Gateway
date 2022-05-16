/*
	���ڶ�ȡ����������̼��ֵ �Լ���������
	ǰʮ���� ��ֵ�̶�Ϊ400 0
	��Ҫÿ���ȡһ��
	
	350~450ppm: ͬ���⻷��
	350~1000ppm:	�������£�����˳��
	>1000ppm: �о��������� ����ʼ���û����˯
*/
#include "Sgp30.h"
#include "HC_I2c.h"
#include "stdlib.h"
#include "ddl.h"

typedef enum SGP30_CMD_EN{
	/*��ʼ����������*/
	INIT_AIR_QUALITY = 0x2003,
	/*��ʼ������������*/
	MEASURE_AIR_QUALITY = 0x2008
}sgp30_cmd_t;

#define CRC8_POLYNOMIAL 0x31

//CO2������  �ӻ���ַ 0101 1000
#define I2C_CO2ADDR		0x58
#define	SGP30_ADDR_WRITE	I2C_CO2ADDR << 1       //0xb0
#define	SGP30_ADDR_READ		(I2C_CO2ADDR << 1) + 1   //0xb1

/*
	CRC-8 У�麯��
*/
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;	    //����
    uint8_t  i = 0, j = 0;

    /* ��ʼ�� */
    remainder = initial_value;

    for(j = 0; j < 2; j++)
    {
        remainder ^= message[j];

        /* ����߿�ʼ ���μ���  */
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

    /* CRC�� */
    return remainder;
}

/*
	�����λ
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
//	//�����豸��ַ+��д���� (w= 0 r= 1)
	IIC_SendByte(SGP30_ADDR_WRITE,IIC1);		
	IIC_SendByte((u8)(cmd >> 8),IIC1);
	IIC_SendByte((u8)cmd,IIC1);
	IIC_Stop(IIC1);	
}

/*
	SGP30 ��ʼ��
*/
void SGP30_Init(void)
{
	SGP30_Reset();
	delay1ms(100);
	SGP30_SendCmd(INIT_AIR_QUALITY);
	delay1ms(100);
}

/*
	��ȡCO2����
*/
void SGP30_GetCo2Data(u16 *tempCo2,u16 *tempTvoc)
{
	static u8 failCnt = 0;
	u8 *tempBuf = malloc(6);
	if(failCnt > 5)
	{//����
		SGP30_Init();
		failCnt = 0;
	}
	//д
	SGP30_SendCmd(MEASURE_AIR_QUALITY);
	delay1ms(110);
	IIC_Start(IIC1);
	//��
	IIC_SendByte(SGP30_ADDR_READ,IIC1);
	//CO2 ����λ TVOC ����λ
	for(u8 i = 0; i < 5; i++)
	{
		tempBuf[i] = IIC_ReadByte(1,IIC1);
	}
	tempBuf[5] = IIC_ReadByte(0,IIC1);
	
	IIC_Stop(IIC1);
	
	if(CheckCrc8(&tempBuf[0],0xFF) != tempBuf[2])
	{//CO2У��ʧ��
		printf("Read Co2 Fail\r\n");
		failCnt++;
		return;
	}
	
	if(CheckCrc8(&tempBuf[3],0xFF) != tempBuf[5])
	{//TVOC У��ʧ��
		printf("Co2 Sensor Err\r\n");
		failCnt++;
		return;
	}
	*tempCo2 = tempBuf[0] << 8 | tempBuf[1];

	*tempTvoc = tempBuf[3] << 8  | tempBuf[4];

	
	if(tempBuf[0] == 0x00)
	{//CO2 ��ֵ������Ϊ0
		failCnt++;
	}
	else
	{
		failCnt = 0;
	}
	free(tempBuf);
}
