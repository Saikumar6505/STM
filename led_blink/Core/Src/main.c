#include "stm32f4xx.h"
#include "gpio.h"
#include "delay.h"

int main(void)
{
    gpio_init();  // Initialize GPIOA pin 5
    while (1)
    {
        gpio_set();     // Turn on LED
        delay_ms(500);
        gpio_reset();   // Turn off LED
        delay_ms(500);
    }
}
