#include "stm32f4xx.h"

#define RS 0x20  /* PB5 mask for Register Select */
#define RW 0x40  /* PB6 mask for Read/Write */
#define EN 0x80  /* PB7 mask for Enable */

/* Function prototypes */
void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);
void LCD_string(const char* str);

int main(void) {
    /* Initialize LCD controller */
    LCD_init();

    while (1) {
        /* Display "Welcome to STM32" on the first line */
        LCD_command(0x80);  // Set cursor to the beginning of the first line
        LCD_string("Welcome to STM32");

        /* Display "Enjoy Coding!" on the second line */
        LCD_command(0xC0);  // Set cursor to the beginning of the second line
        LCD_string("Enjoy Coding!");

        delayMs(2000);

        /* Clear LCD display */
        LCD_command(0x01);  // Clear display command
        delayMs(500);
    }
}

/* Function to display a string on the LCD */
void LCD_string(const char* str) {
    while (*str) {
        LCD_data(*str++);
    }
}

/* Initialize port pins and LCD controller */
void LCD_init(void) {
    PORTS_init();
    delayMs(20);  // Initialization delay

    /* Function set: 8-bit, 2 lines, 5x7 font */
    LCD_command(0x38);
    /* Entry mode set: Increment cursor */
    LCD_command(0x06);
    /* Clear display */
    LCD_command(0x01);
    delayMs(2);  // Wait for the display to clear
    /* Display on, cursor blinking */
    LCD_command(0x0F);
}

void PORTS_init(void) {
    /* Enable GPIOB and GPIOC clock */
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    /* Configure GPIOB pins for RS, RW, and EN */
    GPIOB->MODER &= ~0x0000FC00;  // Clear pin modes for PB5-PB7
    GPIOB->MODER |= 0x00005400;   // Set PB5, PB6, PB7 to output mode
    GPIOB->BSRR = 0x00C00000;     // Turn off EN and RW pins

    /* Configure GPIOC pins for D0-D7 */
    GPIOC->MODER &= ~0x0000FFFF;  // Clear pin modes for PC0-PC7
    GPIOC->MODER |= 0x00005555;   // Set PC0-PC7 to output mode
}

void LCD_command(unsigned char command) {
    GPIOB->BSRR = (RS | RW) << 16;  // RS = 0, RW = 0
    GPIOC->ODR = command;           // Send command to data bus
    GPIOB->BSRR = EN;               // Pulse E high
    delayMs(1);
    GPIOB->BSRR = EN << 16;         // Pulse E low

    // Commands 1 and 2 need up to 1.64ms, others need 40us
    delayMs((command < 4) ? 2 : 1);
}

void LCD_data(char data) {
    GPIOB->BSRR = RS;               // RS = 1
    GPIOB->BSRR = RW << 16;         // RW = 0
    GPIOC->ODR = data;              // Send data to data bus
    GPIOB->BSRR = EN;               // Pulse E high
    delayMs(1);
    GPIOB->BSRR = EN << 16;         // Pulse E low
    delayMs(1);
}

void delayMs(int n) {
    int i;
    for (; n > 0; n--) {
        for (i = 0; i < 3195; i++) {
            // Simple delay loop
        }
    }
}
