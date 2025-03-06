#include "stm32f4xx.h"  // Adjust this include based on your STM32 series

void GPIO_Init(void) {
    // Enable the clock for GPIOA and GPIOC
    RCC->AHB1ENR |= (1 << 0); // Enable GPIOA clock
    RCC->AHB1ENR |= (1 << 2); // Enable GPIOC clock

    // Configure GPIOC Pin 9 as input
    GPIOC->MODER &= ~(3 << (9 * 2)); // Clear mode bits for PC9
    GPIOC->PUPDR |= (1 << (9 * 2));  // Set PC9 to pull-up

    // Configure GPIOA Pin 8 as output
    GPIOA->MODER |= (1 << (8 * 2));   // Set PA8 to output mode
    GPIOA->MODER &= ~(1 << (8 * 2 + 1)); // Clear the second bit to ensure it's output
}

int main(void) {
    GPIO_Init(); // Initialize GPIOs

    while (1) {
        // Read the state of GPIOC Pin 9
        if ((GPIOC->IDR & (1 << 9)) == 0) { // Check if PC9 is low (pressed)
            GPIOA->ODR |= (1 << 8); // Set PA8 high
        } else {
            GPIOA->ODR &= ~(1 << 8); // Set PA8 low
        }
    }
}
