
#ifndef _SPI_H_
#define _SPI_H_


#include "def.h"

void Spi1Init(void);

void Spi1SetSpeed(u8 arg);

u8 Spi1ReadWriteByte(u8 data);

void SpiAsNet(void);

void SpiAsFlash(void);

void SpiClose(void);

#endif
