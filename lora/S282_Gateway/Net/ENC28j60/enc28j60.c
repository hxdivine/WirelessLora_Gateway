
#include "gd32e10x.h"
#include "systick.h"
#include "enc28j60.h"
#include "spi.h"
#include "stdio.h"

static u8 ENC28J60BANK;
static u32 NextPacketPtr;

/*网口掉线标志*/
u8 EthLineIsOut;
/*
	SPI 片选 复位
*/
void ENC28J60_Reset(void)
{
	//传输启动
	Spi1ReadWriteByte(0xFF);
	
	ENC28J60RST_L	//复位
	delayMs(10);
	ENC28J60RST_H
	delayMs(10);
}

/*
	读取ENC28J60寄存器
	op: 操作码
	addr：寄存器地址
*/
u8 ENC28J60_Read_Op(u8 op,u8 addr)
{
	u8 dat = 0;
	
	ENC28J60CS_L
	dat = op | (addr & ADDR_MASK);
	Spi1ReadWriteByte(dat);
	dat = Spi1ReadWriteByte(0xFF);
	//do dummy read if needed (for mac and mii, see datasheet page 29);
	if(addr & 0x80)
	{
		dat = Spi1ReadWriteByte(0xFF);
	}
	ENC28J60CS_H
	return dat;
}


/*
	写入ENC28J60寄存器
	op: 操作码
	addr：寄存器地址
	date 写入数据
*/
void ENC28J60_Write_Op(u8 op,u8 addr,u8 data)
{
	u8 dat = 0;
	ENC28J60CS_L
	dat = op | (addr & ADDR_MASK);
	Spi1ReadWriteByte(dat);
	Spi1ReadWriteByte(data);
	ENC28J60CS_H
}
/*
	读取ENC28J60接收数据缓存区
*/
void ENC28J60_Read_Buf(u32 len,u8* data)
{
	ENC28J60CS_L
	Spi1ReadWriteByte(ENC28J60_READ_BUF_MEM);
	while(len)
	{
		len--;
		*data = (u8)Spi1ReadWriteByte(0);
		data++;
	}
	*data = '\0';
	ENC28J60CS_H
}
/*
	写入ENC28J60发送数据缓存区
*/
void ENC28J60_Write_Buf(u32 len,u8* data)
{
	ENC28J60CS_L
	Spi1ReadWriteByte(ENC28J60_WRITE_BUF_MEM);
	while(len)
	{
		len--;
		Spi1ReadWriteByte(*data);
		data++;
	}
	ENC28J60CS_H
}

/*
	设置寄存器BANK
*/
void ENC28J60_Set_Bank(u8 bank)
{
	if((bank & BANK_MASK) != ENC28J60BANK)
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,(ECON1_BSEL1 | ECON1_BSEL0));
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,(bank & BANK_MASK) >> 5);
		ENC28J60BANK=(bank&BANK_MASK);
	}
}
/*
 读指定寄存器值
*/
u8 ENC28J60_Read(u8 addr)
{
	ENC28J60_Set_Bank(addr);	 
	return ENC28J60_Read_Op(ENC28J60_READ_CTRL_REG,addr);
}
/*
	写指定寄存器
*/
void ENC28J60_Write(u8 addr,u8 data)
{
	ENC28J60_Set_Bank(addr);		 
	ENC28J60_Write_Op(ENC28J60_WRITE_CTRL_REG,addr,data);
}

/*
	读PHY寄存器
*/
u16 ENC28J60_PHY_Read(u8 addr)
{
	u16 Result,i;
   
	ENC28J60_Write(MIREGADR, addr);
	
	ENC28J60_Write(MICMD, MICMD_MIIRD);
	for(i=0;i<6000;i++);

	// wait until the PHY read completes
	while(ENC28J60_Read(MISTAT) & MISTAT_BUSY);

	// reset reading bit
	ENC28J60_Write(MICMD, 0x00);
	Result = ENC28J60_Read(MIRDH) <<8;
	Result |= ENC28J60_Read(MIRDL) ;
	return Result;
}

