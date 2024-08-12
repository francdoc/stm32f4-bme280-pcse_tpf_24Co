/* Includes ------------------------------------------------------------------*/
#include "API_bme280_port.h"

/* Private variable ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Write data to the BME280 sensor via SPI.
 * @param  uint8_t reg: The register address in the BME280 sensor to write to.
 * @param  uint8_t *data: Pointer to the data buffer that holds the data to be written.
 * @param  uint16_t size: The size of the data buffer in bytes.
 * @retval None.
 */
void BME280_HAL_SPI_Write(uint8_t reg, uint8_t *data, uint16_t size)
{
  uint8_t regAddress = reg & WRITE_CMD_BIT; // Apply the write command mask.
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
  HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

/**
 * @brief  Read data from the BME280 sensor via SPI.
 * @param  uint8_t reg: The register address in the BME280 sensor to read from.
 * @param  uint8_t *data: Pointer to the data buffer where the read data will be stored.
 * @param  uint16_t size: The size of the data buffer in bytes.
 * @retval None.
 */
void BME280_HAL_SPI_Read(uint8_t reg, uint8_t *data, uint16_t size)
{
  uint8_t regAddress = reg | READ_CMD_BIT; // Apply the read command mask.
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
  HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), SPI_TX_RX_TIMEOUT);
  HAL_SPI_Receive(&hspi1, data, size, SPI_TX_RX_TIMEOUT);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

/**
 *  @brief  Provides a delay for a specified number of milliseconds.
 * @param  delay: The amount of time, in milliseconds, to delay.
 * @retval None
 */
void BME280_HAL_Delay(uint32_t delay)
{
  HAL_Delay(delay);
}

/**
 * @brief  Toggles the state of the specified LED.
 * @param  Led: Specifies the LED to be toggled. This parameter can be one of the LED identifiers defined in the board support package (BSP), such as `LED2`, `LED3`, etc.
 * @retval None
 */
void BME280_HAL_Blink(Led_TypeDef Led)
{
  BSP_LED_Toggle(Led);
}
