#ifndef API_INC_DRIVER_BME280_H_
#define API_INC_DRIVER_BME280_H_

#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_delay.h"
#include "API_uart.h"

extern SPI_HandleTypeDef hspi1;

extern RTC_HandleTypeDef hrtc;

#define TIMEOUT 1000 // ms

#define CS_Pin GPIO_PIN_3
#define CS_GPIO_Port GPIOB

#define READ_CMD_BIT 0x80  // Command bit for read operation (Most Significant Bit = 1) | 6.3.1 SPI read | applies mask 0x80 = 0b10000000 -> Most Significant Bit (bit number 7) = 1
#define WRITE_CMD_BIT 0x7F // Command bit for write operation (Most Significant Bit = 0) | 6.3.2 SPI write | applies mask 0x7F = 0b01111111 -> Most Significant Bit (bit number 7) = 0

#define BME_HAL_DELAY 100 // ms
#define MEMADDRESSSIZE 1  // bit

#define CALIBMEMADD1 0x88
#define CALIBMEMADD2 0xE1

#define CALIBDATASIZE1 25
#define CALIBDATASIZE2 7
#define CMDWRITESIZE 1
#define RAWDATASIZE 8
#define RAWDATAREG1 0XF7
#define CHIPIDREG 0xD0

#define PinStateLow 0
#define PinStateHigh 1

/*
5.4.3 Register 0xF2 “ctrl_hum”
The “ctrl_hum” register sets the humidity data acquisition options of the device. Changes to this
register only become effective after a write operation to “ctrl_meas”.*/
#define CTRL_HUM 0xF2

/*
5.4.4 Register 0xF3 “status”
The “status” register contains two bits which indicate the status of the device.
*/
#define STATUS 0xF3

/*
5.4.5 Register 0xF4 “ctrl_meas”
The “ctrl_meas” register sets the pressure and temperature data acquisition options of the device. The
register needs to be written after changing “ctrl_hum” for the changes to become effective.
*/
#define CTRL_MEAS 0xF4

/*
5.4.6 Register 0xF5 “config”
The “config” register sets the rate, filter and interface options of the device. Writes to the “config”
register in normal mode may be ignored. In sleep mode writes are not ignored.
*/
#define CONFIG_REG 0xF5

void BME280_init(void);
void BME280_calculate(void);
float BME280_getTemp(void);
float BME280_getHum(void);
void BME280_Error_Handler(void);

typedef enum {
  TEMP_NORMAL,
  TEMP_ALARM,
} tempState_t;

void APP_update(void);
void update_lcd_clock(void);
void lcd_display_data(void);

#endif /* API_INC_DRIVER_BME280_H_ */
