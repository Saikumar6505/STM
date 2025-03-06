/*
 * gpio.c
 *
 *  Created on: Aug 19, 2024
 *      Author: dinesh
 */


#include "stm32f4xx.h"
#include "gpio.h"

void gpio_init(void)
{
    RCC->AHB1ENR |= 1;          // Enable GPIOA clock
    GPIOA->MODER &= ~0x00000C00; // Clear pin mode
    GPIOA->MODER |= 0x00000400;  // Set pin to output mode
}

void gpio_set(void)
{
    GPIOA->ODR |= 0x00000020;   // Turn on LED
}

void gpio_reset(void)
{
    GPIOA->ODR &= ~0x00000020;  // Turn off LED
}
