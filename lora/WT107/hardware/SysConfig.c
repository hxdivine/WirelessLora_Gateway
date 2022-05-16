
#include "SysConfig.h"
#include "string.h"
#include "HC_Flash.h"
#include "hardware.h"

DEVINFO DevInfo;
/*
	设置参数默认值
*/
void SetDevInfoDef(void)
{
  DevInfo.DevId = 1;
  memset(DevInfo.TarId,0,6);
	memset(DevInfo.Uuid,0,6);
	DevInfo.Uuid[0] = ChipId[0] >> 4;
  DevInfo.Uuid[1] = ChipId[0] & 0x0F;
  DevInfo.Uuid[2] = ChipId[1] >> 4;
  DevInfo.Uuid[3] = ChipId[1] & 0x0F;
  DevInfo.Uuid[4] = ChipId[2] >> 4;
  DevInfo.Uuid[5] = ChipId[2] & 0x0F;
	
  DevInfo.RepCycle = 2;
	DevInfo.workFreq = 1;
  DevInfo.IsWrite = 1;
  DevInfo.HeightLimit[0] = 1000;
	DevInfo.HeightLimit[1] = 1000;
	DevInfo.HeightLimit[2] = 6000;
	DevInfo.HeightLimit[3] = 1000;
	DevInfo.HeightLimit[4] = 1000;
	DevInfo.HeightLimit[5] = 1000;
	DevInfo.HeightLimit[6] = 1000;
	DevInfo.LowLimit[0] = 0;
	DevInfo.LowLimit[1] = 0;
	DevInfo.LowLimit[2] = 0;
	DevInfo.LowLimit[3] = 0;
	DevInfo.LowLimit[4] = 0;
	DevInfo.LowLimit[5] = 0;
	DevInfo.LowLimit[6] = 0;
  
	SaveDevInfo(&DevInfo);
  ReadDevInfo(&DevInfo);
}
/*
	保存设备参数
*/
void SaveDevInfo(DEVINFO *dev)
{
	FlashWriteData((u8*)dev,sizeof(DEVINFO));
	FlashReadData((u8*)dev,sizeof(DEVINFO));
}
/*
	读设备参数
*/
void ReadDevInfo(DEVINFO *dev)
{
	FlashReadData((u8*)dev,sizeof(DEVINFO));
}

