
#include "ddl.h"
#include "gpio.h"
#include "timer3.h"
#include "HC_Time3.h"
#include "hardware.h"
#include "SysConfig.h"
/*
	定时器3初始化 用于计数

	u16Period * (1/4M) * 16
	12500 * (1 / 4 M) * 16 = 50000us = 50ms
*/
void Time3Init(u16 u16Period)
{
	uint16_t u16Value;
	stc_tim3_mode0_cfg_t stcTim3BaseCfg;
	//结构体初始化清零
	DDL_ZERO_STRUCT(stcTim3BaseCfg);

	stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;//定时器模式
	stcTim3BaseCfg.enCT = Tim3Timer;//定时器功能 计数时钟为内部PCLK
	stcTim3BaseCfg.enPRS = Tim3PCLKDiv64;//16分频
	stcTim3BaseCfg.enCntMode = Tim316bitArrMode;//自动重装载16位计数器
	stcTim3BaseCfg.bEnGate = FALSE;
	stcTim3BaseCfg.bEnTog = FALSE;
	stcTim3BaseCfg.enGateP = Tim3GatePositive;
	//TIM3 的模式0功能初始化
	Tim3_Mode0_Init(&stcTim3BaseCfg);
	u16Value = 0x10000 - u16Period;
	//设置重装载值 
	Tim3_M0_ARRSet(u16Value);
	//设置计数值
	Tim3_M0_Cnt16Set(u16Value);
	Tim3_ClearIntFlag(Tim3UevIrq);//清除中断标志
	Tim3_Mode0_EnableIrq();//使能TIM3中断
	
	EnableNvic(TIM3_IRQn,IrqLevel1,TRUE);
}
/*
	开始定时器计数
*/
void TimeCntStart(void)
{
	Tim3_M0_Run();
}
/*
	定时器中断
*/
void TIM3_IRQHandler(void)
{
	//计时溢出
	if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
	{
		RepCnt++;
		if(RepCnt >= 1200 * DevInfo.RepCycle)
		{
			RepCnt = 0;
		}
		//中断标志清除
		Tim3_ClearIntFlag(Tim3UevIrq);
	}
}
