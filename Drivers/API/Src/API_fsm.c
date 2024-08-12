#include "API_fsm.h"

static tempState_t currentTempState;

void tempFSM_init()
{
    currentTempState = TEMP_NORMAL;
}

void FSM_update(void);

void eval_data()
{
    uint8_t message[50];
    if (bme280_temperature> THRESHOLD_TEMP)
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

char tempStr[20];
char humStr[20];

void prepare_sensor_data_for_uart(uint8_t *message_1, uint8_t *message_2)
{
    int intPart = (int)bme280_temperature;
    int fracPart = (int)((bme280_temperature- intPart) * 100);
    strcpy((char *)message_1, "Temperature: ");
    itoa(intPart, tempStr, 10);
    strcat((char *)message_1, tempStr);
    strcat((char *)message_1, ".");
    itoa(fracPart, tempStr, 10);
    strcat((char *)message_1, tempStr);
    strcat((char *)message_1, " C\r\n");

    strcpy((char *)message_2, "Humidity: ");
    intPart = (int)bme280_humidity;
    fracPart = (int)((bme280_humidity- intPart) * 100);
    itoa(intPart, humStr, 10);
    strcat((char *)message_2, humStr);
    strcat((char *)message_2, ".");
    itoa(fracPart, humStr, 10);
    strcat((char *)message_2, humStr);
    strcat((char *)message_2, " %\r\n");
}

void uart_display_data(uint8_t *message_1, uint8_t *message_2)
{
    uartSendString(message_1);
    uartSendString(message_2);
}

char lcdTempStr[20];
char lcdHumStr[20];

void prepare_sensor_data_for_lcd(void)
{
    // Prepare temperature string for LCD
    itoa((int)bme280_temperature, lcdTempStr, 10);
    strcat(lcdTempStr, ".");
    itoa((int)((bme280_temperature - (int)bme280_temperature) * 100), lcdTempStr + strlen(lcdTempStr), 10);

    // Prepare humidity string for LCD
    itoa((int)bme280_humidity, lcdHumStr, 10);
    strcat(lcdHumStr, ".");
    itoa((int)((bme280_humidity - (int)bme280_humidity) * 100), lcdHumStr + strlen(lcdHumStr), 10);
}

void lcd_display_sensor_data(void)
{
    // Display temperature on the LCD
    PosCaracLLcd(9); // Assuming position 0 on the upper line
    SacaTextoLcd((uint8_t *)"T:");
    SacaTextoLcd((uint8_t *)lcdTempStr);

    // Display humidity on the LCD
    PosCaracHLcd(9); // Assuming position 0 on the lower line
    SacaTextoLcd((uint8_t *)"H:");
    SacaTextoLcd((uint8_t *)lcdHumStr);
}

void lcd_alarm()
{
    PosCaracLLcd(0);
    SacaTextoLcd((uint8_t *)"ALARMA! ");
}

void lcd_display_clock()
{
    PosCaracHLcd(0);
    DatoBCD(sTime.Hours);
    DatoLcd(':');
    DatoBCD(sTime.Minutes);
    DatoLcd(':');
    DatoBCD(sTime.Seconds);
}

void lcd_display_date()
{
    PosCaracLLcd(0);
    DatoBCD(sDate.Date);
    DatoLcd('/');
    DatoBCD(sDate.Month);
    DatoLcd('/');
    DatoBCD(sDate.Year);
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
	clock_init_code();
	tempFSM_init();
	API_BME280_Init();
    uartInit();
    Init_Lcd();
}

void APP_updateLCD(void)
{
    update_lcd_clock();
    lcd_display_clock();
}

void APP_updateSensorData(void)
{
	API_BME280_Read();
}

void APP_prepareAndDisplaySensorData(void)
{
    prepare_sensor_data_for_lcd();
    lcd_display_sensor_data();
}

void APP_prepareAndSendUARTData(void)
{
    uint8_t message_tem[50];
    uint8_t message_hum[50];
    prepare_sensor_data_for_uart(message_tem, message_hum);
    uart_display_data(message_tem, message_hum);
}

void APP_update()
{
	APP_updateSensorData();
    APP_updateLCD();
    APP_prepareAndDisplaySensorData();
    APP_prepareAndSendUARTData();
    FSM_update();
}
