#ifndef __USART_H
#define __USART_H

#include "stm32f4xx_hal.h"  // or your specific STM32 HAL

void USART_Init(void);
void USART_SendString(char *str);
void USART_ReceiveData(uint8_t *data, uint16_t size);

#endif /* __USART_H */
