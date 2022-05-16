#include "systick.h"
#include "string.h"
#include "tapdev.h"

#include "modbus.h"
#include "config.h"
#include "loraDealDat.h"
#include "pub.h"
#include "GSM234G.h"
#include "hardware.h"
//发送缓存区
u8 mReplyBuf[270] = {0};
//modbus-Rtu 数据格式
MODBUSRTU_FORMAT *headRtu;
//modbus-tcp 数据格式
MODBUSTCP_FORMAT *headTcp;
//modbus 响应数据格式
MODBUS_RESP *modbusResp;

/*
	获取设备ID对应的数组下标 以及各种设备对应的地址偏移
*/
u8 GetDevRegAddr(u16 *addr,u8 *Index,u8 id)
{
	u8 rec = 1,i,tempIndex = 0;
	for(i = 0; i < DEVICE_MAX;i++)
	{//找到设备ID 对应的数组下标
		if(id == DevInfo[i].deviceId)
		{
			*Index = i;
			break;
		}
		else if(DevInfo[i].Type == DEV_WT107 || DevInfo[i].Type > DEV_WT101)
		{//WT104后的型号 都按7位数据存储
			tempIndex = i;
		}
	}
	//没找到
	if(i >= DEVICE_MAX)
	{
		*addr += 7;
		*Index = tempIndex;
		return 0;
	}
	//设备类型为WT102 WT101进行跳过
	if(DevInfo[i].Type >= DEV_WT103 && DevInfo[i].Type <= DEV_WT106)
	{
		*addr += 3;
	}
	else if(DevInfo[i].Type >= DEV_WT108 && DevInfo[i].Type <= DEV_WT109)
	{
		*addr += 1;
	}
	else if(DevInfo[i].Type == DEV_WT107 && DevInfo[i].Type > DEV_WT106)
	{
		*addr += 7;
	}
	else
	{
		rec = 0;
	}
	return rec;
}


/*
	获取寄存器地址 与实际读取地址间的偏移
*/
u16 GetDevRegIndex(u16 reg,u8 Index)
{
	u8 offset;
	//设备类型为WT102直接跳过 实际也不可能存在WT102进入此函数
	if(DevInfo[Index].Type == DEV_WT107 || DevInfo[Index].Type >= DEV_WT106)
	{
		offset = reg % 7 ? reg % 7:7;
	}
	else if(DevInfo[Index].Type >= DEV_WT103 && DevInfo[Index].Type <= DEV_WT106)
	{
		offset = reg % 3 ? (reg % 2? 5 : 6): 7;
	}
	else if(DevInfo[Index].Type >= DEV_WT108 && DevInfo[Index].Type <= DEV_WT109)
	{
		offset = 7;
	}
	else
	{
		offset = 0;
	}
	return offset;
}

/*
	用于获取寄存器数据
	根据终端设备的类型进行匹配
*/
void SetReplayBuf(u8 *InBuf,u16 num,u16 regAddr)
{
	for(u8 i = 0,j = 0; i < num; i++)
	{
		InBuf[j++] = DevRegVal[(regAddr + i)] >> 8;
		InBuf[j++] = DevRegVal[(regAddr + i)] & 0xFF;
	}
}


void ChangeNum(u8 *num1,u8 *num2)
{
	u8 t = *num1;
	*num1 = *num2;
	*num2 = t;
}
/*
	将DO状态进行输出 一个设备为一位
*/
u8 CharToByte(u8 *InBuf)
{
	u8 rec = 0;
	
	for(u8 i = 0; i < 8; i++)
	{
		rec = rec << 1 | (InBuf[7 - i] & 0x01);
	}
	return rec;
}



/*
	下发DO设置
*/
void SetDODevBufData(u8 regAddr,u8 option)
{
	u8 devId = regAddr / 2 + 1;
	option = regAddr % 2? option | 0x80 : option;
	DownLoad2TerminalWT2(devId,1,option);
}


