#include "API_bme280_port.h"

void BME280_HAL_SPI_Write(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg & WRITE_CMD_BIT; // Apply the write command mask.
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

void BME280_HAL_SPI_Read(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg | READ_CMD_BIT; // Apply the read command mask.
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), SPI_TX_RX_TIMEOUT);
    HAL_SPI_Receive(&hspi1, data, size, SPI_TX_RX_TIMEOUT);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}
