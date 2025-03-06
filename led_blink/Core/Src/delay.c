/*
 * delay.c
 *
 *  Created on: Aug 19, 2024
 *      Author: dinesh
 */

#include "delay.h"

void delay_ms(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3195; j++)
            asm("nop");
}
