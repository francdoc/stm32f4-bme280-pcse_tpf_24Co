#ifndef API_INC_DRIVER_BME280_H_
#define API_INC_DRIVER_BME280_H_

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"        /* <- HAL include */
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_bme280_port.h" /* <- BME280 port include */

/* Exported constants --------------------------------------------------------*/

// GPIO Pin and Port for Chip Select (CS) line
#define CS_Pin GPIO_PIN_3
#define CS_GPIO_Port GPIOB

// Command bits for SPI read/write operations
#define READ_CMD_BIT 0x80  // Command bit for read operation (Most Significant Bit = 1) | Applies mask 0x80 = 0b10000000 -> Most Significant Bit (bit number 7) = 1
#define WRITE_CMD_BIT 0x7F // Command bit for write operation (Most Significant Bit = 0) | Applies mask 0x7F = 0b01111111 -> Most Significant Bit (bit number 7) = 0

// Delay values for BME280 communication
#define BME280_HAL_DELAY 100 // ms

// Timeout for SPI transmit/receive operations
#define SPI_TX_RX_TIMEOUT 1000 // ms

// Register addresses for BME280 sensor data and configuration
#define PRESSURE_MSB_REG 0xF7  // Register address for the most significant byte of pressure data
#define RAW_OUTPUT_DATA_SIZE 8 // Size of the raw data block to be read from the sensor in bytes

#define CHIP_ID_REG 0xD0     // Register address for reading the BME280 chip ID
#define CHIP_ID_BLOCK_SIZE 1 // Size of the chip ID block to be read in bytes

// GPIO Pin states for CS line
#define PinStateLow 0
#define PinStateHigh 1

// Number of LED blinks to indicate successful or error reception of sensor data
#define NumOkRxBlinks 2
#define NumErrorRxBlinks 2

// Size of the command to be written to a register (in bytes)
#define CMD_WRITE_SIZE 1 // bytes

/*
5.4.2 Register 0xE0 “reset”
The “reset” register contains the soft reset word reset[7:0]. If the value 0xB6 is written to the register,
the device is reset using the complete power-on-reset procedure. Writing other values than 0xB6 has
no effect. The readout value is always 0x00.*/
#define BME280_RESET_REG 0xE0 // Register address for performing a soft reset on the sensor

/*
5.4.3 Register 0xF2 “ctrl_hum”
The “ctrl_hum” register sets the humidity data acquisition options of the device. Changes to this
register only become effective after a write operation to “ctrl_meas”.*/
#define BME280_CTRL_HUM_REG 0xF2    // Register address for controlling humidity data acquisition options
#define BME280_CTRL_MEASR_REG 0xF4  // Register address for controlling pressure and temperature data acquisition options
#define BME280_CTRL_CONFIG_REG 0xF5 // Register address for setting the sensor's configuration options

// Memory addresses for reading calibration data from the BME280 sensor
#define BME280_CALIB_00_ADDR 0x88 // Starting address for the first block of calibration data (temperature and pressure)
#define BME280_CALIB_26_ADDR 0xE1 // Starting address for the second block of calibration data (humidity)

// Sizes of calibration data blocks in bytes
#define BME280_CALIBDATA_BLOCK1_SIZE 26 // Size of the first block of calibration data (temperature and pressure)
#define BME280_CALIBDATA_BLOCK2_SIZE 7  // Size of the second block of calibration data (humidity)

// Indices for accessing temperature calibration data bytes in the calibration data buffers
#define DIG_T1_LSB_INDEX 0
#define DIG_T1_MSB_INDEX 1
#define DIG_T2_LSB_INDEX 2
#define DIG_T2_MSB_INDEX 3
#define DIG_T3_LSB_INDEX 4
#define DIG_T3_MSB_INDEX 5

// Indices for accessing humidity calibration data bytes in the calibration data buffers
#define DIG_H1_INDEX 24
#define DIG_H2_LSB_INDEX 0
#define DIG_H2_MSB_INDEX 1
#define DIG_H3_INDEX 2
#define DIG_H4_MSB_INDEX 3
#define DIG_H4_LSB_INDEX 4
#define DIG_H5_MSB_INDEX 4
#define DIG_H5_LSB_INDEX 5
#define DIG_H6_INDEX 6

// Indices for accessing temperature and humidity data bytes in the sensor's output data buffer
#define TEMP_MSB_INDEX 3
#define TEMP_LSB_INDEX 4
#define TEMP_XLSB_INDEX 5 // X in XLSB stands for "extra least significant byte", it refers to the additional bits beyond the least significant byte that help complete the 20-bit resolution.

#define HUM_MSB_INDEX 6
#define HUM_LSB_INDEX 7

// Bit shifts for aligning temperature and humidity data
#define TEMP_MSB_SHIFT 12
#define TEMP_LSB_SHIFT 4
#define TEMP_XLSB_SHIFT 4 // "Extra" bits, so only 4 bits are significant
#define HUM_MSB_SHIFT 8

// Scale factors for converting raw ADC values to human-readable units
#define TEMPERATURE_SCALE_FACTOR 100.0f // Scale factor to convert compensated temperature to degrees Celsius.
#define HUMIDITY_SCALE_FACTOR 1024.0f   // Scale factor to convert compensated humidity to percentage RH.

// BME280 chip ID
#define BME280_CHIP_ID 0x60

/* Exported types ------------------------------------------------------------*/

/* Exported variables -------------------------------------------------------*/

// Here we declare temperature and humidity as extern to make them accessible in other source files (API_fsm.c).
extern float bme280_temperature, bme280_humidity;

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Initializes the BME280 sensor by configuring its control registers and reading calibration parameters.
 * @param  None
 * @retval None
 */
void API_BME280_Init(void);

/**
 * @brief  Reads raw temperature and humidity data from the BME280 sensor and applies compensation formulas.
 * @param  None
 * @retval uint8_t: Returns 0 if the read operation is successful, 1 if an error occurs.
 */
extern uint8_t API_BME280_ReadAndProcess(void);

/**
 * @brief  Error handler for BME280 operations, enters an infinite loop in case of an error.
 * @param  None
 * @retval None
 */
void API_BME280_ErrorHandler(void);

#endif /* API_INC_DRIVER_BME280_H_ */
