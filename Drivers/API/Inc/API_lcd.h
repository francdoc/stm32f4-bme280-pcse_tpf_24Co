#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "API_lcd_port.h"

// Define constants with meaningful names
#define LCD_CMD_DATA_MODE 0x01
#define LCD_CMD_CONTROL_MODE 0x00
#define LCD_I2C_ADDRESS 0x27
#define LCD_ENABLE_PIN 0x04
#define LCD_BACKLIGHT 0x08
#define LCD_CLEAR_CMD 0x01
#define LCD_RETURN_HOME_CMD 0x02
#define LCD_ENTRY_MODE_CMD 0x04
#define LCD_DISPLAY_CONTROL_CMD 0x08
#define LCD_CURSOR_SHIFT_CMD 0x10
#define LCD_FUNCTION_SET_CMD 0x20
#define LCD_SET_CGRAM_CMD 0x40
#define LCD_SET_DDRAM_CMD 0x80
#define LCD_DISPLAY_ON 0x04
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_BLINK_ON 0x01
#define LCD_INCREMENT_MODE 0x02
#define LCD_4BIT_MODE_CMD 0x28
#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0

#define LCD_INIT_CMD_1 0x30
#define LCD_INIT_CMD_2 0x20

#define LCD_HIGH_NIBBLE_MASK 0xF0
#define LCD_LOW_NIBBLE_SHIFT 4

#define LCD_DELAY_MULTIPLIER 1

// Public API functions
_Bool API_LCD_Initialize(void);
void API_LCD_SendData(uint8_t data);
void API_LCD_SendBCDData(uint8_t data);
void API_LCD_DisplayString(uint8_t *text);
void API_LCD_SetCursorLine1(uint8_t position);
void API_LCD_SetCursorLine2(uint8_t position);

#endif /* API_INC_API_LCD_H_ */
