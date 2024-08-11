#ifndef API_INC_DRIVER_BME280_H_
#define API_INC_DRIVER_BME280_H_

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_bme280_port.h"

#define CS_Pin GPIO_PIN_3
#define CS_GPIO_Port GPIOB

#define READ_CMD_BIT 0x80  // Command bit for read operation (Most Significant Bit = 1) | 6.3.1 SPI read | applies mask 0x80 = 0b10000000 -> Most Significant Bit (bit number 7) = 1
#define WRITE_CMD_BIT 0x7F // Command bit for write operation (Most Significant Bit = 0) | 6.3.2 SPI write | applies mask 0x7F = 0b01111111 -> Most Significant Bit (bit number 7) = 0

#define BME280_HAL_DELAY 100 // ms

#define SPI_TX_RX_TIMEOUT 1000 // ms

#define PRESSURE_MSB_REG 0XF7
#define RAW_OUTPUT_DATA_SIZE 8 // bytes

#define CHIP_ID_REG 0xD0
#define CHIP_ID_BLOCK_SIZE 1 // bytes

#define PinStateLow 0
#define PinStateHigh 1

#define NumOkRxBlinks 2
#define NumErrorRxBlinks 2

#define CMD_WRITE_SIZE 1 // bytes

/*
5.4.2 Register 0xE0 “reset”
The “reset” register contains the soft reset word reset[7:0]. If the value 0xB6 is written to the register,
the device is reset using the complete power-on-reset procedure. Writing other values than 0xB6 has
no effect. The readout value is always 0x00.*/
#define BME280_RESET_REG 0xE0

/*
5.4.3 Register 0xF2 “ctrl_hum”
The “ctrl_hum” register sets the humidity data acquisition options of the device. Changes to this
register only become effective after a write operation to “ctrl_meas”.*/
#define BME280_CTRL_HUM_REG 0xF2

/*
5.4.5 Register 0xF4 “ctrl_meas”
The “ctrl_meas” register sets the pressure and temperature data acquisition options of the device. The
register needs to be written after changing “ctrl_hum” for the changes to become effective.
*/
#define BME280_CTRL_MEASR_REG 0xF4

/*
5.4.6 Register 0xF5 “config”
The “config” register sets the rate, filter and interface options of the device. Writes to the “config”
register in normal mode may be ignored. In sleep mode writes are not ignored.
*/
#define BME280_CTRL_CONFIG_REG 0xF5

// Define the memory addresses for calibration data in the BME280 sensor
#define BME280_CALIB_00_ADDR 0x88 // Starting address for the first block of calibration data (temperature and pressure)
#define BME280_CALIB_26_ADDR 0xE1 // Starting address for the second block of calibration data (humidity)

// Define the sizes of the calibration data blocks
#define BME280_CALIBDATA_BLOCK1_SIZE 26 // Size in bytes of the first block of calibration data (temperature and pressure)
#define BME280_CALIBDATA_BLOCK2_SIZE 7  // Size in bytes of the second block of calibration data (humidity)

// Define indices for temperature calibration data bytes
#define DIG_T1_LSB_INDEX  0
#define DIG_T1_MSB_INDEX  1
#define DIG_T2_LSB_INDEX  2
#define DIG_T2_MSB_INDEX  3
#define DIG_T3_LSB_INDEX  4
#define DIG_T3_MSB_INDEX  5

// Define indices for humidity calibration data bytes
#define DIG_H1_INDEX      24
#define DIG_H2_LSB_INDEX  0
#define DIG_H2_MSB_INDEX  1
#define DIG_H3_INDEX      2
#define DIG_H4_MSB_INDEX  3
#define DIG_H4_LSB_INDEX  4
#define DIG_H5_MSB_INDEX  4
#define DIG_H5_LSB_INDEX  5
#define DIG_H6_INDEX      6

#define TEMP_MSB_INDEX 3
#define TEMP_LSB_INDEX 4
#define TEMP_XLSB_INDEX 5 // X in XLSB stands for "extra least significant byte", it refers to the additional bits beyond the least significant byte that help complete the 20-bit resolution.

#define HUM_MSB_INDEX 6
#define HUM_LSB_INDEX 7

#define TEMP_MSB_SHIFT 12
#define TEMP_LSB_SHIFT 4
#define TEMP_XLSB_SHIFT 4 // "Extra" bits, so only 4 bits are significant

#define HUM_MSB_SHIFT 8

// 4.2.3 Compensation formulas (pg. 25/60)
#define TEMPERATURE_SCALE_FACTOR 100.0f // Scale factor to convert compensated temperature to degrees Celsius.
#define HUMIDITY_SCALE_FACTOR 1024.0f // Scale factor to convert compensated humidity to percentage RH.

typedef int32_t BME280_S32_t;  // global type
typedef uint32_t BME280_U32_t; // global type

// Declare temp and hum as extern to make them accessible in API_fsm.c file.
extern float temp, hum;

void BME280_init(void);
void BME280_Error_Handler(void);
extern uint8_t BME280_read(void);

#endif /* API_INC_DRIVER_BME280_H_ */
