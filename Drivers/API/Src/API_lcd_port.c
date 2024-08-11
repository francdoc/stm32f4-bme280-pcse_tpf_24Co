#include"API_lcd_port.h"

extern I2C_HandleTypeDef hi2c1;

void LCD_HAL_I2C_Write(uint8_t valor){
	HAL_I2C_Master_Transmit (&hi2c1, LCD_DIR<<1, &valor, sizeof(valor), HAL_MAX_DELAY);
}
