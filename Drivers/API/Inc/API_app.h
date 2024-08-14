#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

#include "API_delay.h"
#include "API_uart.h"

#include "API_lcd.h"
#include "API_lcd_port.h"

#include "API_bme280.h"

#include "API_clock_date.h"

// APP FSM logic define parameters
#define THRESHOLD_TEMP 22 // degrees celsius

// APP LCD display define parameters
#define APP_LCD_LINE_1 1
#define APP_LCD_LINE_2 2
#define APP_ALARM_LCD_CURSOR_POS 0
#define APP_TEMP_LCD_CURSOR_POS 9
#define APP_HUM_LCD_CURSOR_POS 9
#define APP_CLOCK_CURSOR_POS 0

#define SIZE 50
#define DECIMAL 10
#define ZEROVAL 0
#define FRACTIONAL_MULTIPLIER 100

typedef enum
{
  TEMP_NORMAL,
  TEMP_ALARM,
} tempState_t;

void APP_init(void);
void APP_update(void);
