
//========================================================================================
// Filename     : keypad_svnseg_interface.c
// Description  : Embedded C program to control 7-segment display with keypad and STM32 board
// Author       : Kyungjae Lee
// History      : 02/07/2023 - Created file
//========================================================================================

// 7-Segment Interfacing: PE4-11
//
// 7-segment pins
// 1    2    4    5    6    7    9    10    (3, 8: common cathode)
// PE7  PE8  PE9  PE10 PE11 PE12 PE13 PE14  (all input mode)


// Keypad Interfacing: PD0-3, PD8-11
//
// Keypad Pin
// 8    7    6    5    4    3    2    1
// R1   R2   R3   R4   C1   C2   C3   C4
// -----------------   -----------------
// output              input
// PD0  PD1  PD2  PD3  PD8  PD9  PD10 PD11
//
// Pull-up resistor is necessary to avoid floating state of an input pin.
// Key not pressed = inputs are High
// Key pressed = inputs are Low
//
// When R1 is Low and all other rows are High, and when C2 is Low,
// we can conclude that the button 2 has been pressed (Intersection of C2 and R1)

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
#define SEG_A (1 << 0) // PE12
#define SEG_B (1 << 1) // PE11
#define SEG_C (1 << 2) // PE9
#define SEG_D (1 << 3) // PE8
#define SEG_E (1 << 4) // PE7
#define SEG_F (1 << 5) // PE13
#define SEG_G (1 << 6) // PE14


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
    // Initialize registers
    init();

    // Keypad & 7-segment display interfacing
    while (1)
    {
        keypad_svnseg_interfacing();
    }
} // end main

// Initializes registers
void init()
{
    // Activate the GPIO peripheral (enable the clock).
    *pRCC_AHB1ENR |= (1 << 0); // for GPIOD, set bit[3]
    *pRCC_AHB1ENR |= (1 << 2); // for GPIOE, set bit[4]

    // Configure the GPIOE pins:
    // Set PE7 - PE14 to OUTPUT mode.
    *pPORTC_MODER &= ~(0xFFFF << 10); // clear
    *pPORTC_MODER |= (0x5555 << 0); // set

    // Configure the GPIOD pins:
    // Set PD0 - PD3 to OUTPUT mode (ROWs).
    *pPORTA_MODER &= ~(0xFF << 0); // clear
    *pPORTA_MODER |= (0x55 << 0); // set
    // Set PD8 - PD11 to INPUT mode (COLs).
    *pPORTA_PUPDR &= ~(0xFF << 8); // clear

    // Enable internal pull-up registers for input pins.
    // Pull-up resistor is necessary to avoid floating state of an input pin.
    *pPORTA_PUPDR &= ~(0xFF << 8); // clear
    *pPORTA_PUPDR |= (0x55 << 8); // set

    *pPORTC_ODR &= ~(0xFF << 0);
} // end init

// Reads keypad input and passes the input value to turn_on_svnseg()
void keypad_svnseg_interfacing()
{
    // Make all row GPIODs HIGH state.
    *pPORTA_ODR |= 0xF;
    // Make R1 (PD0) LOW state.
    *pPORTA_ODR &= ~(1 << 0);

    if (!(*pPORTA_IDR & (1 << 4)))
    {
        delay();
        turn_on_svnseg('1');
    }

    if (!(*pPORTA_IDR & (1 << 5)))
    {
        delay();
        turn_on_svnseg('2');
    }

    if (!(*pPORTA_IDR & (1 << 6)))
    {
        delay();
        turn_on_svnseg('3');
    }

    if (!(*pPORTA_IDR & (1 << 7)))
    {
        delay();
        turn_on_svnseg('A');
    }

    // Make all row GPIOs HIGH state.
    *pPORTA_ODR |= 0xF; // 1111
    // Make R2 (PD1) LOW state.
    *pPORTA_ODR &= ~(1 << 1);   // 1101

    if (!(*pPORTA_IDR & (1 << 4)))
    {
        delay();
        turn_on_svnseg('4');
    }

    if (!(*pPORTA_IDR & (1 << 5)))
    {
        delay();
        turn_on_svnseg('5');
    }

    if (!(*pPORTA_IDR & (1 << 6)))
    {
        delay();
        turn_on_svnseg('6');
    }

    if (!(*pPORTA_IDR & (1 << 7)))
    {
        delay();
        turn_on_svnseg('B');
    }

    // Make all row GPIOs HIGH state.
    *pPORTA_ODR |= 0xF; // 1111
    // Make R3 (PD2) LOW state.
    *pPORTA_ODR &= ~(1 << 2);   // 1011

    if (!(*pPORTA_IDR & (1 << 4)))
    {
        delay();
        turn_on_svnseg('7');
    }

    if (!(*pPORTA_IDR & (1 << 5)))
    {
        delay();
        turn_on_svnseg('8');
    }

    if (!(*pPORTA_IDR & (1 << 6)))
    {
        delay();
        turn_on_svnseg('9');
    }

    if (!(*pPORTA_IDR & (1 << 7)))
    {
        delay();
        turn_on_svnseg('C');
    }

    // Make all row GPIOs HIGH state.
    *pPORTA_ODR |= 0xF; // 1111
    // Make R4 (PD3) LOW state.
    *pPORTA_ODR &= ~(1 << 3);   // 0111

    if (!(*pPORTA_IDR & (1 << 4)))
    {
        delay();
        turn_on_svnseg('*');    // * will be regarded as 'E'
    }

    if (!(*pPORTA_IDR & (1 << 5)))
    {
        delay();
        turn_on_svnseg('0');
    }

    if (!(*pPORTA_IDR & (1 << 6)))
    {
        delay();
        turn_on_svnseg('#'); // # will be regarded as 'F'
    }

    if (!(*pPORTA_IDR & (1 << 7)))
    {
        delay();
        turn_on_svnseg('D');
    }
} // end keypad_svnseg_interfacing

// Turns on 7-segment display corresponding to the passed hex value
void turn_on_svnseg(uint8_t hexval)
{
    *pPORTC_ODR &= ~(0xFF << 7);

    switch (hexval)
    {
    case '0':
        // ABCDEF
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        break;
    case '1':
        // BC
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        break;
    case '2':
        // ABDEG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_G;
        break;
    case '3':
        // ABCDG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_G;
        break;
    case '4':
        // BCGF
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_G;
        *pPORTC_ODR |= SEG_F;
        break;
    case '5':
        // ACDFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case '6':
        // ACDEFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case '7':
        // ABC
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        break;
    case '8':
        // ABCDEFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case '9':
        // ABCDFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case 'A':
        // ABCEFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case 'B':
        // CDEFG
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        *pPORTC_ODR |= SEG_G;
        break;
    case 'C':
        // ADEF
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_F;
        break;
    case 'D':
        // BCDEG
        *pPORTC_ODR |= SEG_B;
        *pPORTC_ODR |= SEG_C;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_G;
        break;
    case '*':
        // ADEGF
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_D;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_G;
        *pPORTC_ODR |= SEG_F;
        break;
    case '#':
        // AEFG
        *pPORTC_ODR |= SEG_A;
        *pPORTC_ODR |= SEG_E;
        *pPORTC_ODR |= SEG_G;
        *pPORTC_ODR |= SEG_F;
        break;
    default :
        break;
    }
} // end turn_on_svnseg

// Gives delay for keypad input debouncing
void delay(void)
{
    for(uint32_t i = 0; i < 300000; i++);
} // end delay
