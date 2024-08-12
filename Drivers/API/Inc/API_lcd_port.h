#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "API_lcd.h"

extern I2C_HandleTypeDef hi2c1;

extern void Error_Handler(void);
void LCD_HAL_I2C_Write(uint8_t valor);
