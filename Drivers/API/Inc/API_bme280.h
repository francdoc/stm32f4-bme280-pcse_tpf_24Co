#ifndef API_INC_DRIVER_BME280_H_
#define API_INC_DRIVER_BME280_H_

#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_delay.h"
#include "API_uart.h"

extern SPI_HandleTypeDef hspi1;

#define TIMEOUT 1000 // ms

#define CS_Pin GPIO_PIN_3
#define CS_GPIO_Port GPIOB

#define READ_CMD_BIT 0x80  // Command bit for read operation (Most Significant Bit = 1) | 6.3.1 SPI read | applies mask 0x80 = 0b10000000 -> Most Significant Bit (bit number 7) = 1
#define WRITE_CMD_BIT 0x7F // Command bit for write operation (Most Significant Bit = 0) | 6.3.2 SPI write | applies mask 0x7F = 0b01111111 -> Most Significant Bit (bit number 7) = 0

void BME280_init(void);
void BME280_calculate(void);
float BME280_getTemp(void);
float BME280_getHum(void);
void BME280_Error_Handler(void);

void APP(void);

#endif /* API_INC_DRIVER_BME280_H_ */
