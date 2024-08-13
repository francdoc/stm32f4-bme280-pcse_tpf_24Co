#include "API_lcd.h"

/* Private Function Prototypes ---------------------------------------------- */
static void sendNibbleToLCD(uint8_t data, bool mode);
static void sendNibbleAndPause(uint8_t data, bool mode, uint8_t delay);
static void writeDataToLCD(uint8_t data, bool mode);
static void executeLCDCommand(uint8_t command);
static void sendAsciiCharToLCD(uint8_t asciiChar);
static void okLcdInitSignal(void);
static void errorLedSignal(void);

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
 * @param mode: The mode (command/data). Must be LCD_CMD_CONTROL_MODE or LCD_CMD_DATA_MODE.
 * @retval None.
 */
static void sendNibbleToLCD(uint8_t data, bool mode)
{
    if (mode != LCD_CMD_CONTROL_MODE && mode != LCD_CMD_DATA_MODE)
    {
        errorLedSignal();
    }

    LCD_HAL_I2C_Write(data | mode | LCD_ENABLE_PIN | LCD_BACKLIGHT);
    LCD_HAL_Delay(1 * MILLISECOND);
    LCD_HAL_I2C_Write(data | mode | LCD_BACKLIGHT);
    LCD_HAL_Delay(1 * MILLISECOND);
}

/**
 * @brief Sends nibble to LCD and applies a specified input.
 * @param data: Data to send. Must be within the range of a nibble (0x0 - 0xF0).
 * @param pause: The delay to apply after sending the nibble. Must be a reasonable delay value.
 * @param mode: The mode (command/data). Must be LCD_CMD_CONTROL_MODE or LCD_CMD_DATA_MODE.
 * @retval None.
 */
static void sendNibbleAndPause(uint8_t data, bool mode, uint8_t delay)
{
    if (mode != LCD_CMD_CONTROL_MODE && mode != LCD_CMD_DATA_MODE)
    {
        errorLedSignal();
    }

    sendNibbleToLCD(data, mode);
    LCD_HAL_Delay(delay);
}

/**
 * @brief Writes 8 bits to the LCD by sending two 4-bit sequences.
 * @param data: The data to send. Must be a valid byte value.
 * @param mode: The mode (command/data). Must be LCD_CMD_CONTROL_MODE or LCD_CMD_DATA_MODE.
 * @retval None.
 */
static void writeDataToLCD(uint8_t data, bool mode)
{
    if (mode != LCD_CMD_CONTROL_MODE && mode != LCD_CMD_DATA_MODE)
    {
        errorLedSignal();
    }

    sendNibbleToLCD(data & LCD_HIGH_NIBBLE_MASK, mode);
    sendNibbleToLCD(data << LCD_LOW_NIBBLE_SHIFT, mode);
}

/**
 * @brief Executes a command on the LCD.
 * @param command: The command to be sent. Must be a valid LCD command.
 * @retval None.
 */
static void executeLCDCommand(uint8_t command)
{
    // Assume command is always valid as it comes from predefined constants
    writeDataToLCD(command, LCD_CMD_CONTROL_MODE);
}

/**
 * @brief Sends an ASCII character to the LCD.
 * @param asciiChar: The character to send.
 * @retval None.
 */
static void sendAsciiCharToLCD(uint8_t asciiChar)
{
    writeDataToLCD(asciiChar + ASCII_DIGIT_OFFSET, LCD_CMD_DATA_MODE);
}

/**
 * @brief  Flashes LED to signal an error in LCD operations.
 * @param  None
 * @retval None
 */
static void errorLedSignal(void)
{
    for (int i = 0; i <= NUM_ERROR_LCD_BLINKS; i++)
    {
        LCD_HAL_Blink(LED3); // Sensor error.
        LCD_HAL_Delay(MILLISECOND * 100);
    }
}

/**
 * @brief Signals the successful initialization of the LCD by blinking an LED.
 *
 * This function provides a visual indication that the LCD has been successfully initialized.
 * It toggles an LED (LED1) four times.
 * @retval None.
 */
static void okLcdInitSignal(void)
{
    for (int i = 0; i < NUM_OK_INIT_LCD_BLINKS; i++) // LCD init OK blink signal
    {
        LCD_HAL_Blink(LED1);
        LCD_HAL_Delay(MILLISECOND);
    }
}

/* Public Function Definitions ----------------------------------------------- */

/**
 * @brief Initializes the LCD with the predefined commands.
 * @retval _Bool: Returns 0 on success.
 */
