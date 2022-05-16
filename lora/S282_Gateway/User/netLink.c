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
	Ӳ�����
*/
void HardwareCheck(u32 *temTimCnt)
{
	if(GetSysTimCnt() - *temTimCnt > 10)
	{//10ms����һ��
		*temTimCnt = GetSysTimCnt();
		CheckRefactory();
		CheckPower();
		ChekPwd();
		if(freqChangeFlag == 1)
		{//Ƶ�θ��� ��һ���ϱ����� + 2min �ڸ��������ն��ϱ�Ƶ��
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
	��̫�� GPRSģ����
*/
void NetPro(void)
{
	NorPro();
	GprsRunning();
}

/*
	��̫�����
	LORA���
*/
void NorPro(void)
{
	if(SysInfo.Rj45Mode & 0x0F)
	{
		NetRunning();	//����uip�¼���������뵽�û������ѭ������  
	}
	LoraPro();
	NewDevOffline();
	WatchDogFeed();
	PcCmdPro();
}