/*
	获取DO寄存器数据
*/
void GetDODevBufData(u8 *InBuf,u8 num,u8 regAddr)
{
	u8 staPtr = regAddr / 2;
	u8 i = 0,j = 0;
	u8 tempBuf[8];
	while(i < num)
	{
		for(j = 0; j < 8; j++)
		{
			if(regAddr % 2)
			{
				if(staPtr > DEVICE_MAX)
				{
					tempBuf[j++] = 0;
				}
				else if(DevInfo[staPtr].Type != DEV_WT102)
				{
					tempBuf[j++] = 0;
				}
				else
				{
					tempBuf[j++] = DevRegVal[staPtr * 7 + 1];				
				}
				staPtr++;
				if(staPtr > DEVICE_MAX)
				{
					tempBuf[j] = 0;
				}
				else if(DevInfo[staPtr].Type != DEV_WT102)
				{
					tempBuf[j] = 0;
				}
				else
				{
					tempBuf[j] = DevRegVal[staPtr * 7];				
				}
			}
			else
			{
				if(staPtr > DEVICE_MAX)
				{
					tempBuf[j++] = 0;
					tempBuf[j] = 0;	
				}
				else if(DevInfo[staPtr].Type != DEV_WT102)
				{
					tempBuf[j++] = 0;
					tempBuf[j] = 0;	
				}
				else
				{
					tempBuf[j++] = DevRegVal[staPtr * 7 ];
					tempBuf[j] = DevRegVal[staPtr * 7 + 1];				
				}
				staPtr++;
			}
		}
		InBuf[i++] = CharToByte(tempBuf);
	}
}

/*
	获取DI寄存器数据
*/
void GetDIDevBufData(u8 *InBuf,u8 num,u8 regAddr)
{
	u8 staPtr = regAddr / 4;
	u8 i = 0,j = 0;
	u8 tempBuf[8];
	while(i < num)
	{
		for(j = 0; j < 8; j++)
		{
			switch(regAddr % 4)
			{
			  case 0:
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else
					{
						tempBuf[j++] = DevRegVal[staPtr * 7 ];
						tempBuf[j++] = DevRegVal[staPtr * 7 + 1];
						tempBuf[j++] = DevRegVal[staPtr * 7 + 2];
						tempBuf[j] = DevRegVal[staPtr * 7 + 3];				
					}
					staPtr++;	
					break;
				case 1:
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
					}
					else
					{
						tempBuf[j++] = DevRegVal[staPtr * 7 + 1];
						tempBuf[j++] = DevRegVal[staPtr * 7 + 2];
						tempBuf[j++] = DevRegVal[staPtr * 7 + 3];		
					}
					staPtr++;
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j] = 0;
					}
					else
					{
						tempBuf[j] = DevRegVal[staPtr * 7];			
					}	
					break;
				case 2:
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
					}
					else
					{
						
						tempBuf[j++] = DevRegVal[staPtr * 7 + 2];
						tempBuf[j++] = DevRegVal[staPtr * 7 + 3];		
					}
					staPtr++;
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else
					{
						tempBuf[j++] = DevRegVal[staPtr * 7];		
						tempBuf[j] = DevRegVal[staPtr * 7 + 1];	
					}	
					break;
				default:
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
					}
					else
					{
						tempBuf[j++] = DevRegVal[staPtr * 7 + 3];		
					}
					staPtr++;
					if(staPtr > DEVICE_MAX)
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else if(DevInfo[staPtr].Type != DEV_WT101)	
					{
						tempBuf[j++] = 0;
						tempBuf[j++] = 0;
						tempBuf[j] = 0;
					}
					else
					{
						tempBuf[j++] = DevRegVal[staPtr * 7];		
						tempBuf[j++] = DevRegVal[staPtr * 7 + 1];	
						tempBuf[j] = DevRegVal[staPtr * 7 + 2];
					}	
					break;
			}

		}
		InBuf[i++] = CharToByte(tempBuf);
	}
}

