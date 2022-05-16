/******************** (C) COPYRIGHT  风驰电子嵌入式开发工作室 ***************************/
#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H






#define A_SYSINFO 		FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS



void ReadEeprom(u16 addr,u8 *buf,u8 len);
void WriteEeprom(u16 addr,u8 *buf,u8 len);
void ReadSysInfo(void);
void SaveSysInfo(void);
void ReadTel(u8 ptr);
void SaveTel(u8 ptr);
void Refactory(void);

#endif

/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/