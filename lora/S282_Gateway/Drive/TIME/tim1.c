
#include "tim1.h"
#include "gd32e10x.h"
#include "stdio.h"
#include "hardware.h"

/*
	∂® ±∆˜1 ≥ı ºªØ
	Tout = ((per + 1) * (psc + 1)) / Tclk
*/
void InitTimer1(u16 psc,u32 per)
{
/* ----------------------------------------------------------------------------
    TIMER1 Configuration: 
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
    ---------------------------------------------------------------------------- 
*/
   
	timer_parameter_struct timer_InitPara;
  rcu_periph_clock_enable(RCU_TIMER1);
	timer_deinit(TIMER1);
	
	timer_InitPara.prescaler = psc;
	timer_InitPara.counterdirection = TIMER_COUNTER_UP;
	timer_InitPara.period = per;
	timer_InitPara.clockdivision = TIMER_CKDIV_DIV4;
//	timer_InitPara.repetitioncounter = 0;
	timer_InitPara.alignedmode = TIMER_COUNTER_EDGE;
	timer_init(TIMER1,&timer_InitPara);
	
	timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
  timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    
	timer_enable(TIMER1);
}

void TIMER1_IRQHandler(void)
{
	if(timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP) == SET)
	{
		if(NetLedCnt > 0)
		{//EthLed…¡À∏
			NetLedCnt--;
			NETLEDON
			if(NetLedCnt == 0)
			{
				NETLEDOFF
			}
		}
		if(LoraLedCnt > 0)
		{//LoRaLed…¡À∏
			LoraLedCnt--;
			RFLEDON	
			if(LoraLedCnt == 0)
			{
				RFLEDOFF
			}
		}
		GprsLedCnt--;
		if(GprsLedCnt > GprsLedEnd)
		{//GPrsLed…¡À∏
			GPRSLEDON
		}
		else 
		{
			GPRSLEDOFF
			if(GprsLedCnt == 0)
			{
				GprsLedCnt = GprsLedStart + GprsLedEnd;
			}
		}
		
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
	}
}