_Bool API_LCD_Initialize(void)
{
    LCD_HAL_Delay(MILLISECOND * 20);

    sendNibbleAndPause(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE, MILLISECOND * 10);
    sendNibbleAndPause(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE, MILLISECOND * 10);
    sendNibbleAndPause(LCD_INIT_CMD_1, LCD_CMD_CONTROL_MODE, MILLISECOND * 10);
    sendNibbleAndPause(LCD_INIT_CMD_2, LCD_CMD_CONTROL_MODE, MILLISECOND * 10);

    for (uint8_t i = 0; i < sizeof(LCD_INIT_COMMANDS); i++)
    {
        executeLCDCommand(LCD_INIT_COMMANDS[i]);
    }

    okLcdInitSignal();

    LCD_HAL_Delay(MILLISECOND);

    return 0;
}

/**
 * @brief Sends data to the LCD.
 * @param data: The data to send. Must be a valid byte value.
 * @retval None.
 */
void API_LCD_SendData(uint8_t data)
{
    writeDataToLCD(data, LCD_CMD_DATA_MODE);
}

/**
 * @brief Sends a BCD-encoded byte to the LCD. This function prepares and formats data.
 * @param data: The BCD data to send. Must be a valid BCD-encoded byte.
 * @retval None.
 */
void API_LCD_SendBCDData(uint8_t data)
{
    sendAsciiCharToLCD((data & BCD_HIGH_NIBBLE_MASK) >> BCD_HIGH_NIBBLE_SHIFT);
    sendAsciiCharToLCD(data & BCD_LOW_NIBBLE_MASK);
}

/**
 * @brief Displays a string on the LCD.
 *
 * This function loops through each character in the provided string
 * and sends it to the LCD one by one. The string should be null-terminated,
 * and the LCD cursor will automatically move to the next position after each character.
 *
 * @param text: Pointer to the null-terminated string to be displayed.
 * @retval None.
 */
void API_LCD_DisplayString(uint8_t *text)
{
    // Loop through each character in the string until the null terminator is encountered
    while (*text != EOL)
    {
        // Send the current character to the LCD
        API_LCD_SendData(*text);

        // Move to the next character in the string
        text++;
    }
}

/**
 * @brief Sets the cursor position on the first or second line of the LCD
 * based on its line input.
 *
 * This function moves the cursor to the specified position on either the first or second line
 * of the LCD, depending on the `lcd_line` parameter. The `position` parameter specifies the
 * horizontal position on the line, starting from the leftmost position (0).
 *
 * @param position: The cursor position relative to the start of the line. Should be within the LCD's width.
 * @param lcd_line: Specifies the LCD line to set the cursor to (1 for the first line, 2 for the second line).
 * @retval None.
 */
void API_LCD_SetCursorLine(uint8_t position, uint8_t lcd_line)
{
    if (lcd_line == LCD_FIRST_ROW_INDEX)
    {
        executeLCDCommand(position | LCD_LINE_1);
    }
    else if (lcd_line == LCD_SECOND_ROW_INDEX)
    {
        executeLCDCommand(position | LCD_LINE_2);
    }
}

/**
 * @brief Displays two consecutive messages on the same LCD line in a single API call.
 *
 * This ad-hoc function allows you to display two messages on a specified LCD line starting from an initial position.
 * This function avoids the use of dynamic memory allocation (e.g., malloc) which, while providing flexibility, can introduce
 * potential risks in embedded systems. This approach is safer but limited to handling a maximum of two input strings per LCD line.
 *
 * @param init_pos: The initial cursor position on the LCD line. Should be within the LCD's width.
 * @param lcd_line: Specifies the LCD line to display the messages on (1 for the first line, 2 for the second line).
 * @param message1: Pointer to the first message string to be displayed.
 * @param message2: Pointer to the second message string to be displayed.
 * @retval None.
 */
void API_LCD_DisplayTwoMsgs(uint8_t init_pos, uint8_t lcd_line, uint8_t *message1, uint8_t *message2)
{
    API_LCD_SetCursorLine(init_pos, lcd_line);
    API_LCD_DisplayString(message1);
    API_LCD_DisplayString(message2);
}

/**
 * @brief Displays a single message on a specified LCD line at a given position.
 *
 * This function sets the cursor to a specified position on a given LCD line and displays the provided message.
 * It is useful for displaying individual messages on the LCD without the need for string concatenation.
 *
 * @param init_pos: The initial cursor position on the LCD line. Should be within the LCD's width.
 * @param lcd_line: Specifies the LCD line to display the message on (1 for the first line, 2 for the second line).
 * @param message: Pointer to the message string to be displayed.
 * @retval None.
 */
void API_LCD_DisplayMsg(uint8_t init_pos, uint8_t lcd_line, uint8_t *message)
{
    API_LCD_SetCursorLine(init_pos, lcd_line);
    API_LCD_DisplayString(message);
}

/**
 * @brief  This function is executed in case of error occurrence. Program will get stuck in this part of the code. Indicating major LCD error.
 * @retval None
 */
void API_LCD_ErrorHandler(void)
{
    while (1)
    {
        errorLedSignal();
    }
}
