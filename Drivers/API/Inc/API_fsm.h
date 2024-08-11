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

#define THRESHOLD_TEMP 24

typedef enum
{
  TEMP_NORMAL,
  TEMP_ALARM,
} tempState_t;

void lcd_display_date(void);
void lcd_display_clock(void);

void lcd_alarm();

void prepare_sensor_data_for_lcd(void);
void lcd_display_sensor_data(void);
void prepare_sensor_data_for_uart(uint8_t *message_1, uint8_t *message_2);
void uart_display_data(uint8_t *message_1, uint8_t *message_2);

void APP_init(void);
void APP_update(void);
void APP_updateLCD(void);
void APP_updateSensorData(void);
void APP_prepareAndDisplaySensorData(void);
void APP_prepareAndSendUARTData(void);
