#include "API_app.h"

/* Global and Static Variables -------------------------------------------------------*/
static tempState_t currentTempState;

char strbuff[SIZE];
char message_tem[SIZE];
char message_hum[SIZE];
char lcdTempStr[SIZE];
char lcdHumStr[SIZE];
char messageFsm[50];

/* Function Prototypes -------------------------------------------------------------*/
static void APP_FSM_init(void);
static void APP_FSM_update(void);

static void APP_uartPrepareData(float bme280_data, char *message, const char *tag, const char *unit);
static void APP_uartPrepareSensorTempHum(char *message_tem, char *message_hum);
static void APP_uartDisplaySensorData(char *message_tem, char *message_hum);
static void APP_lcdPrepareSensorData(void);
static void APP_lcdDisplaySensorData(void);
static void APP_lcdAlarm(void);
static void APP_lcdDisplayClock(void);
static void APP_lcdDisplayDate(void);
static void APP_lcdUpdateTime(void);
static void APP_updateSensorData(void);
static void APP_prepareAndDisplaySensorData(void);
static void APP_prepareAndSendUARTData(void);
static void APP_FsmErrorHandler(void);

/* Private Function Definitions --------------------------------------------- */

/**
 * @brief Initializes the finite state machine (FSM) to the default temperature state.
 * @retval None
 */
void APP_FSM_init(void)
{
    currentTempState = TEMP_NORMAL;
}

/**
 * @brief Prepares a UART message with formatted sensor data.
 * @param bme280_data: The sensor data to format.
 * @param message: Buffer to store the formatted message.
 * @param tag: The tag to prepend to the data (e.g., "Temperature: ").
 * @param unit: The unit to append to the data (e.g., "C" or "%").
 * @retval None
 */
void APP_uartPrepareData(float bme280_data, char *message, const char *tag, const char *unit)
{
    int intPart = (int)bme280_data;
    int fracPart = (int)((bme280_data - intPart) * FRACTIONAL_MULTIPLIER);

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

/**
 * @brief Prepares the temperature and humidity data for UART transmission.
 * @param message_tem: Buffer to store the formatted temperature data.
 * @param message_hum: Buffer to store the formatted humidity data.
 * @retval None
 */
void APP_uartPrepareSensorTempHum(char *message_tem, char *message_hum)
{
    APP_uartPrepareData((float)bme280_temperature, message_tem, "Temperature: ", "C");
    APP_uartPrepareData((float)bme280_humidity, message_hum, "Humidity: ", "%");
}

/**
 * @brief Sends the temperature and humidity data over UART.
 * @param message_1: The formatted temperature message to send.
 * @param message_2: The formatted humidity message to send.
 * @retval None
 */
void APP_uartDisplaySensorData(char *message_1, char *message_2)
{
    uartSendString((uint8_t *)message_1);
    uartSendString((uint8_t *)message_2);
}

/**
 * @brief Prepares the temperature and humidity data for LCD display.
 * @retval None
 */
void APP_lcdPrepareSensorData(void)
{
    itoa((int)bme280_temperature, lcdTempStr, DECIMAL);
    strcat(lcdTempStr, ".");
    itoa((int)((bme280_temperature - (int)bme280_temperature) * FRACTIONAL_MULTIPLIER), lcdTempStr + strlen(lcdTempStr), DECIMAL);

    itoa((int)bme280_humidity, lcdHumStr, DECIMAL);
    strcat(lcdHumStr, ".");
    itoa((int)((bme280_humidity - (int)bme280_humidity) * FRACTIONAL_MULTIPLIER), lcdHumStr + strlen(lcdHumStr), DECIMAL);
}

/**
 * @brief Displays the prepared temperature and humidity data on the LCD.
 * @retval None
 */
void APP_lcdDisplaySensorData(void)
{
    API_LCD_DisplayTwoMsgs(APP_HUM_LCD_CURSOR_POS, APP_LCD_LINE_1, (uint8_t *)"H:", (uint8_t *)lcdHumStr);
    API_LCD_DisplayTwoMsgs(APP_TEMP_LCD_CURSOR_POS, APP_LCD_LINE_2, (uint8_t *)"T:", (uint8_t *)lcdTempStr);
}

/**
 * @brief Displays an alarm message on the LCD in the event of a temperature alarm.
 * @retval None
 */
void APP_lcdAlarm(void)
{
    API_LCD_DisplayMsg(APP_ALARM_LCD_CURSOR_POS, APP_LCD_LINE_2, (uint8_t *)"ALARMA! ");
}

/**
 * @brief Displays the current time on the LCD.
 * @retval None
 */
void APP_lcdDisplayClock(void)
{
    API_LCD_SetCursorLine(APP_CLOCK_CURSOR_POS, APP_LCD_LINE_1);
    API_LCD_SendBCDData(sTime.Hours);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Minutes);
    API_LCD_SendData(':');
    API_LCD_SendBCDData(sTime.Seconds);
}

