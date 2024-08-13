#include "API_fsm.h"

static tempState_t currentTempState;

char strbuff[SIZE];

char message_tem[SIZE];
char message_hum[SIZE];

char lcdTempStr[SIZE];
char lcdHumStr[SIZE];

void tempFSM_init()
{
    currentTempState = TEMP_NORMAL;
}

void FSM_update(void);

void eval_data()
{
    uint8_t message[50];
    if (bme280_temperature > THRESHOLD_TEMP)
    {
        currentTempState = TEMP_ALARM;
        strcpy((char *)message, "Temperature Alarm State.\r\n");
        uartSendString(message);

#ifdef DEBUG_EVAL_DATA
        for (int i = 0; i <= 3; i++)
        {
            BSP_LED_Toggle(LED3);
            HAL_Delay(20);
        }
#endif
    }
    else
    {
        currentTempState = TEMP_NORMAL;
        strcpy((char *)message, "Temperature Normal State.\r\n");
        uartSendString(message);
    }
}

/**
 * @brief Prepares a formatted UART message with the specified tag and sensor data.
 *
 * This function takes a floating-point sensor data value, converts it to a string format with the specified tag,
 * and stores the result in the provided message buffer. The formatted string will be in the format of:
 * "Tag: <integer_part>.<fractional_part> <unit>\r\n".
 *
 * @param bme280_data: The sensor data (e.g., temperature or humidity) to be formatted.
 * @param message: Pointer to the buffer where the formatted message will be stored.
 * @param tag: Pointer to the string tag (e.g., "Temperature: " or "Humidity: ") to prepend to the data.
 * @param unit: Pointer to the unit string (e.g., "C" or "%") to append to the data.
 * @retval None
 */
void prepareUartData(float bme280_data, uint8_t *message, const char *tag, const char *unit)
{
    int intPart = (int)bme280_data;
    int fracPart = (int)(bme280_data - intPart) * 100;

    strcpy((char *)message, tag);

    memset(strbuff, ZEROVAL, sizeof(strbuff));

    itoa(intPart, strbuff, DECIMAL);
    strcat((char *)message, strbuff);

    strcat((char *)message, ".");

    memset(strbuff, ZEROVAL, sizeof(strbuff));

    itoa(fracPart, strbuff, DECIMAL);
    strcat((char *)message, strbuff);

    strcat((char *)message, " ");
    strcat((char *)message, unit);
    strcat((char *)message, "\r\n");
}

void prepare_sensor_data_for_uart(uint8_t *message_tem, uint8_t *message_hum)
{
    prepareUartData((float)bme280_temperature, message_tem, "Temperature: ", "C");
    prepareUartData((float)bme280_humidity, message_hum, "Humidity: ", "%");
}

void uart_display_data(uint8_t *message_1, uint8_t *message_2)
{
    uartSendString(message_1);
    uartSendString(message_2);
}

void prepare_sensor_data_for_lcd(void)
{
    itoa((int)bme280_temperature, lcdTempStr, 10);
    strcat(lcdTempStr, ".");
    itoa((int)((bme280_temperature - (int)bme280_temperature) * 100), lcdTempStr + strlen(lcdTempStr), 10);

    itoa((int)bme280_humidity, lcdHumStr, 10);
    strcat(lcdHumStr, ".");
    itoa((int)((bme280_humidity - (int)bme280_humidity) * 100), lcdHumStr + strlen(lcdHumStr), 10);
}

void lcd_display_sensor_data(void)
{
    API_LCD_DisplayTwoMsgs(FSM_HUM_LCD_CURSOR_POS, FSM_LCD_LINE_1, (uint8_t *)"H:", (uint8_t *)lcdHumStr);
    API_LCD_DisplayTwoMsgs(FSM_TEMP_LCD_CURSOR_POS, FSM_LCD_LINE_2, (uint8_t *)"T:", (uint8_t *)lcdTempStr);
}

void lcd_alarm()
{
    API_LCD_DisplayMsg(FSM_ALARM_LCD_CURSOR_POS, FSM_LCD_LINE_2, (uint8_t *)"ALARMA! ");
}

void lcd_display_clock()
{
    API_LCD_SetCursorLine(0, FSM_LCD_LINE_1);
    API_LCD_SendBCDData(sTime.Hours);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Minutes);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Seconds);
}

void lcd_display_date()
{
    API_LCD_SetCursorLine(0, FSM_LCD_LINE_2);
    API_LCD_SendBCDData(sDate.Date);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Month);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Year);
}

void FSM_update()
{
    eval_data();
    switch (currentTempState)
    {
    case TEMP_ALARM:
        lcd_alarm();
        break;
    case TEMP_NORMAL:
        lcd_display_date();
        break;
    default:
        break;
    }
}

void APP_init()
{
    ClockInit();
    tempFSM_init();
    API_BME280_Init();
    uartInit();
    API_LCD_Initialize();
}

void APP_updateLCD(void)
{
    ClockUpdateTimeDate();
    lcd_display_clock();
}

void APP_updateSensorData(void)
{
    API_BME280_ReadAndProcess();
}

void APP_prepareAndDisplaySensorData(void)
{
    prepare_sensor_data_for_lcd();
    lcd_display_sensor_data();
}

void APP_prepareAndSendUARTData(void)
{
    memset(message_tem, ZEROVAL, sizeof(message_tem));
    memset(message_hum, ZEROVAL, sizeof(message_hum));

    prepare_sensor_data_for_uart((char *)message_tem, (char *)message_hum);
    uart_display_data((char *)message_tem, (char *)message_hum);
}

void APP_update()
{
    APP_updateSensorData();
    APP_updateLCD();
    APP_prepareAndDisplaySensorData();
    APP_prepareAndSendUARTData();
    FSM_update();
}
