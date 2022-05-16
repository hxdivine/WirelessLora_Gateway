#ifndef _HC_FLASH_H_
#define _HC_FLASH_H_

#include "def.h"
#include "flash.h"

void FlashWriteData(uint8_t *InBuf,u16 writeLen);

void FlashReadData(uint8_t *InBuf,u16 readLen);

void FlashInit(void);

#endif
