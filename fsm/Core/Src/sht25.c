/*
 * sht25.c
 *
 *  Created on: Oct 11, 2024
 *      Author: dinesh
 */

#include "sht25.h"

extern I2C_HandleTypeDef hi2c1;

float SHT25_ReadTemperature(void) {
    uint8_t command = 0xE3;  // Command to read temperature
    uint8_t data[2];

    HAL_I2C_Master_Transmit(&hi2c1, SHT25_I2C_ADDRESS << 1, &command, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, SHT25_I2C_ADDRESS << 1, data, 2, HAL_MAX_DELAY);

    uint16_t tempRaw = (data[0] << 8) | data[1];
    tempRaw &= ~0x0003;  // Clear the status bits
    return -46.85 + (175.72 * (float)tempRaw / 65536.0);
}

float SHT25_ReadHumidity(void) {
    uint8_t command = 0xE5;  // Command to read humidity
    uint8_t data[2];

    HAL_I2C_Master_Transmit(&hi2c1, SHT25_I2C_ADDRESS << 1, &command, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, SHT25_I2C_ADDRESS << 1, data, 2, HAL_MAX_DELAY);

    uint16_t humRaw = (data[0] << 8) | data[1];
    humRaw &= ~0x0003;  // Clear the status bits
    return -6.0 + (125.0 * (float)humRaw / 65536.0);
}

