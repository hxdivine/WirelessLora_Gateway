
#ifndef _LORA_H_
#define _LORA_H_

#include "def.h"
#include "config.h"

void InitLora(void);

void SendLoraDatPack(u8 *buf,u8 len);

u8 LoraDatPro(void);

#endif