/**
 * @brief Displays the current date on the LCD.
 * @retval None
 */
void APP_lcdDisplayDate(void)
{
    API_LCD_SetCursorLine(APP_CLOCK_CURSOR_POS, APP_LCD_LINE_2);
    API_LCD_SendBCDData(sDate.Date);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Month);
    API_LCD_SendData('/');
    API_LCD_SendBCDData(sDate.Year);
}

/**
 * @brief Updates the FSM state and triggers the appropriate actions and displays based on the current state.
 * @retval None
 */
void APP_FSM_update(void)
{
    memset(messageFsm, ZEROVAL, sizeof(messageFsm)); // Clear the FSM message buffer

    switch (currentTempState)
    {
    case TEMP_NORMAL:
        if (bme280_temperature > THRESHOLD_TEMP) // Transition to ALARM state
        {
            currentTempState = TEMP_ALARM;

            strcpy(messageFsm, "Temperature Alarm State.\r\n");
            uartSendString((uint8_t *)messageFsm);
            APP_lcdAlarm();
        }
        else // Remain in NORMAL state
        {
            strcpy(messageFsm, "Temperature Normal State.\r\n");
            uartSendString((uint8_t *)messageFsm);
            APP_lcdDisplayDate();
        }
        break;

    case TEMP_ALARM:
        if (bme280_temperature <= THRESHOLD_TEMP) // Transition to NORMAL state
        {
            currentTempState = TEMP_NORMAL;

            strcpy(messageFsm, "Temperature Normal State.\r\n");
            uartSendString((uint8_t *)messageFsm);
            APP_lcdDisplayDate();
        }
        else // Remain in ALARM state
        {
            strcpy(messageFsm, "Temperature Alarm State.\r\n");
            uartSendString((uint8_t *)messageFsm);
            APP_lcdAlarm();
        }
        break;

    default:
    	APP_FsmErrorHandler();
        break;
    }
}

/**
 * @brief Updates the current time and displays it on the LCD.
 * @retval None
 */
void APP_lcdUpdateTime(void)
{
    ClockUpdateTimeDate();
    APP_lcdDisplayClock();
}

/**
 * @brief Updates the sensor data by reading from the BME280 sensor.
 * @retval None
 */
void APP_updateSensorData(void)
{
    API_BME280_ReadAndProcess();
}

/**
 * @brief Prepares and displays the sensor data on the LCD.
 * @retval None
 */
void APP_prepareAndDisplaySensorData(void)
{
    APP_lcdPrepareSensorData();
    APP_lcdDisplaySensorData();
}

/**
 * @brief Prepares and sends the sensor data over UART.
 * @retval None
 */
void APP_prepareAndSendUARTData(void)
{
    memset(message_tem, ZEROVAL, sizeof(message_tem)); // I clean the message buffer before populating it again with the data prepare function that's next.
    memset(message_hum, ZEROVAL, sizeof(message_hum));

    APP_uartPrepareSensorTempHum(message_tem, message_hum);
    APP_uartDisplaySensorData(message_tem, message_hum);
}

/**
 * @brief Handles invalid case in APP FSM.
 */
static void APP_FsmErrorHandler()
{
    while (1)
    {
    }
}

/* Public Function Definitions ----------------------------------------------- */

/**
 * @brief Initializes all necessary components for the application, including the clock, FSM, BME280 sensor, UART, and LCD.
 * @retval None
 */
void APP_init(void)
{
    ClockInit();
    APP_FSM_init();
    API_BME280_Init();
    uartInit();
    API_LCD_Initialize();
}

/**
 * @brief Main update function that handles sensor data acquisition, LCD updates, sensor data display, and FSM updates.
 * @retval None
 */
void APP_update(void)
{
    APP_updateSensorData();
    APP_lcdUpdateTime();
    APP_prepareAndDisplaySensorData();
    APP_prepareAndSendUARTData();
    APP_FSM_update();
}
