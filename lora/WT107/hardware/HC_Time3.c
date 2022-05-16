
#include "ddl.h"
#include "gpio.h"
#include "timer3.h"
#include "HC_Time3.h"
#include "hardware.h"
#include "SysConfig.h"
/*
	��ʱ��3��ʼ�� ���ڼ���

	u16Period * (1/4M) * 16
	12500 * (1 / 4 M) * 16 = 50000us = 50ms
*/
void Time3Init(u16 u16Period)
{
	uint16_t u16Value;
	stc_tim3_mode0_cfg_t stcTim3BaseCfg;
	//�ṹ���ʼ������
	DDL_ZERO_STRUCT(stcTim3BaseCfg);

	stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;//��ʱ��ģʽ
	stcTim3BaseCfg.enCT = Tim3Timer;//��ʱ������ ����ʱ��Ϊ�ڲ�PCLK
	stcTim3BaseCfg.enPRS = Tim3PCLKDiv64;//16��Ƶ
	stcTim3BaseCfg.enCntMode = Tim316bitArrMode;//�Զ���װ��16λ������
	stcTim3BaseCfg.bEnGate = FALSE;
	stcTim3BaseCfg.bEnTog = FALSE;
	stcTim3BaseCfg.enGateP = Tim3GatePositive;
	//TIM3 ��ģʽ0���ܳ�ʼ��
	Tim3_Mode0_Init(&stcTim3BaseCfg);
	u16Value = 0x10000 - u16Period;
	//������װ��ֵ 
	Tim3_M0_ARRSet(u16Value);
	//���ü���ֵ
	Tim3_M0_Cnt16Set(u16Value);
	Tim3_ClearIntFlag(Tim3UevIrq);//����жϱ�־
	Tim3_Mode0_EnableIrq();//ʹ��TIM3�ж�
	
	EnableNvic(TIM3_IRQn,IrqLevel1,TRUE);
}
/*
	��ʼ��ʱ������
*/
void TimeCntStart(void)
{
	Tim3_M0_Run();
}
/*
	��ʱ���ж�
*/
void TIM3_IRQHandler(void)
{
	//��ʱ���
	if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
	{
		RepCnt++;
		if(RepCnt >= 1200 * DevInfo.RepCycle)
		{
			RepCnt = 0;
		}
		//�жϱ�־���
		Tim3_ClearIntFlag(Tim3UevIrq);
	}
}
