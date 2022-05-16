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
		//延时时间 影响uip通信 数据接收时间不精准 接收多帧数据  
		//进入低功耗模式 任意终端唤醒
		//wait for interrupt 
		__WFI();
	}  
}
