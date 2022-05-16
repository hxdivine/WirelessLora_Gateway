
#include "HC_Wdt.h"
#include "ddl.h"

void WdtInit(void)
{
	Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
	//3.28s
	Wdt_Init(WdtResetEn,WdtT13s1);
}	

void WdtFeedDog(void)
{
	Wdt_Feed();
}
