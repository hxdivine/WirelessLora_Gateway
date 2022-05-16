
#ifndef _MODBUS_H_
#define _MODBUS_H_

#include "def.h"

#define	READ_COILS			0x01		//读线圈	
#define READ_DI       	0x02		//读开关
#define READ_HLD_REG  	0x03		//读保持寄存器
#define READ_AI       	0x04		//读模拟
#define SET_COIL      	0x05		//写单线圈
#define SET_HLD_REG   	0x06		//写单寄存器
#define SET_M_COIL    	0x0F		//写多线圈
#define SET_M_REG				0x10		//写多寄存器

//接收到的ModBus-RTU 数据格式
typedef struct{
	u8 DeviceId;			//设备Id
	u8 func;					//功能码
	u8 RegAddrH;			//寄存器地址
	u8 RegAddrL;
	u8 NumH;					//读取数量
	u8 NumL;

}MODBUSRTU_FORMAT;
//接收到的ModBus-TCP 数据格式
typedef struct
{
	u16 SeqId;				//交互序列标识			
	u16 ModbusFlag;		//协议标识
	u8 LenH;					//报文长度
	u8 lenL;		
	u8 DeviceId;			//设备ID
	u8 func;					//功能码
	u8 RegAddrH;			//寄存器地址
	u8 RegAddrL;
	u8 NumH;					//读取数量
	u8 NumL;
}MODBUSTCP_FORMAT;

//需要发送的ModBus 数据格式  RTU 和 TCP通信
typedef struct{

//	u16 SeqId;				//交互序列标识			
//	u16 ModbusFlag;		//协议标识
//	u8 LenH;					//报文长度
//	u8 lenL;
	u8 DeviceId;			//设备Id
	u8 func;					//功能码
	u8 ByteNum;				//读取的字节数
	u8 dataBuf[256];	//返回的数据buf

}MODBUS_RESP;

extern u8 mReplyBuf[270];	//响应Buf 最大256bit

//基地址
#define MODBUS_DEVADDR_BASE		40000
//设备配置信息地址
#define MODBUS_SYSINFO_BASE		43000

extern const unsigned char auchCRCHi[];

extern const unsigned  char auchCRCLo[];

unsigned short ModBusCrc(unsigned char *puchMsg , unsigned short usDataLen);

void AnalyModBus(u8 *InBuf,u8 len,u8 model,u8 *mReplyBuf);

void ReplyModBus(u8 *mReplyBuf,u8 mReplyPtr,u8 mode);

#endif
