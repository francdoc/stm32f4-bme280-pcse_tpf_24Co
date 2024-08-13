#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"

#include "API_lcd_port.h"

/* Constants ----------------------------------------------------------------*/

// LCD control and data mode flags
#define LCD_CMD_CONTROL_MODE 0x00
#define LCD_CMD_DATA_MODE 0x01

// LCD I2C address and control pins
#define LCD_I2C_ADDRESS 0x27 // I2C address of the LCD
#define LCD_ENABLE_PIN 0x04  // Enable pin for the LCD
#define LCD_BACKLIGHT 0x08   // Backlight control for the LCD

// LCD command codes
#define LCD_CLEAR_CMD 0x01
#define LCD_RETURN_HOME_CMD 0x02
#define LCD_ENTRY_MODE_CMD 0x04      // Command to set the entry mode for the LCD
#define LCD_DISPLAY_CONTROL_CMD 0x08 //  Command to control the display settings
#define LCD_CURSOR_SHIFT_CMD 0x10
#define LCD_FUNCTION_SET_CMD 0x20
#define LCD_SET_CGRAM_CMD 0x40
#define LCD_SET_DDRAM_CMD 0x80

// LCD display control flags
#define LCD_DISPLAY_ON 0x04      //  Flag to turn on the display
#define LCD_CURSOR_ON 0x02       // Flag to turn on the cursor
#define LCD_CURSOR_BLINK_ON 0x01 //  Flag to enable cursor blinking

// LCD entry mode flags
#define LCD_INCREMENT_MODE 0x02 //  Flag to set cursor movement direction to increment

// LCD function set flags
#define LCD_4BIT_MODE_CMD 0x28 //  Command to set LCD to 4-bit mode

// LCD line addresses
#define LCD_LINE_1 0x80 // Address for the first line of the LCD
#define LCD_LINE_2 0xC0 //  Address for the second line of the LCD

// LCD initialization commands
#define LCD_INIT_CMD_1 0x30 //  Initial command for LCD setup (sequence 1)
#define LCD_INIT_CMD_2 0x20 //  Initial command for LCD setup (sequence 2)

// LCD data handling
#define LCD_HIGH_NIBBLE_MASK 0xF0 //  Mask to extract the high nibble of a byte
#define LCD_LOW_NIBBLE_SHIFT 4    //  Number of bits to shift for the low nibble

// Time constants
#define MILLISECOND 1 // Time constant for delays in milliseconds

// ASCII conversion
#define ASCII_DIGIT_OFFSET '0' //  Offset to convert a numerical digit to ASCII representation

// LCD row index code
#define LCD_FIRST_ROW_INDEX 1
#define LCD_SECOND_ROW_INDEX 2

// Number of LED blinks to indicate successful initialization of the LCD
#define NUM_OK_INIT_LCD_BLINKS 4

// Number of LED blinks to indicate an error in LCD operations
#define NUM_ERROR_LCD_BLINKS 4

// End of line terminator
#define EOL '\0'

// Define constants for bit manipulation
#define BCD_HIGH_NIBBLE_MASK 0xF0 // Mask to extract the high nibble (upper 4 bits)
#define BCD_LOW_NIBBLE_MASK 0x0F  // Mask to extract the low nibble (lower 4 bits)
#define BCD_HIGH_NIBBLE_SHIFT 4   // Number of bits to shift the high nibble to the right

/* Public API Functions ----------------------------------------------------- */

_Bool API_LCD_Initialize(void);
void API_LCD_SendData(uint8_t data);
void API_LCD_SendBCDData(uint8_t data);
void API_LCD_DisplayString(uint8_t *text);
void API_LCD_SetCursorLine(uint8_t position, uint8_t lcd_line);
void API_LCD_DisplayTwoMsgs(uint8_t init_pos, uint8_t lcd_line, uint8_t *message1, uint8_t *message2);
void API_LCD_DisplayMsg(uint8_t init_pos, uint8_t lcd_line, uint8_t *message);
void API_LCD_ErrorHandler(void);

#endif /* API_INC_API_LCD_H_ */
