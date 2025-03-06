#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_hal.h"  // or your specific STM32 HAL

void GPIO_Init(void);
void GPIO_SetPin(uint16_t pin);
void GPIO_ClearPin(uint16_t pin);

#endif /* __GPIO_H */
