/*
 * sht25.h
 *
 *  Created on: Oct 11, 2024
 *      Author: dinesh
 */

#ifndef INC_SHT25_H_
#define INC_SHT25_H_

#include "main.h"

#define SHT25_I2C_ADDRESS 0x40

float SHT25_ReadTemperature(void);
float SHT25_ReadHumidity(void);

#endif /* INC_SHT25_H_ */
