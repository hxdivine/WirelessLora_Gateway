#include "gd32e10x.h"
#include "systick.h"

#include "hardware.h"
#include "dealDat.h" 
#include "netLink.h"
#include "iwdg.h"

int main(void)
{
	u32 temTimCnt;
	systick_config();
	TotalInit();

	while(1)
	{
		HardwareCheck(&temTimCnt);
		NetPro();
		WatchDogFeed();
		PcCmdPro();
		//��ʱʱ�� Ӱ��uipͨ�� ���ݽ���ʱ�䲻��׼ ���ն�֡����  
		//����͹���ģʽ �����ն˻���
		//wait for interrupt 
		__WFI();
	}  
}
