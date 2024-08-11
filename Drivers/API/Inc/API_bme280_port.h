#ifndef API_INC_API_BME280_PORT_H_
#define API_INC_API_BME280_PORT_H_

#include "stm32f4xx_hal.h"
#include "API_bme280.h"

void BME280_HAL_SPI_Write(uint8_t reg, uint8_t *data, uint16_t size);
void BME280_HAL_SPI_Read(uint8_t reg, uint8_t *data, uint16_t size);

#endif /* API_INC_API_BME280_PORT_H_ */
