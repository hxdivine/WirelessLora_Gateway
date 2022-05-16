
#ifndef _SYS_FLASH_H_
#define _SYS_FLASH_H_

#include "def.h"

void Fmc_ErasePages(uint32_t address,u16_t PageNum);

void Fmc_WriteToFlash(uint32_t address,u8 *InBuf, u16 len);

void Fmc_ReadToFlash(uint32_t address,uint32_t *InBuf, u16 len);

#endif
