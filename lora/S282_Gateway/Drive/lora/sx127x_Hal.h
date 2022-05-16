
#ifndef _SX127X_HAL_H_
#define _SX127X_HAL_H_

#include "def.h"

#define LORARST_H      gpio_bit_set(GPIOB,GPIO_PIN_10);
#define LORARST_L      gpio_bit_reset(GPIOB,GPIO_PIN_10);

#define LORACS_H       gpio_bit_set(GPIOB,GPIO_PIN_11);
#define LORACS_L       gpio_bit_reset(GPIOB,GPIO_PIN_11);

void SX127X_WriteBuffer(uint8_t addr,uint8_t *buffer,uint8_t size);

void SX127X_ReadBuffer(uint8_t addr,uint8_t *buffer,uint8_t size);

#endif
