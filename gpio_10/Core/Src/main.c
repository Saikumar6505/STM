#include "stm32f4xx.h"
#include<stdio.h>
/* Variables */
uint8_t ledState = 0;           // LED state (0: OFF, 1: ON)
uint32_t switchPressCount = 0;  // Switch press counter

/* Function to enable ITM for SWV Console */


/* Simple delay function for debouncing */
void delay(volatile uint32_t count) {
    while (count--);
}

int main(void) {
    /* Enable GPIOA and GPIOC clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock (for LED)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  // Enable GPIOC clock (for Button)

    /* Configure PA5 as output (for LED) */
    GPIOA->MODER |= (1 << 10);   // Set PA5 to output mode
    GPIOA->MODER &= ~(1 << 11);  // Clear bit 11
    GPIOA->OTYPER &= ~(1 << 5);  // Push-pull output type
    GPIOA->PUPDR &= ~((1 << 10) | (1 << 11));  // No pull-up/pull-down on PA5

    /* Configure PC13 as input (for Button) */
    GPIOC->MODER &= ~((1 << 26) | (1 << 27));  // Set PC13 to input mode
    GPIOC->PUPDR |= (1 << 27);                 // Enable pull-up for PC13
    GPIOC->PUPDR &= ~(1 << 26);                // Clear pull-down

    /* Print program start message */
    printf("Program Started!\n");

    /* Main loop */
    while (1) {
        // Check if button (PC13) is pressed (logic low)
        if (!(GPIOC->IDR & (1 << 13))) {
            delay(100000);  // Simple debouncing
            if (!(GPIOC->IDR & (1 << 13))) {  // Confirm button is still pressed
                // Toggle LED state
                ledState = !ledState;

                if (ledState) {
                    GPIOA->ODR |= (1 << 5);  // Turn LED ON
                    printf("LED ON\n");
                } else {
                    GPIOA->ODR &= ~(1 << 5); // Turn LED OFF
                    printf("LED OFF\n");
                }

                // Increment switch press counter
                switchPressCount++;
                printf("Switch Press Count: %lu\n", switchPressCount);

                // Wait for button release
                while (!(GPIOC->IDR & (1 << 13)));  // Wait until button is released
                delay(100000);  // Debouncing after release
            }
        }
    }
}
