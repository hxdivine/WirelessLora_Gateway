
#include "stm8l15x.h"
#include "pub.h"
#include "flash_eeprom.h"
#include "hw_config.h" 
#include "stdio.h"

WTSYSINFO SysInfo;
 
    

void ReadEeprom(u16 addr,u8 *buf,u8 len)
{
  while(len--)	
    {
        *buf=FLASH_ReadByte(addr) ;
        buf++;
        addr++;
    }
}

void WriteEeprom(u16 addr,u8 *buf,u8 len)
{

	u8 i;   
	FLASH_Unlock(FLASH_MemType_Data);
	for(i=0;i<len;i++){
		FLASH_ProgramByte(addr+i,*buf);
		FLASH_WaitForLastOperation(FLASH_MemType_Data); 
		buf++;
	}
    FLASH_Lock(FLASH_MemType_Data);
}



void Refactory(void)
{
    
    SysInfo.id=250;
    SysInfo.TranMinute=240;//
       
  
    SysInfo.TempHigh=0x1fff;
    SysInfo.HumiHigh=0x1fff;

    
    SysInfo.TempLow=-400;    
    SysInfo.HumiLow=0;    

    SaveSysInfo();
}

void ReadSysInfo(void)
{
	ReadEeprom(A_SYSINFO,(u8*)&SysInfo,sizeof(SysInfo));
}
void SaveSysInfo(void)
{
	WriteEeprom(A_SYSINFO,(u8*)&SysInfo,sizeof(SysInfo));
}