/*
	获取设备在线情况
*/
void GetDevOnlineStatus(u8 *InBuf,u8 num,u8 staPtr)
{
	u8 i = 0,j = 0;
	u8 tempBuf[8];
	while(i < num)
	{
		for(j = 0; j < 8; j++)
		{
			if(staPtr >= DEVICE_MAX)
			{
				tempBuf[j] = 0;
			}
			else if(DevInfo[staPtr].isExist == 1)
			{
				tempBuf[j] = 1;
			}
			else
			{
				tempBuf[j] = 0;
			}
			staPtr += 1;
		}
		InBuf[i++] = CharToByte(tempBuf);
	}
}

/*
	设置相关参数
*/
void SetSysInfoData(u8 addr,u16 num)
{
	switch(addr)
	{
		case 0:
			if(num > 247)
				SysInfo.DeviceId = (u8)num;
		break;
		case 1:
			if(num < 3)
				SysInfo.NetLinkPriority = (u8)num;
		break;
		case 2:
			if(num < 3)
				SysInfo.GprsMode = (u8)num;
		break;
		case 3:
			if(num < 3)
				SysInfo.Rj45Mode = (u8)num;
		break;
		case 4:
			if(num < 2)
				SysInfo.Dhcp = (u8)num;
		break;
		case 5:
		case 6:
		case 7:
		case 8:
			if(num < 256)
				SysInfo.LocalIp[addr - 5] = (u8)num;
		break;
		case 9:
			SysInfo.LocalPort = num;
		break;
		case 10:
		case 11:
		case 12:
		case 13:
			if(num < 256)
			{
				SysInfo.SubNetMask[addr - 10] = (u8)num;
			}
		break;
		case 14:
		case 15:
		case 16:
		case 17:
			if(num < 256)
			SysInfo.GateIp[addr - 14] = (u8)num;
		break;
		case 18:
		case 19:
		case 20:
		case 21:
			if(num < 256)
			SysInfo.DnsIp1[addr - 18] = (u8)num;
		break;
		case 22:
		case 23:
		case 24:
		case 25:
			if(num < 256)
			SysInfo.DnsIp2[addr - 22] = (u8)num;
		break;
		case 26:
			if(num > 0 && num < 50)
			{
				SysInfo.Workfreq = (u8)num;
			}
		break;
		case 27:
			if(num > 0 && num < 26)
			{
				SysInfo.TerminalRepCycle = (u8)num;
			}
		break;
		default:
			break;		
	}
}
/*
	获取设备相关可配置信息
*/
void GetSysInfoData(u8 *InBuf,u8 Addr)
{
	for(u8 i = 0; i < 27; i++)
	{
		InBuf[2 * i] = 0;
	}
	InBuf[1] = SysInfo.DeviceId;
	InBuf[3] = SysInfo.NetLinkPriority;
	InBuf[5] = SysInfo.GprsMode;
	InBuf[7] = SysInfo.Rj45Mode;
	InBuf[9] = SysInfo.Dhcp;
	InBuf[11] = SysInfo.LocalIp[0];
	InBuf[13] = SysInfo.LocalIp[1];
	InBuf[15] = SysInfo.LocalIp[2];
	InBuf[17] = SysInfo.LocalIp[3];
	InBuf[18] = SysInfo.LocalPort >> 8;
	InBuf[19] = SysInfo.LocalPort;
	InBuf[21] = SysInfo.SubNetMask[0];
	InBuf[23] = SysInfo.SubNetMask[1];
	InBuf[25] = SysInfo.SubNetMask[2];
	InBuf[27] = SysInfo.SubNetMask[3];
	InBuf[29] = SysInfo.GateIp[0];
	InBuf[31] = SysInfo.GateIp[1];
	InBuf[33] = SysInfo.GateIp[2];
	InBuf[35] = SysInfo.GateIp[3];
	InBuf[37] = SysInfo.DnsIp1[0];
	InBuf[39] = SysInfo.DnsIp1[1];
	InBuf[41] = SysInfo.DnsIp1[2];
	InBuf[43] = SysInfo.DnsIp1[3];
	InBuf[45] = SysInfo.DnsIp2[0];
	InBuf[47] = SysInfo.DnsIp2[1];
	InBuf[49] = SysInfo.DnsIp2[2];
	InBuf[51] = SysInfo.DnsIp2[3];
	InBuf[53] = SysInfo.Workfreq;
	InBuf[55] = SysInfo.TerminalRepCycle;
	if(Addr == 0)
	{
		return;
	}
	
	for(u8 i = Addr,j = 0; i < 28; i++)
	{
		InBuf[j++] = InBuf[i * 2];
		InBuf[j++] = InBuf[i * 2 + 1];
	}
}



