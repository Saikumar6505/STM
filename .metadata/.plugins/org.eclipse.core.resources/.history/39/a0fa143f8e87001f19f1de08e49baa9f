/*
 * lcd_i2c.c
 *
 *  Created on: Oct 9, 2024
 *      Author: dinesh
 */

#include "lcd_i2c.h"

// Adjust these macros to match your LCD address and configuration
#define SLAVE_ADDRESS_LCD 0x4E  // Change according to your LCD module's I2C address

void lcd_init(I2C_HandleTypeDef *hi2c) {
    // Wait for LCD to power up
    HAL_Delay(50);

    // Initialization sequence
    lcd_send_cmd(hi2c, 0x30);  // Wake up
    HAL_Delay(5);
    lcd_send_cmd(hi2c, 0x30);  // Wake up
    HAL_Delay(1);
    lcd_send_cmd(hi2c, 0x30);  // Wake up
    HAL_Delay(10);

    // 4-bit mode
    lcd_send_cmd(hi2c, 0x20);  // Function set: 4-bit mode

    // LCD configuration
    lcd_send_cmd(hi2c, 0x28);  // 2-line, 5x8 dots
    lcd_send_cmd(hi2c, 0x08);  // Display off, cursor off, blink off
    lcd_send_cmd(hi2c, 0x01);  // Clear display
    HAL_Delay(2);
    lcd_send_cmd(hi2c, 0x06);  // Entry mode set: shift right, cursor right
    lcd_send_cmd(hi2c, 0x0C);  // Display on, cursor off, blink off
}

void lcd_clear(I2C_HandleTypeDef *hi2c) {
    lcd_send_cmd(hi2c, LCD_CLEARDISPLAY);
    HAL_Delay(2);  // LCD clear command requires a delay
}

void lcd_put_cur(I2C_HandleTypeDef *hi2c, uint8_t row, uint8_t col) {
    uint8_t pos = (row == 0) ? (0x80 | col) : (0xC0 | col);
    lcd_send_cmd(hi2c, pos);
}

void lcd_send_string(I2C_HandleTypeDef *hi2c, char *str) {
    while (*str) {
        lcd_send_data(hi2c, *str++);
    }
}

void lcd_send_cmd(I2C_HandleTypeDef *hi2c, char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];

    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);

    // Send upper nibble
    data_t[0] = data_u | 0x0C;  // en=1, rs=0
    data_t[1] = data_u | 0x08;  // en=0, rs=0
    // Send lower nibble
    data_t[2] = data_l | 0x0C;  // en=1, rs=0
    data_t[3] = data_l | 0x08;  // en=0, rs=0

    HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

void lcd_send_data(I2C_HandleTypeDef *hi2c, char data) {
    char data_u, data_l;
    uint8_t data_t[4];

    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);

    // Send upper nibble
    data_t[0] = data_u | 0x0D;  // en=1, rs=1
    data_t[1] = data_u | 0x09;  // en=0, rs=1
    // Send lower nibble
    data_t[2] = data_l | 0x0D;  // en=1, rs=1
    data_t[3] = data_l | 0x09;  // en=0, rs=1

    HAL_I2C_Master_Transmit(hi2c, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

