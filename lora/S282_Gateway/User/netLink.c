#include "netLink.h"
#include "EthNet.h"
#include "GprsNet.h"
#include "config.h"
#include "dealDat.h"
#include "loraDealDat.h"
#include "hardware.h"
#include "iwdg.h"
#include "pub.h"

u8 NetSelected = 0;
/*
	硬件检测
*/
void HardwareCheck(u32 *temTimCnt)
{
	if(GetSysTimCnt() - *temTimCnt > 10)
	{//10ms进入一次
		*temTimCnt = GetSysTimCnt();
		CheckRefactory();
		CheckPower();
		ChekPwd();
		if(freqChangeFlag == 1)
		{//频段更改 在一个上报周期 + 2min 内更改所有终端上报频段
			if(ABSVALUE(calendar.min,freqChangeTim) >= SysInfo.TerminalRepCycle * 2 + 2)
			{
				SysInfo.Workfreq = tempWorkfreq;
				freqChangeFlag = 0;
				SetWorkFreq();
			}
		}
	}
}

/*
	以太网 GPRS模块检测
*/
void NetPro(void)
{
	NorPro();
	GprsRunning();
}

/*
	以太网检测
	LORA检测
*/
void NorPro(void)
{
	if(SysInfo.Rj45Mode & 0x0F)
	{
		NetRunning();	//处理uip事件，必须插入到用户程序的循环体中  
	}
	LoraPro();
	NewDevOffline();
	WatchDogFeed();
	PcCmdPro();
}
