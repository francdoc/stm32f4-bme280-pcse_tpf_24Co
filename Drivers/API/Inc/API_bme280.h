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

void BME280_init(void);
void BME280_calculate(void);
float BME280_getTemp(void);
float BME280_getHum(void);
void BME280_Error_Handler(void);

void APP(void);

#endif /* API_INC_DRIVER_BME280_H_ */
