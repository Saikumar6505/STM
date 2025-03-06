#include <stdint.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define ADDR_RCC_BASE 0x40023800
#define ADDR_RCC_AHB1ENR (ADDR_RCC_BASE + 0x30)

// Port A
#define ADDR_PORTA_BASE 0x40020000
#define ADDR_PORTA_MODER (ADDR_PORTA_BASE + 0x00)
#define ADDR_PORTA_PUPDR (ADDR_PORTA_BASE + 0x0C)
#define ADDR_PORTA_IDR (ADDR_PORTA_BASE + 0x10)
#define ADDR_PORTA_ODR (ADDR_PORTA_BASE + 0x14)

// Port C
#define ADDR_PORTC_BASE 0x40020800
#define ADDR_PORTC_MODER (ADDR_PORTC_BASE + 0x00)
#define ADDR_PORTC_ODR (ADDR_PORTC_BASE + 0x14)

// Segment to GPIO pin mapping
#define SEG_A (1 << 0)  // PC0
#define SEG_B (1 << 1)  // PC1
#define SEG_C (1 << 2)  // PC2
#define SEG_D (1 << 3)  // PC3
#define SEG_E (1 << 4)  // PC4
#define SEG_F (1 << 5)  // PC5
#define SEG_G (1 << 6)  // PC6

void init();
void turn_on_svnseg(uint8_t hexval);
void keypad_svnseg_interfacing();
void delay(void);

volatile uint32_t *const pRCC_AHB1ENR = (uint32_t *)ADDR_RCC_AHB1ENR;
volatile uint32_t *const pPORTA_MODER = (uint32_t *)ADDR_PORTA_MODER;
volatile uint32_t *const pPORTA_ODR = (uint32_t *)ADDR_PORTA_ODR;
volatile uint32_t *const pPORTA_IDR = (uint32_t *)ADDR_PORTA_IDR;
volatile uint32_t *const pPORTA_PUPDR = (uint32_t *)ADDR_PORTA_PUPDR;
volatile uint32_t *const pPORTC_MODER = (uint32_t *)ADDR_PORTC_MODER;
volatile uint32_t *const pPORTC_ODR = (uint32_t *)ADDR_PORTC_ODR;

int main(void)
{
    init();
    while (1)
    {
        keypad_svnseg_interfacing();
    }
}

// Initializes registers
void init()
{
    *pRCC_AHB1ENR |= (1 << 0); // Enable GPIOA
    *pRCC_AHB1ENR |= (1 << 2); // Enable GPIOC

    *pPORTC_MODER &= ~(0xFFFFF); // Clear PC0-PC6
    *pPORTC_MODER |= (0x55555); // Set PC0-PC6 to OUTPUT

    *pPORTA_MODER &= ~(0xFF); // Clear PA0-PA3
    *pPORTA_MODER |= (0x55); // Set PA0-PA3 to OUTPUT
    *pPORTA_MODER &= ~(0xFF << 8); // Clear PA4-PA7
    *pPORTA_PUPDR &= ~(0xFF << 8); // Clear pull-up/pull-down for PA4-PA7
    *pPORTA_PUPDR |= (0x55 << 8); // Enable pull-up for PA4-PA7

    *pPORTC_ODR &= ~(0x7F); // Turn off all segments initially
}

// Reads keypad input and passes the input value to turn_on_svnseg()
void keypad_svnseg_interfacing()
{
    static const uint8_t keypad[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    for (uint8_t row = 0; row < 4; row++)
    {
        *pPORTA_ODR |= 0xF; // Set all rows high
        *pPORTA_ODR &= ~(1 << row); // Set current row low

        for (uint8_t col = 0; col < 4; col++)
        {
            if (!(*pPORTA_IDR & (1 << (4 + col))))
            {
                delay(); // Debounce delay
                turn_on_svnseg(keypad[row][col]);
                while (!(*pPORTA_IDR & (1 << (4 + col)))); // Wait for key release
                return;
            }
        }
    }
}

// Turns on 7-segment display corresponding to the passed hex value
void turn_on_svnseg(uint8_t hexval)
{
    *pPORTC_ODR &= ~(0x7F); // Clear all segments

    static const uint8_t segData[16] = {
        SEG_G,         // '0'
        SEG_A | SEG_D |SEG_E |SEG_F |SEG_G,  // '1'
        SEG_C | SEG_F ,                // '2'
        SEG_E | SEG_F ,                                               // '3'
        SEG_A | SEG_D | SEG_E ,                         // '4'
        SEG_B | SEG_E ,                                                // '5'
        SEG_B ,                                                        // '6'
        SEG_D | SEG_E | SEG_F |SEG_G,                                 // '7'
        SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,        // '8'
        SEG_E ,        // '9'
        SEG_D ,       // 'A'
        SEG_A | SEG_B ,                // 'B'
        SEG_B | SEG_C | SEG_G ,                         // 'C'
        SEG_A | SEG_F ,                // 'D'
        SEG_B | SEG_C ,                         // 'E'
        SEG_B | SEG_C | SEG_D                          // 'F'
    };

    if (hexval >= '0' && hexval <= '9')
        *pPORTC_ODR |= segData[hexval - '0'];
    else if (hexval >= 'A' && hexval <= 'F')
        *pPORTC_ODR |= segData[hexval - 'A' + 10];
    else if (hexval == '*')
        *pPORTC_ODR |= segData[14]; // Assume '*'
    else if (hexval == '#')
        *pPORTC_ODR |= segData[15]; // Assume '#'
}

// Simple delay function
void delay(void)
{
    for (volatile uint32_t i = 0; i < 1000000; i++);
}
