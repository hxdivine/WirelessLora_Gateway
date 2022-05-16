#ifndef _SX127X_HAL_H_
#define _SX127X_HAL_H_

#include "def.h"

#define LORARST_H      Gpio_WriteOutputIO(GpioPortB, GpioPin1,TRUE);
#define LORARST_L      Gpio_WriteOutputIO(GpioPortB, GpioPin1,FALSE);

#define LORACS_H       SPI_Stop();
#define LORACS_L       SPI_Start();

void SX127X_WriteBuffer(uint8_t addr,uint8_t *buffer,uint8_t size);

void SX127X_ReadBuffer(uint8_t addr,uint8_t *buffer,uint8_t size);

#endif
