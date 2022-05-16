
#include "gd32e10x.h"
#include "iwdg.h"

/*
 �������Ź�����
 ι��ʱ�� 12.8s
*/
void WatchDogInit(void)
{
	rcu_osci_on(RCU_IRC40K);

	fwdgt_write_enable();
	fwdgt_config(2000,FWDGT_PSC_DIV128);
	fwdgt_counter_reload();
	fwdgt_enable();
}

/*
 �������Ź�����
*/
void WatchDogFeed(void)
{
	fwdgt_counter_reload();
}
