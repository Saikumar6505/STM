#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx_hal.h"  // or your specific STM32 HAL

void I2C_Init(void);
void I2C_Write(uint8_t addr, uint8_t *data, uint16_t size);
void I2C_Read(uint8_t addr, uint8_t *data, uint16_t size);

#endif /* __I2C_H */
