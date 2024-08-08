#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#include "API_lcd.h"

extern void Error_Handler(void);
void LCD_Write_Byte(uint8_t valor);