#ifndef API_INC_API_BME280_PORT_H_
#define API_INC_API_BME280_PORT_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"  /* <- HAL include */
#include "API_bme280.h"     /* <- BME280 driver include */

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Write data to the BME280 sensor via SPI.
  * @param  uint8_t reg: The register address in the BME280 sensor to write to.
  * @param  uint8_t *data: Pointer to the data buffer that holds the data to be written.
  * @param  uint16_t size: The size of the data buffer in bytes.
  * @retval None
  */
void BME280_HAL_SPI_Write(uint8_t reg, uint8_t *data, uint16_t size);

/**
  * @brief  Read data from the BME280 sensor via SPI.
  * @param  uint8_t reg: The register address in the BME280 sensor to read from.
  * @param  uint8_t *data: Pointer to the data buffer where the read data will be stored.
  * @param  uint16_t size: The size of the data buffer in bytes.
  * @retval None
  */
void BME280_HAL_SPI_Read(uint8_t reg, uint8_t *data, uint16_t size);

#endif /* API_INC_API_BME280_PORT_H_ */
