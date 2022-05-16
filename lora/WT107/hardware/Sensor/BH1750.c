/*
	��ȡ����ǿ��
	ת����ʽ (float)*ill / 1.2)
*/
#include "BH1750.h"
#include "HC_I2c.h"
#include "ddl.h"
#include "stdlib.h"

#define BHPowDown       0x00      //�ر�ģ��
#define BHPowOn         0x01      //��ģ��
#define BHReset         0x07      //���üĴ�������PowerOnģʽ��Ч
#define BHModeH1        0x10      //�߷ֱ��� ��λ1lx ʱ��120ms
#define BHModeH2        0x11      //�߷ֱ���2 ��λ0.5lx ʱ��120ms
#define BHModeL         0x13      //�ͷֱ��� ��λ4lx ʱ��16ms
#define BHSigModeH      0x20      //һ�θ߷ֱ��� ת����ת�� PowerDown
#define BHSigModeH2     0x21      //ͬ��
#define BHSigModeL      0x23      //ͬ��

//���մ����� �ӻ���ַ 
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
	����ָ��
*/
void BH175_SendCmd(u8 cmd)
{
	IIC_Start(IIC0);
	IIC_SendByte(BH_WRITE_ADDR,IIC0);
	IIC_SendByte(cmd,IIC0);
	IIC_Stop(IIC0);
}
/*
	����ǿ�ȳ�ʼ��
*/
void BH175_Init(void)
{
	BH175_SendCmd(BHPowOn);
	delay1ms(180);
}

/*
	��ָ��
*/
void BH175_ReadData(u8 *buf)
{
	IIC_Start(IIC0);
	//������ַ+���ź�
	IIC_SendByte(BH_READ_ADDR,IIC0);
	//Ӧ��
	buf[0] = IIC_ReadByte(1,IIC0);
	//��Ӧ��
	buf[1] = IIC_ReadByte(0,IIC0);
	
	IIC_Stop(IIC0);

}
/*
	���մ�������ʼ��
*/
void BH175_Ready(void)
{
	BH175_SendCmd(BHPowOn);
	BH175_SendCmd(BHModeH1);
	delay1ms(180);
}

/*
	��ȡ����ǿ��
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
