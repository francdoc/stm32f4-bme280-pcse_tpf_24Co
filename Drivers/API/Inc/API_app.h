#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"        /* <- HAL include */
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_delay.h"
#include "API_uart.h"

#include "API_lcd.h"
#include "API_lcd_port.h"

#include "API_bme280.h"
#include "API_clock_date.h"

/* APP FSM logic define parameters -------------------------------------------*/

#define THRESHOLD_TEMP 22 // Temperature threshold in degrees Celsius for state change

/* APP LCD display define parameters -----------------------------------------*/

#define APP_LCD_LINE_1 1           // LCD line 1
#define APP_LCD_LINE_2 2           // LCD line 2
#define APP_ALARM_LCD_CURSOR_POS 0 // Cursor position for alarm message on LCD
#define APP_TEMP_LCD_CURSOR_POS 9  // Cursor position for temperature on LCD
#define APP_HUM_LCD_CURSOR_POS 9   // Cursor position for humidity on LCD
#define APP_CLOCK_CURSOR_POS 0     // Cursor position for clock display on LCD

/* Miscellaneous define parameters -------------------------------------------*/

#define SIZE 50                   // Buffer size for strings
#define DECIMAL 10                // Decimal base for integer to string conversion
#define ZEROVAL 0                 // Value representing zero, used in initialization
#define FRACTIONAL_MULTIPLIER 100 // Multiplier for converting fractional part of float to integer

/* Enumerations --------------------------------------------------------------*/

/**
 * @brief Enumeration for temperature states.
 * TEMP_NORMAL: Temperature is within normal range.
 * TEMP_ALARM: Temperature exceeds the defined threshold.
 */
typedef enum
{
  TEMP_NORMAL, // Normal temperature state
  TEMP_ALARM,  // Alarm temperature state
} tempState_t;

/* Function Prototypes -------------------------------------------------------*/

/**
 * @brief Initializes the application components and prepares the system for operation.
 *        This function initializes the clock, FSM, BME280 sensor, UART, and LCD.
 * @retval None
 */
void APP_init(void);

/**
 * @brief Main update function for the application. This function handles sensor data acquisition,
 *        LCD updates, sensor data display, and FSM updates.
 * @retval None
 */
void APP_update(void);
