/*
 * This code is based on the original implementation by professor Israel Pavelek,
 * as provided in his class. The original code has been refactored and modified to
 * integrate into the API FSM system, making it compatible with the specific
 * requirements of this project. These modifications include changes in function names,
 * the addition of input/output control variables, and adaptation to the project's
 * coding standards.
 * */

 #include "API_lcd.h"

/* Private Function Prototypes ---------------------------------------------- */
static void ExecuteLCDCommand(uint8_t command);
static void SendNibbleToLCD(uint8_t data, bool mode);
static void WriteDataToLCD(uint8_t data, bool mode);
static void SendAsciiCharToLCD(uint8_t asciiChar);

// Initialization sequence commands
static const uint8_t LCD_INIT_COMMANDS[] = {
	LCD_4BIT_MODE_CMD,
	LCD_DISPLAY_CONTROL_CMD,
	LCD_RETURN_HOME_CMD,
	LCD_ENTRY_MODE_CMD + LCD_INCREMENT_MODE,
	LCD_DISPLAY_CONTROL_CMD + LCD_DISPLAY_ON,
	LCD_CLEAR_CMD};

/* Private Function Definitions --------------------------------------------- */

/**
 * @brief Sends 4 bits to the LCD.
 * @param data: The data to send.
 * @param mode: The mode (command/data).
 * @retval None.
 */
static void SendNibbleToLCD(uint8_t data, bool mode)
{
	LCD_HAL_I2C_Write(data | mode | LCD_ENABLE_PIN | LCD_BACKLIGHT);
	LCD_HAL_Delay(1 * MILLISECOND);
	LCD_HAL_I2C_Write(data | mode | LCD_BACKLIGHT);
	LCD_HAL_Delay(1 * MILLISECOND);
}

/**
 * @brief Executes a command on the LCD.
 * @param command: The command to be sent.
 * @retval None.
 */
static void ExecuteLCDCommand(uint8_t command)
{
	WriteDataToLCD(command, LCD_CMD_CONTROL_MODE);
}

/**
 * @brief Writes 8 bits to the LCD by sending two 4-bit sequences.
 * @param data: The data to send.
 * @param mode: The mode (command/data).
 * @retval None.
 */
static void WriteDataToLCD(uint8_t data, bool mode)
{
	SendNibbleToLCD(data & LCD_HIGH_NIBBLE_MASK, mode);
	SendNibbleToLCD(data << LCD_LOW_NIBBLE_SHIFT, mode);
}

/**
 * @brief Sends an ASCII character to the LCD.
 * @param asciiChar: The character to send.
 * @retval None.
 */
static void SendAsciiCharToLCD(uint8_t asciiChar)
{
	WriteDataToLCD(asciiChar + '0', LCD_CMD_DATA_MODE);
}

/* Public Function Definitions ----------------------------------------------- */

/**
 * @brief Initializes the LCD with the predefined commands.
 * @retval _Bool: Returns 0 on success.
 */
_Bool API_LCD_Initialize(void)
{
	LCD_HAL_Delay(MILLISECOND * 20);

	SendNibbleToLCD(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE);
	LCD_HAL_Delay(MILLISECOND * 10);

	SendNibbleToLCD(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE);
	LCD_HAL_Delay(MILLISECOND);

	SendNibbleToLCD(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE);
	SendNibbleToLCD(LCD_INIT_CMD_2, LCD_CMD_CONTROL_MODE);

	for (uint8_t i = 0; i < sizeof(LCD_INIT_COMMANDS); i++)
	{
		ExecuteLCDCommand(LCD_INIT_COMMANDS[i]);
	}

	LCD_HAL_Delay(MILLISECOND);

	for (int i = 0; i < 4; i++) // LCD init OK blink signal
	{
		LCD_HAL_Blink(LED1);
		LCD_HAL_Delay(MILLISECOND);
	}

	return 0;
}

/**
 * @brief Sends data to the LCD.
 * @param data: The data to send.
 * @retval None.
 */
void API_LCD_SendData(uint8_t data)
{
	WriteDataToLCD(data, LCD_CMD_DATA_MODE);
}

/**
 * @brief Sends a BCD-encoded byte to the LCD.
 * @param data: The BCD data to send.
 * @retval None.
 */
void API_LCD_SendBCDData(uint8_t data)
{
	SendAsciiCharToLCD((data & 0xF0) >> 4);
	SendAsciiCharToLCD(data & 0x0F);
}

/**
 * @brief Displays a string on the LCD.
 * @param text: Pointer to the string to be displayed.
 * @retval None.
 */
void API_LCD_DisplayString(uint8_t *text)
{
	while (*text)
	{
		API_LCD_SendData(*text++);
	}
}

/**
 * @brief Sets the cursor position on the first or second line of the LCD
 * based on its line input.
 * @param position: The position to set the cursor to.
 * @retval None.
 */
void API_LCD_SetCursorLine(uint8_t position, uint8_t lcd_line)
{
	if (lcd_line == 1)
	{
		ExecuteLCDCommand(position | LCD_LINE_1);
	}
	if (lcd_line == 2)
	{
		ExecuteLCDCommand(position | LCD_LINE_2);
	}
}

/* Ad-hoc function to display two consecutive messages on the same LCD line in a single API call.
 * This approach avoids the use for dynamic memory allocation functions for concatenating strings,
 * such as malloc, which would provide more flexibility but introduces potential risks in embedded systems.
 * The downside of this approach is that it is limited to handling a maximum of two input strings
 * per LCD line.*/
void API_LCD_DisplayTwoMsgs(uint8_t init_pos, uint8_t lcd_line, uint8_t *message1, uint8_t *message2)
{
	API_LCD_SetCursorLine(init_pos, lcd_line);
	API_LCD_DisplayString(message1);
	API_LCD_DisplayString(message2);
}

void API_LCD_DisplayMsg(uint8_t init_pos, uint8_t lcd_line, uint8_t *message)
{
	API_LCD_SetCursorLine(init_pos, lcd_line);
	API_LCD_DisplayString(message);
}
