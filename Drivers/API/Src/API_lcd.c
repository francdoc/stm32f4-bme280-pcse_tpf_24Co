#include "API_lcd.h"

static void DelayLcd(uint32_t demora);
static void ControlLcd(uint8_t valor);
static void Envia8bitsLcd (uint8_t valor,_Bool tipo);
static void Envia4bitsLcd (uint8_t valor,_Bool tipo);

static const uint8_t LCD_INIT_CMD[]={
		_4BIT_MODE,DISPLAY_CONTROL,RETURN_HOME,ENTRY_MODE+AUTOINCREMENT,DISPLAY_CONTROL+DISPLAY_ON,CLR_LCD
};

_Bool Init_Lcd(void){
   DelayLcd(millisecond*20);
   Envia4bitsLcd(COMANDO_INI1,CONTROL);
   DelayLcd(millisecond*10);
   Envia4bitsLcd(COMANDO_INI1,CONTROL);
   DelayLcd(millisecond*1);
   Envia4bitsLcd(COMANDO_INI1,CONTROL);
   Envia4bitsLcd(COMANDO_INI2,CONTROL);
   for(uint8_t i=0;i<sizeof(LCD_INIT_CMD);i++)ControlLcd(LCD_INIT_CMD[i]);
   DelayLcd(millisecond*2);
   for (int i = 0; i <= 3; i++)
    {
 	   BSP_LED_Toggle(LED1); // init LCD OK
 	   HAL_Delay(millisecond*250);
    }
   return LCD_OK;
}

static void DelayLcd(uint32_t demora){
	  HAL_Delay(demora);
}

static void ControlLcd(uint8_t valor){
	Envia8bitsLcd(valor,CONTROL);
}

void DatoLcd (uint8_t dato){
	Envia8bitsLcd(dato,DATOS);
}

void DatoAsciiLcd (uint8_t dato){
	Envia8bitsLcd(dato+ '0',DATOS);
}

void DatoBCD (uint8_t dato){
	  DatoAsciiLcd((((dato)&0xf0)>>4));
	  DatoAsciiLcd(((dato)&0x0f));
}

void Envia8bitsLcd (uint8_t valor,_Bool tipo){
	Envia4bitsLcd(valor&HIGH_NIBBLE,tipo);
	Envia4bitsLcd(valor<<LOW_NIBBLE,tipo);
}

static void Envia4bitsLcd (uint8_t valor,_Bool tipo){
	LCD_Write_Byte(valor+tipo+EN+BL);
	DelayLcd(millisecond);
	LCD_Write_Byte(valor+tipo+BL);
	DelayLcd(millisecond);
}

void SacaTextoLcd (uint8_t *texto){
	while(*texto)DatoLcd(*texto++);
}

void ClrLcd(void){
   ControlLcd(CLR_LCD);
   DelayLcd(millisecond*2);

}

void PosCaracHLcd(uint8_t posH){
	ControlLcd(posH | LINEA1);
}

void PosCaracLLcd(uint8_t posL){
	ControlLcd(posL | LINEA2);
}

void CursorOffLcd(void){
	ControlLcd(DISPLAY_CONTROL+DISPLAY_ON);
}

void CursorOnLcd(void){
	ControlLcd(DISPLAY_CONTROL+CURSOR_ON+DISPLAY_ON+CURSOR_BLINK);
}