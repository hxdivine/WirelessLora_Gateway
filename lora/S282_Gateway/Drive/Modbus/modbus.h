
#ifndef _MODBUS_H_
#define _MODBUS_H_

#include "def.h"

#define	READ_COILS			0x01		//����Ȧ	
#define READ_DI       	0x02		//������
#define READ_HLD_REG  	0x03		//�����ּĴ���
#define READ_AI       	0x04		//��ģ��
#define SET_COIL      	0x05		//д����Ȧ
#define SET_HLD_REG   	0x06		//д���Ĵ���
#define SET_M_COIL    	0x0F		//д����Ȧ
#define SET_M_REG				0x10		//д��Ĵ���

//���յ���ModBus-RTU ���ݸ�ʽ
typedef struct{
	u8 DeviceId;			//�豸Id
	u8 func;					//������
	u8 RegAddrH;			//�Ĵ�����ַ
	u8 RegAddrL;
	u8 NumH;					//��ȡ����
	u8 NumL;

}MODBUSRTU_FORMAT;
//���յ���ModBus-TCP ���ݸ�ʽ
typedef struct
{
	u16 SeqId;				//�������б�ʶ			
	u16 ModbusFlag;		//Э���ʶ
	u8 LenH;					//���ĳ���
	u8 lenL;		
	u8 DeviceId;			//�豸ID
	u8 func;					//������
	u8 RegAddrH;			//�Ĵ�����ַ
	u8 RegAddrL;
	u8 NumH;					//��ȡ����
	u8 NumL;
}MODBUSTCP_FORMAT;

//��Ҫ���͵�ModBus ���ݸ�ʽ  RTU �� TCPͨ��
typedef struct{

//	u16 SeqId;				//�������б�ʶ			
//	u16 ModbusFlag;		//Э���ʶ
//	u8 LenH;					//���ĳ���
//	u8 lenL;
	u8 DeviceId;			//�豸Id
	u8 func;					//������
	u8 ByteNum;				//��ȡ���ֽ���
	u8 dataBuf[256];	//���ص�����buf

}MODBUS_RESP;

extern u8 mReplyBuf[270];	//��ӦBuf ���256bit

//����ַ
#define MODBUS_DEVADDR_BASE		40000
//�豸������Ϣ��ַ
#define MODBUS_SYSINFO_BASE		43000

extern const unsigned char auchCRCHi[];

extern const unsigned  char auchCRCLo[];

unsigned short ModBusCrc(unsigned char *puchMsg , unsigned short usDataLen);

void AnalyModBus(u8 *InBuf,u8 len,u8 model,u8 *mReplyBuf);

void ReplyModBus(u8 *mReplyBuf,u8 mReplyPtr,u8 mode);

#endif
