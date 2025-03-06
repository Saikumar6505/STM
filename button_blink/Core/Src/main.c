#include "stm32f4xx.h"

int main(void)
{
	RCC->AHB1ENR |= 4; /* enable GPIOC clock */
	RCC->AHB1ENR |= 1; /*enable GPIOA clock */
	GPIOA->MODER &= ~0x00000C00; /* clear pin mode */
	GPIOA->MODER |= 0x00000400; /* set pin to output mode */
	GPIOC->MODER &= ~0x0C000000; /* clear pin mode to input mode */

	while(1)
	{
		if (GPIOC->IDR & 0x2000) /* if PC13 is high */
		GPIOA->BSRR = 0x00200000; /* turn off green LED */
		else
		GPIOA->BSRR = 0x00000020; /* turn on green LED */
	}
}
