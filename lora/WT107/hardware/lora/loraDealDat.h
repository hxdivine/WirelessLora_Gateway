#ifndef _LORA_DEAL_DAT_H_
#define _LORA_DEAL_DAT_H_

#include "def.h"

extern u8 needRestart;

u8 LoraSendDat(u8 mode);

void LoraRecDat(void);

#endif
