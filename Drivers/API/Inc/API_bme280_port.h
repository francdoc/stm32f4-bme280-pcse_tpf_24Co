#ifndef API_INC_API_BME280_PORT_H_
#define API_INC_API_BME280_PORT_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"        /* <- HAL include */
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */
#include "API_bme280.h"           /* <- BME280 driver include */

/* SPI handler declaration */

extern SPI_HandleTypeDef hspi1;

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

/**
 *  @brief  Provides a delay for a specified number of milliseconds.
 * @param  delay: The amount of time, in milliseconds, to delay.
 * @retval None
 */
void BME280_HAL_Delay(uint32_t delay);

/**
 * @brief  Toggles the state of the specified LED.
 * @param  Led: Specifies the LED to be toggled. This parameter can be one of the LED identifiers defined in the board support package (BSP), such as `LED2`, `LED3`, etc.
 * @retval None
 */
void BME280_HAL_Blink(Led_TypeDef Led);

#endif /* API_INC_API_BME280_PORT_H_ */
