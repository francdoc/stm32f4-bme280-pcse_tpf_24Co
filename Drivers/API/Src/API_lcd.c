#include "API_lcd.h"

static void ControlLcd(uint8_t valor);
static void Envia8bitsLcd(uint8_t valor, _Bool tipo);
static void Envia4bitsLcd(uint8_t valor, _Bool tipo);

static const uint8_t LCD_INIT_CMD[] = {
	_4BIT_MODE, DISPLAY_CONTROL, RETURN_HOME, ENTRY_MODE + AUTOINCREMENT, DISPLAY_CONTROL + DISPLAY_ON, CLR_LCD};

_Bool Init_Lcd(void)
{
	LCD_HAL_Delay(millisecond * 20);
	Envia4bitsLcd(COMANDO_INI1, CONTROL);
	LCD_HAL_Delay(millisecond * 10);
	Envia4bitsLcd(COMANDO_INI1, CONTROL);
	LCD_HAL_Delay(millisecond * 1);
	Envia4bitsLcd(COMANDO_INI1, CONTROL);
	Envia4bitsLcd(COMANDO_INI2, CONTROL);
	for (uint8_t i = 0; i < sizeof(LCD_INIT_CMD); i++) {
		ControlLcd(LCD_INIT_CMD[i]);
	}

	LCD_HAL_Delay(millisecond * 2);

	// Signal that LCD initialization is OK.
	for (int i = 0; i <= 4; i++)
	{
		LCD_HAL_Blink(LED1); // Ini LCD OK.
		LCD_HAL_Delay(millisecond * 1);
	}
	return 0;
}

// public
void DatoLcd(uint8_t dato)
{
	Envia8bitsLcd(dato, DATOS);
}

// public
void DatoBCD(uint8_t dato)
{
	DatoAsciiLcd((((dato) & 0xf0) >> 4));
	DatoAsciiLcd(((dato) & 0x0f));
}

static void ControlLcd(uint8_t valor)
{
	Envia8bitsLcd(valor, CONTROL);
}

static void Envia8bitsLcd(uint8_t valor, _Bool tipo)
{
	Envia4bitsLcd(valor & HIGH_NIBBLE, tipo);
	Envia4bitsLcd(valor << LOW_NIBBLE, tipo);
}

static void DatoAsciiLcd(uint8_t dato)
{
	Envia8bitsLcd(dato + '0', DATOS);
}

static void Envia4bitsLcd(uint8_t valor, _Bool tipo)
{
	LCD_HAL_I2C_Write(valor + tipo + EN + BL);
	LCD_HAL_Delay(millisecond);
	LCD_HAL_I2C_Write(valor + tipo + BL);
	LCD_HAL_Delay(millisecond);
}

// public
void SacaTextoLcd(uint8_t *texto)
{
	while (*texto)
		DatoLcd(*texto++);
}

// public
void PosCaracHLcd(uint8_t posH)
{
	ControlLcd(posH | LINEA1);
}

// public
void PosCaracLLcd(uint8_t posL)
{
	ControlLcd(posL | LINEA2);
}
