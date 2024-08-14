#include "API_app.h"

static tempState_t currentTempState;

char strbuff[SIZE];

char message_tem[SIZE];
char message_hum[SIZE];

char lcdTempStr[SIZE];
char lcdHumStr[SIZE];

char messageFsm[50];

void APP_FSM_init(void);
void APP_evalData(void);
void APP_uartPrepareData(float bme280_data, char *message, const char *tag, const char *unit);
void APP_uartPrepareSensorTempHum(char *message_tem, char *message_hum);
void APP_uartDisplaySensorData(char *message_tem, char *message_hum);
void APP_lcdPrepareSensorData(void);
void APP_lcdDisplaySensorData(void);
void APP_lcdAlarm(void);
void APP_lcdDisplayClock(void);
void APP_lcdDisplayDate(void);
void APP_FSM_update(void);
void APP_lcdUpdateTime(void);
void APP_updateSensorData(void);
void APP_prepareAndDisplaySensorData(void);

void APP_FSM_init(void)
{
    currentTempState = TEMP_NORMAL;
}

void APP_evalData(void)
{
    memset(messageFsm, ZEROVAL, sizeof(messageFsm));

    if (bme280_temperature > THRESHOLD_TEMP)
    {
        currentTempState = TEMP_ALARM;
        strcpy(messageFsm, "Temperature Alarm State.\r\n");
        uartSendString((uint8_t *)messageFsm);
    }
    else
    {
        currentTempState = TEMP_NORMAL;
        strcpy(messageFsm, "Temperature Normal State.\r\n");
        uartSendString((uint8_t *)messageFsm);
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
void APP_uartPrepareData(float bme280_data, char *message, const char *tag, const char *unit)
{
    int intPart = (int)bme280_data;
    int fracPart = (int)((bme280_data - intPart) * 100);

    strcpy(message, tag);

    memset(strbuff, ZEROVAL, sizeof(strbuff));

    itoa(intPart, strbuff, DECIMAL);
    strcat(message, strbuff);

    strcat(message, ".");

    memset(strbuff, ZEROVAL, sizeof(strbuff));

    itoa(fracPart, strbuff, DECIMAL);
    strcat(message, strbuff);

    strcat(message, " ");
    strcat(message, unit);
    strcat(message, "\r\n");
}

void APP_uartPrepareSensorTempHum(char *message_tem, char *message_hum)
{
	APP_uartPrepareData((float)bme280_temperature, message_tem, "Temperature: ", "C");
	APP_uartPrepareData((float)bme280_humidity, message_hum, "Humidity: ", "%");
}

void APP_uartDisplaySensorData(char *message_1, char *message_2)
{
    uartSendString((uint8_t *)message_1);
    uartSendString((uint8_t *)message_2);
}

void APP_lcdPrepareSensorData(void)
{
    itoa((int)bme280_temperature, lcdTempStr, DECIMAL);
    strcat(lcdTempStr, ".");
    itoa((int)((bme280_temperature - (int)bme280_temperature) * 100), lcdTempStr + strlen(lcdTempStr), DECIMAL);

    itoa((int)bme280_humidity, lcdHumStr, DECIMAL);
    strcat(lcdHumStr, ".");
    itoa((int)((bme280_humidity - (int)bme280_humidity) * 100), lcdHumStr + strlen(lcdHumStr), DECIMAL);
}

void APP_lcdDisplaySensorData(void)
{
    API_LCD_DisplayTwoMsgs(APP_HUM_LCD_CURSOR_POS, APP_LCD_LINE_1, (uint8_t *)"H:", (uint8_t *)lcdHumStr);
    API_LCD_DisplayTwoMsgs(APP_TEMP_LCD_CURSOR_POS, APP_LCD_LINE_2, (uint8_t *)"T:", (uint8_t *)lcdTempStr);
}

void APP_lcdAlarm(void)
{
    API_LCD_DisplayMsg(APP_ALARM_LCD_CURSOR_POS, APP_LCD_LINE_2, (uint8_t *)"ALARMA! ");
}

void APP_lcdDisplayClock(void)
{
    API_LCD_SetCursorLine(APP_CLOCK_CURSOR_POS, APP_LCD_LINE_1);
    API_LCD_SendBCDData(sTime.Hours);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Minutes);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Seconds);
}

void APP_lcdDisplayDate(void)
{
    API_LCD_SetCursorLine(APP_CLOCK_CURSOR_POS, APP_LCD_LINE_2);
    API_LCD_SendBCDData(sDate.Date);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Month);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Year);
}

void APP_FSM_update(void)
{
    APP_evalData();
    switch (currentTempState)
    {
    case TEMP_ALARM:
        APP_lcdAlarm();
        break;
    case TEMP_NORMAL:
        APP_lcdDisplayDate();
        break;
    default:
        break;
    }
}

void APP_lcdUpdateTime(void)
{
    ClockUpdateTimeDate();
    APP_lcdDisplayClock();
}

// Updates bme280_temperature and bme280_humidity global variables
void APP_updateSensorData(void)
{
    API_BME280_ReadAndProcess();
}

void APP_prepareAndDisplaySensorData(void)
{
    APP_lcdPrepareSensorData();
    APP_lcdDisplaySensorData();
}

void APP_prepareAndSendUARTData(void)
{
    memset(message_tem, ZEROVAL, sizeof(message_tem));
    memset(message_hum, ZEROVAL, sizeof(message_hum));

    APP_uartPrepareSensorTempHum(message_tem, message_hum);
    APP_uartDisplaySensorData(message_tem, message_hum);
}

// global function
void APP_init()
{
    ClockInit();
    APP_FSM_init();
    API_BME280_Init();
    uartInit();
    API_LCD_Initialize();
}

// global function
void APP_update()
{
    APP_updateSensorData();
    APP_lcdUpdateTime();
    APP_prepareAndDisplaySensorData();
    APP_prepareAndSendUARTData();
    APP_FSM_update();
}
