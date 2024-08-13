#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"        /* <- HAL include */
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */
#include "API_lcd.h"              /* <- LCD driver include */

#define LCD_WRITE_CMD 1

/* I2C handler declaration */

extern I2C_HandleTypeDef hi2c1;

/* Exported functions ------------------------------------------------------- */

void LCD_HAL_I2C_Write(uint8_t valor);
void LCD_HAL_Delay(uint32_t delay);
void LCD_HAL_Blink(Led_TypeDef Led);