/*
	分析modbus数据格式
	modBus-Rtu: deviceID(1Bit) func(1Bit) reg(2Bit) num(2Bit) crc(1BIt) 
	modBus-Tcp: seq(2Bit)	modbusId(2Bit) MsgLen(2Bit) deviceID(1Bit) func(1Bit) reg(2Bit) num(2bit)
	InBuf:	输入参数
	model:	选择模式	0 关闭 1 GPRS 2 ETH 3 测试
	mReply:	返回参数
*/
void AnalyModBus(u8 *InBuf,u8 len,u8 model,u8 *mReplyBuf)
{
	u16 CrcResult;
	u16 Regaddr;
	u16 tempAddr;
	u16 num;
	u8 mReplyPtr = 0;
	u8 IsRTU = 0;

	if(len < 6)
	{
		return;
	}
	headRtu = (MODBUSRTU_FORMAT*)InBuf;
	headTcp = (MODBUSTCP_FORMAT*)InBuf;
	memset(modbusResp->dataBuf,0,255);
	//进行判断是不是RTU协议
	if((headRtu->func > 0 && headRtu->func <= 6 ) || 
		(headRtu->func >= 0x0F && headRtu->func <= 0x10))
	{
		CrcResult = ModBusCrc(InBuf,len - 2);
		if(CrcResult == (InBuf[len - 2] * 256 + InBuf[len - 1]))
		{//CRC准确 确认时RTU协议
			Regaddr = headRtu->RegAddrH * 256 + headRtu->RegAddrL;
			num = headRtu->NumH * 256 + headRtu->NumL;
			modbusResp = (MODBUS_RESP *)mReplyBuf;
			
			modbusResp->DeviceId = headRtu->DeviceId;
			modbusResp->func = headRtu->func;
			IsRTU = 1;
		}
		else
		{
			IsRTU = 0;
		}
	}
	//TCP协议格式
	if(IsRTU == 0)
	{
		if(headTcp->ModbusFlag == 0)
		{//是对282进行读取 
			if(len < 12)
			{//tcp协议格式最小长度为12
				return;
			}
			Regaddr = headTcp->RegAddrH * 256 + headTcp->RegAddrL;
			num = headTcp->NumH * 256 + headTcp->NumL;
			
			//前6位 TCP格式
			modbusResp = (MODBUS_RESP*)&mReplyBuf[6];
			//为了方便后面程序进行统一判断  将TCP后面相同的结构 转换为RTU结构
			headRtu = (MODBUSRTU_FORMAT*)&InBuf[6];
			
			mReplyBuf[0] = headTcp->SeqId;
			mReplyBuf[1] = headTcp->SeqId >> 8;
			
			mReplyBuf[2] = headTcp->ModbusFlag;
			mReplyBuf[3] = headTcp->ModbusFlag >> 8;
			
			modbusResp->DeviceId = headTcp->DeviceId;
			modbusResp->func = headTcp->func;
		}
		else
		{//不是RTU 也不是TCP 协议
			S282_Log("Error Crc & TCP,turn to lora");
			SendLoraData(headTcp->DeviceId + DEVICE_MAX,model,InBuf,len);
			return;
		}
	}

	if(SysInfo.DeviceId != headRtu->DeviceId)
	{//读取的时WT100 透传下的设备
		SendLoraData(headRtu->DeviceId + DEVICE_MAX,model,InBuf,len);
		return;
	}
	//03 04 功能码
	if(headRtu->func == READ_HLD_REG || headRtu->func == READ_AI)
	{
		//超出发送数据范围
		if(num > 200)
		{
			return;
		}
		//读的是终端设备 采集上来的数据
		if(	(Regaddr >= MODBUS_DEVADDR_BASE) && 
			((Regaddr + num) <= MODBUS_DEVADDR_BASE + MAXREG)	)
		{
			//读取字节个数
			modbusResp->ByteNum = num * 2;
			tempAddr = Regaddr - MODBUS_DEVADDR_BASE;
			SetReplayBuf(modbusResp->dataBuf,num,tempAddr);
			
			mReplyPtr = 3 + modbusResp->ByteNum;
		}
		else if((Regaddr >= MODBUS_SYSINFO_BASE) &&
			((Regaddr + num) < MODBUS_SYSINFO_BASE + 29)	)
		{
			//读设备信息
			tempAddr = Regaddr - MODBUS_SYSINFO_BASE;
			modbusResp->ByteNum = num * 2;
			GetSysInfoData(modbusResp->dataBuf,tempAddr);
			mReplyPtr = 3 + modbusResp->ByteNum;
		}
	}
	else if(headRtu->func == SET_HLD_REG)// 写1 06
	{
		//写设备配置信息
		if(Regaddr >= MODBUS_SYSINFO_BASE && Regaddr < MODBUS_SYSINFO_BASE + 28)
		{
			tempAddr = Regaddr - MODBUS_SYSINFO_BASE;
			SetSysInfoData(tempAddr,num);
			SaveSysInfo();
			if(IsRTU)
			{
				mReplyBuf[2] = headRtu->RegAddrH;
				mReplyBuf[3] = headRtu->RegAddrL;
				mReplyBuf[4] = headRtu->NumH;
				mReplyBuf[5] = headRtu->NumL;
				mReplyPtr = 6; 	
			}	
			else
			{
				modbusResp->ByteNum = 0;
			}    		
    }	
	}
	else if(headRtu->func == SET_M_REG) // 写多REG 16功能码
	{
		if(num <= 127 && Regaddr >= MODBUS_SYSINFO_BASE && ((Regaddr + num) <= MODBUS_SYSINFO_BASE + 28))
		{
    	tempAddr = Regaddr - MODBUS_SYSINFO_BASE;
			u8 base;
			if(IsRTU)
			{
				base = 0;
			}
			else
			{
				base = 6;
			}
			for(u8 i = 0; i < num; i++,tempAddr++)
			{
				SetSysInfoData(tempAddr,InBuf[base + 7 + i * 2] << 8 | InBuf[base + 7 + i * 2 + 1]);
			}
			SaveSysInfo();
			//返回设置成功
			//ack->ByteNo=0;
			//mReplyPtr=3;
			if(IsRTU)
			{
				mReplyBuf[2] = headRtu->RegAddrH;
				mReplyBuf[3] = headRtu->RegAddrL;
				mReplyBuf[4] = headRtu->NumH;
				mReplyBuf[5] = headRtu->NumL;
				mReplyPtr = 6; 	
			}	
			else
			{
				modbusResp->ByteNum = 0;
			} 
		}
	}
	
	else if(headRtu->func == READ_COILS)
	{//01功能码
		if(num > DEVICE_MAX * 2)
		{
			return;
		}
		if((Regaddr + num) <= DEVICE_MAX * 2)
		{//数量为100个  1个WT102 有2路DO输出 
			//读取字节个数
			modbusResp->ByteNum = num % 8 ? num / 8 + 1 : num / 8;
			GetDODevBufData(modbusResp->dataBuf,modbusResp->ByteNum,Regaddr);
			mReplyPtr = 3 + modbusResp->ByteNum;
		}

	}
	else if(headRtu->func == READ_DI)
	{
		//02功能码
		if(Regaddr >= DEVICE_MAX * 4 && (Regaddr + num) <= 5*DEVICE_MAX)
		{
			modbusResp->ByteNum = num % 8 ? num / 8 + 1 : num / 8;
			GetDevOnlineStatus(modbusResp->dataBuf,modbusResp->ByteNum,Regaddr- DEVICE_MAX*4);
			mReplyPtr = 3 + modbusResp->ByteNum;
		}
		else
		{
			if(num > DEVICE_MAX * 4)
			{
				return;
			}
			if((Regaddr + num) <= DEVICE_MAX * 4)
			{//1个WT101有4路DI输入
				modbusResp->ByteNum = num % 8 ? num / 8 + 1 : num / 8;
				GetDIDevBufData(modbusResp->dataBuf,modbusResp->ByteNum,Regaddr);
				mReplyPtr = 3 + modbusResp->ByteNum;
			}
		}
	}
	else if(headRtu->func == SET_COIL)
	{
		if(Regaddr <= 100)
		{
			if(num & 0xFF00)
			{
				SetDODevBufData(Regaddr,1);
			}
		else if(num == 0x00)
		{
			SetDODevBufData(Regaddr,0);
		}
		}
		modbusResp->ByteNum = 0;
		mReplyPtr = 5;
	}
	else if(headRtu->func == SET_M_COIL)
	{
		if(num < 100 && (Regaddr + num) < 300)
		{
			u8 base;
			if(IsRTU)
			{
				base = 0;
			}
			else
			{
				base = 6;
			}
			num /= 8;
			for(u8 i = 0; i < num; i++)
			{
				for(u8 j = 0; j < 8;j++)
				{
					if((Regaddr + num) <= 100)
					{
						if(InBuf[base + 7 + i * 2] & 0x01)
						{
							SetDODevBufData(Regaddr++,1);
						}
						else 
						{
							SetDODevBufData(Regaddr++,0);
						}
					}
					InBuf[base + 7 + i *2] = InBuf[base + 7 + i *2] >> 1;
				}
			}
			//返回设置成功
			//ack->ByteNo=0;
			//mReplyPtr=3;
			if(IsRTU)
			{
				mReplyBuf[2] = headRtu->RegAddrH;
				mReplyBuf[3] = headRtu->RegAddrL;
				mReplyBuf[4] = headRtu->NumH;
				mReplyBuf[5] = headRtu->NumL;
				mReplyPtr = 6; 	
			}	
			else
			{
				modbusResp->ByteNum = 0;
			} 
		}
	}
	
	if(IsRTU)
	{//RTU 协议需要校验位
		CrcResult = ModBusCrc(mReplyBuf,mReplyPtr);
		mReplyBuf[mReplyPtr++] = CrcResult >> 8;
		mReplyBuf[mReplyPtr++]	= CrcResult;
	}
	else
	{//TCP 协议
		if(modbusResp->ByteNum == 0)
		{
			mReplyBuf[4] = 0;
			mReplyBuf[5] = 6;
			mReplyBuf[8] = headRtu->RegAddrH;
			mReplyBuf[9] = headRtu->RegAddrL;
			mReplyBuf[10] = headRtu->NumH;
			mReplyBuf[11] = headRtu->NumL;
			mReplyPtr = 12;
		}
		else
		{
			mReplyBuf[4] = (modbusResp->ByteNum + 3) >> 8;
			mReplyBuf[5] = (modbusResp->ByteNum +3) & 0xFF;
			mReplyPtr = 9 + modbusResp->ByteNum;
		}
	}
	
	if(mReplyPtr == 0)
	{
		SendLoraData(headRtu->DeviceId + DEVICE_MAX,model,InBuf,len);
	}
	ReplyModBus(mReplyBuf,mReplyPtr,model);
}

/*
	Modbus数据响应
*/
void ReplyModBus(u8 *mReplyBuf,u8 mReplyPtr,u8 mode)
{
	if(mode == 1)
	{//gprs
		SendGprsDat(mReplyBuf,mReplyPtr);
  }
  else if(mode == 2)
	{//rj45 server
   	UipServerAck(mReplyBuf,mReplyPtr);
  }
	else if(mode == 3)
	{//rj45 client
    UipClient2Server(mReplyBuf,mReplyPtr);
  }
}
