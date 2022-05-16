
#ifndef _HC_SPI_H_
#define _HC_SPI_H_

#include "def.h"

void SpiInit(void);

u8 Spi1ReadWriteByte(u8 data);

void SPI_Stop(void);

void SPI_Start(void);

#endif
