
#include "rtc.h"
#include "pub.h"
#include "gd32e10x.h"


_CALENDAR_ calendar;
u8 timFlag = 0;
/*
	RTCʱ������
*/
void RTCInit(void)
{
	rcu_periph_clock_enable(RCU_PMU);

	if (bkp_data_read(BKP_DATA_0) != 0xA0A0)	
	{
		/*����󱸵�Դ�洢����*/
		pmu_backup_write_enable();
		/*ʹ��PMU �� BKPIʱ��*/
		rcu_periph_clock_enable(RCU_BKPI);
		bkp_deinit();
		/*ʹ���ⲿ����ʱ��*/
		rcu_osci_on(RCU_LXTAL);
		/*�ȴ��ⲿ����ʱ���ȶ�*/
		rcu_osci_stab_wait(RCU_LXTAL);
		/*ѡȡ�ⲿ����ʱ��ΪRTCʱ��Դ*/
		rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
		
		rcu_periph_clock_enable(RCU_RTC);
		/*RTC �� APB�ӿ�ͬ��*/
		rtc_register_sync_wait();
		/*�ȴ��ϴ�д�������*/
		rtc_lwoff_wait();
		/*�������ж�*/
		rtc_interrupt_enable(RTC_INT_SECOND);
		rtc_lwoff_wait();
		
		/*ʱ��Ƶ������1HZ*/
		rtc_prescaler_set(32767);
		rtc_lwoff_wait();
		/*��������*/
		rtc_configuration_mode_enter();
		RTC_Set(21,9,18,91,37,55);
		/*�˳�����ģʽ*/
		rtc_configuration_mode_exit();
		pmu_backup_write_enable();
		bkp_data_write(BKP_DATA_0,0xA0A0);
		pmu_backup_write_disable();
	}
	else
	{
		rtc_register_sync_wait();
		rtc_lwoff_wait();
		/*�������ж�*/
		rtc_interrupt_enable(RTC_INT_SECOND);
		rtc_lwoff_wait();
	}  				     
	RTC_Get();
	rcu_all_reset_flag_clear();
}

void RTC_IRQHandler(void)
{		
	if(rtc_flag_get(RTC_FLAG_SECOND) != RESET)
	{			
		RTC_Get();
 	}	  	
	rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);
	rtc_lwoff_wait();  	    						 	   	 
}

/*
	����Ķ��壺�ܱ�4���� �ܱ�400���� ���ܱ�100����

	return 1 ��  0 ����
*/
u8 Is_Leap_Year(u16 year)
{
	if(year % 4 == 0)
	{
		if(year % 100 == 0)
		{
			if(year % 400 == 0)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}


const u8 mon_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
/*2000����·�ӳ��*/
const u8 table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 
u8 uDaysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};	

#define IS_LEAPYEAR(y)		(((0 == (y) % 4) && (0 != (y) % 100)) || (0 == (y) % 400))
#define BEGIN_YEAR			2008

#define DAYS_PER_YEAR(y)	(IS_LEAPYEAR(y)? 366u : 365u)
#define DAYS_PER_MONTH(m)   (uDaysPerMonth[(m) - 1])

/*
	��������
*/
u32 RTCTimeToSeconds(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u32 uSeconds = 0;
	u16 uDays;
	u16 uYear;
	u16 uMonth;
	
	uSeconds += sec;
	uSeconds += min * 60;
	uSeconds += hour * 3600;
	
	uDays = sday - 1;
	for (uYear = BEGIN_YEAR; uYear < syear; uYear++)
	{
		uDays += DAYS_PER_YEAR(uYear);
	}
	
	if (IS_LEAPYEAR(uYear))
	{
		DAYS_PER_MONTH(2) = 29;
	}
	else
	{
		DAYS_PER_MONTH(2) = 28;
	}	
	for (uMonth = 1; uMonth < smon; uMonth++)
	{
		uDays += DAYS_PER_MONTH(uMonth);
	}
	
	uSeconds += uDays * 3600 * 24;
	return uSeconds;
}


/*
 ʱ������
*/
u8 RTC_Set(u8 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u32 seccount = RTCTimeToSeconds(syear+2000,smon,sday,hour,min,sec);
	
	rcu_periph_clock_enable(RCU_BKPI);
	rcu_periph_clock_enable(RCU_PMU);
	rtc_lwoff_wait();
	pmu_backup_write_enable();
	rtc_counter_set(seccount);
	pmu_backup_write_disable();
	rtc_lwoff_wait();
	timFlag = 1;
	
	return 0;	
}
/*
 ��ȡʱ��
*/
u8 RTC_Get(void)
{
	u32 secs;
	u16 uDays,uYear,uMonth;

	secs = rtc_counter_get();
	calendar.sec = secs % 60;
	secs = secs / 60;
	calendar.min = secs % 60;
	secs = secs / 60;	
	calendar.hour = secs % 24;		
	uDays = secs / 24;
	for (uYear = BEGIN_YEAR; uDays >= DAYS_PER_YEAR(uYear); uYear++) 
	{
		uDays -= DAYS_PER_YEAR(uYear);
	}
	calendar.w_year = uYear;
	if (IS_LEAPYEAR(uYear))
	{
		DAYS_PER_MONTH(2) = 29;
	}
	else
	{
		DAYS_PER_MONTH(2) = 28;
	}
	for (uMonth = 1; uDays >= DAYS_PER_MONTH(uMonth); uMonth++)
	{
		uDays -= DAYS_PER_MONTH(uMonth);
	}
	calendar.w_month = uMonth;
	calendar.w_day = uDays + 1;
	calendar.week = RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_day);
	return 0;
}

/*
 ��ȡ����
*/
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH,yearL;
	
	yearH = year / 100;	
	yearL = year % 100; 
	/*21����*/
	if (yearH > 19)
	{
		yearL += 100;
	}
	
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7; 
	temp2 = temp2 + day + table_week[month-1];
	if((yearL % 4 == 0) && (month < 3))
	{
		temp2--;
	}
	return(temp2 % 7);
}
