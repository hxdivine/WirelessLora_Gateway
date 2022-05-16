
#include "HC_Flash.h"
#include "ddl.h"
//��?o����???����??
#define DEV_ADDR	0xFF00

/*
	2��D�䨺y?Y��??��2?Flash
*/
void FlashWriteData(uint8_t *InBuf,u16 writeLen)
{
	
	uint32_t u32Addr = DEV_ADDR;
	Flash_SectorErase(u32Addr);
	//?��D���?��y?YD?����512
	for(u8 i = 0; i < writeLen; i++)
	{
		Flash_WriteByte(u32Addr,InBuf[i]);
		u32Addr++;
	}
	
}
/*
	?����??��2?Flash��y?Y
*/
void FlashReadData(uint8_t *InBuf,u16 readLen)
{
	uint32_t u32Addr = DEV_ADDR;
	//?��?����?��y?YD?����512
	for(u16 i = 0; i < readLen; i++)
	{
		InBuf[i] = *(volatile uint8_t *)u32Addr;
		delay1ms(2);
		u32Addr++;
	}
}
/*
	Flash 3?��??��
*/
void FlashInit(void)
{
	u8 tryCnt = 0;
	while(Ok != Flash_Init(1,TRUE))
	{
		tryCnt++;
		delay100us(1);
		
		if(tryCnt > 200)
		{
			break;
		}
	}
}