/*
 向ENC28J60的PHY寄存器写入数据
*/
void ENC28J60_PHY_Write(u8 addr,u32 data)
{
	u16 retry=0;
	ENC28J60_Write(MIREGADR,addr);
	ENC28J60_Write(MIWRL,data);
	ENC28J60_Write(MIWRH,data>>8);		   
	while((ENC28J60_Read(MISTAT) & MISTAT_BUSY) && retry < 0XFFF)
	{
		retry++;
	}
}
/*
 初始化ENC28J60  传入 MAC地址
*/
u8 ENC28J60_Init(u8* macaddr)
{
	u16 retry=0;		  
	ENC28J60_Reset();
	ENC28J60_Write_Op(ENC28J60_SOFT_RESET,0,ENC28J60_SOFT_RESET);//软件复位
	while(!(ENC28J60_Read(ESTAT) & ESTAT_CLKRDY) && retry < 500)//等待时钟稳定
	{
		retry++;
		delayMs(1);
	};
	if(retry >= 500)
		return 1;//ENC28J60初始化失败
	NextPacketPtr = RXSTART_INIT;
	//接收起始字节
	ENC28J60_Write(ERXSTL,RXSTART_INIT & 0xFF);	
	ENC28J60_Write(ERXSTH,RXSTART_INIT >> 8);	  
	//接收读指针字节
	ENC28J60_Write(ERXRDPTL,RXSTART_INIT & 0xFF);
	ENC28J60_Write(ERXRDPTH,RXSTART_INIT >> 8);
	//接收接收字节
	ENC28J60_Write(ERXNDL,RXSTOP_INIT & 0xFF);
	ENC28J60_Write(ERXNDH,RXSTOP_INIT >> 8);
	//发送起始字节
	ENC28J60_Write(ETXSTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(ETXSTH,TXSTART_INIT >> 8);
	//发送结束字节
	ENC28J60_Write(ETXNDL,TXSTOP_INIT & 0xFF);
	ENC28J60_Write(ETXNDH,TXSTOP_INIT >> 8);
	
	ENC28J60_Write(ERXFCON,ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
	ENC28J60_Write(EPMM0,0x3f);
	ENC28J60_Write(EPMM1,0x30);
	ENC28J60_Write(EPMCSL,0xf9);
	ENC28J60_Write(EPMCSH,0xf7);
	
	ENC28J60_Write(MACON1,MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	
	ENC28J60_Write(MACON2,0x00);
	
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,MACON3,MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);

	ENC28J60_Write(MAIPGL,0x12);
	ENC28J60_Write(MAIPGH,0x0C);
	
	ENC28J60_Write(MABBIPG,0x15);
	
	ENC28J60_Write(MAMXFLL,MAX_FRAMELEN & 0xFF);	
	ENC28J60_Write(MAMXFLH,MAX_FRAMELEN >> 8);
	//设置MAC地址
	ENC28J60_Write(MAADR5,macaddr[0]);	
	ENC28J60_Write(MAADR4,macaddr[1]);
	ENC28J60_Write(MAADR3,macaddr[2]);
	ENC28J60_Write(MAADR2,macaddr[3]);
	ENC28J60_Write(MAADR1,macaddr[4]);
	ENC28J60_Write(MAADR0,macaddr[5]);
	//配置PHY为全双工
	ENC28J60_PHY_Write(PHCON1,PHCON1_PDPXMD);	

	ENC28J60_PHY_Write(PHCON2,PHCON2_HDLDIS);
	
	ENC28J60_Set_Bank(ECON1);
	
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE | EIE_PKTIE);
	
	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_RXEN);
	
	for(u16 i = 0; i < 500; i++)
	{
		delayMs(3);
		if(ENC28J60_PHY_Read(PHSTAT2) & 0x400)
		{
			EthLineIsOut = 0;
			break;
		}
		else
		{
			EthLineIsOut = 1;
		}
	}
	
	if(ENC28J60_Read(MAADR5)== macaddr[0])
	{
		return 0;//初始化成功
	}
	else
	{
		return 1;
	}		
}
/*
 读取EREVID
*/
u8 ENC28J60_Get_EREVID(void)
{
	return ENC28J60_Read(EREVID);
}

/*
	发送数据包到网路
*/
void ENC28J60_Packet_Send(u32 len,u8* packet)
{
	ENC28J60_Write(EWRPTL,TXSTART_INIT & 0xFF);
	ENC28J60_Write(EWRPTH,TXSTART_INIT >> 8);
  
	ENC28J60_Write(ETXNDL,(TXSTART_INIT + len) & 0xFF);
	ENC28J60_Write(ETXNDH,(TXSTART_INIT + len) >> 8);

	ENC28J60_Write_Op(ENC28J60_WRITE_BUF_MEM,0,0x00);

 	ENC28J60_Write_Buf(len,packet);

	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON1,ECON1_TXRTS);

	if((ENC28J60_Read(EIR)&EIR_TXERIF))
	{
		ENC28J60_Write_Op(ENC28J60_BIT_FIELD_CLR,ECON1,ECON1_TXRTS);
	}
}
							  
/*
 从网络接收数据包
*/
u32 ENC28J60_Packet_Receive(u32 maxlen,u8* packet)
{
	u32 rxstat;
	u32 len;    

	if(ENC28J60_PHY_Read(PHSTAT2) & 0x400)
	{
		EthLineIsOut = 0;
	}
	else 
	{
		EthLineIsOut = 1;
	}
	
	if(ENC28J60_Read(EPKTCNT) == 0)
	{
		return 0;
	}
	ENC28J60_Write(ERDPTL,(NextPacketPtr));
	ENC28J60_Write(ERDPTH,(NextPacketPtr) >> 8);	   

	NextPacketPtr = ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	NextPacketPtr |= ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0) << 8;

	len = ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	len |= ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0) << 8;
 	len -= 4; 

	rxstat = ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0);
	rxstat |= ENC28J60_Read_Op(ENC28J60_READ_BUF_MEM,0) << 8;

	if(len > maxlen - 1)
	{
		len = maxlen - 1;	
	}
	
	if((rxstat & 0x80) == 0)
	{
		len = 0;
	}
	else
	{
		ENC28J60_Read_Buf(len,packet); 
	}
	ENC28J60_Write(ERXRDPTL,(NextPacketPtr));
	ENC28J60_Write(ERXRDPTH,(NextPacketPtr) >> 8);
	
 	ENC28J60_Write_Op(ENC28J60_BIT_FIELD_SET,ECON2,ECON2_PKTDEC);
	return(len);
}

/*
 网口掉线检测
*/
u8 ETHLineOut(void)
{  
	static u8 OldState = 0x55;
	if(OldState != EthLineIsOut)
	{
		OldState = EthLineIsOut;
//		if(EthLineIsOut == 1)
//		{	
//			printf("warnning:CheckEthLine\r\n");
//		}
//		else
//		{
//			printf("NetLineIn\r\n");
//		}
	}
	return EthLineIsOut;
}

