
#ifndef _SGP30_H_
#define _SGP30_H_

#include "def.h"

void SGP30_Init(void);

void SGP30_GetCo2Data(u16 *co2,u16 *tvoc);

#endif
