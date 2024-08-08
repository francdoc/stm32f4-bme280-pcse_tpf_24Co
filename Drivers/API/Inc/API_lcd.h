#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"

#include "API_Lcd_port.h"

#define DATOS 0x01
#define CONTROL 0x00
#define LCD_DIR 0x27
#define EN 0x04
#define BL 0x08
#define CLR_LCD 0x01
#define RETURN_HOME 0x02
#define ENTRY_MODE 0x04
#define DISPLAY_CONTROL 0x08
#define CURSOR_DISPLAY_SHIFT 0x10
#define FUNCTION_SET 0x20
#define SET_CGRAM 0x40
#define SET_DDRAM 0x80
#define DISPLAY_ON 0x04
#define CURSOR_ON 0x02
#define CURSOR_BLINK 0x01
#define AUTOINCREMENT 0x02
#define _4BIT_MODE 0x28
#define LINEA1 0x80
#define LINEA2 0xc0

#define COMANDO_INI1 0x30
#define COMANDO_INI2 0x20

#define HIGH_NIBBLE 0xf0
#define LOW_NIBBLE 0x04

#define millisecond 1

_Bool Init_Lcd(void);
void SacaTextoLcd(uint8_t *texto);
void DatoLcd(uint8_t dato);
void DatoAsciiLcd(uint8_t dato);
void DatoBCD(uint8_t dato);
void ClrLcd(void);
void PosCaracHLcd(uint8_t posH);
void PosCaracLLcd(uint8_t posL);
void CursorOffLcd(void);
void CursorOnLcd(void);

#endif
