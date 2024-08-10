#include "API_lcd_port.h"
#include "API_bme280.h"
#include "API_lcd.h"

#include <stdlib.h>
#include <math.h>

RTC_TimeTypeDef sTime = {0}; // key user variables for RTC date
RTC_DateTypeDef sDate = {0};

void update_lcd_clock(void)
{
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void clock_init_code(void)
{
    /*
     * * Initialize RTC and set the Time and Date
     */

    sTime.Hours = 0x01;
    sTime.Minutes = 0x20;
    sTime.Seconds = 0x00;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_AUGUST;
    sDate.Date = 0x05;
    sDate.Year = 0x24;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void BME280_Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

static void SPI_Write(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg & WRITE_CMD_BIT; // Apply the write command mask
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

static void SPI_Read(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg | READ_CMD_BIT; // Apply the read command mask
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), TIMEOUT);
    HAL_SPI_Receive(&hspi1, data, size, TIMEOUT);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

static tempState_t currentTempState;

void tempFSM_init()
{
    currentTempState = TEMP_NORMAL;
}

void FSM_update(void);

/*
5.4.2 Register 0xE0 “reset”
The “reset” register contains the soft reset word reset[7:0]. If the value 0xB6 is written to the register,
the device is reset using the complete power-on-reset procedure. Writing other values than 0xB6 has
no effect. The readout value is always 0x00.*/
#define RESET_REG 0xE0

// HACER STRUCTURA CON PUNTEROS! EVALUAR!
static uint16_t dig_T1, dig_P1, dig_H1, dig_H3;
static int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9, dig_H2, dig_H4, dig_H5, dig_H6;
static int32_t tADC, hADC; // global

typedef int32_t BME280_S32_t;  // global type
typedef uint32_t BME280_U32_t; // global type

static float temp, hum;

// 4.2.2 Trimming parameter readout
// UNDERSTAND AND ADAPT TO OWN IT
static void trimmingParametersRead(void)
{
    uint8_t calibData1[26]; // Table 18: Memory map -> calib00..calib25 | 0x88 to 0xA1
    uint8_t calibData2[7];

    SPI_Read(CALIBMEMADD1, calibData1, CALIBDATASIZE1); // 8-bit temperature calibration value
    SPI_Read(CALIBMEMADD2, calibData2, CALIBDATASIZE2); // 8-bit humidity calibration value

    // Combine the bytes read from the calibration memory into 16-bit integers.
    dig_T1 = (calibData1[1] << 8) | calibData1[0];
    dig_T2 = (calibData1[3] << 8) | calibData1[2];
    dig_T3 = (calibData1[5] << 8) | calibData1[4];

    dig_P1 = (calibData1[7] << 8) | calibData1[6];
    dig_P2 = (calibData1[9] << 8) | calibData1[8];
    dig_P3 = (calibData1[11] << 8) | calibData1[10];
    dig_P4 = (calibData1[13] << 8) | calibData1[12];
    dig_P5 = (calibData1[15] << 8) | calibData1[14];
    dig_P6 = (calibData1[17] << 8) | calibData1[16];
    dig_P7 = (calibData1[19] << 8) | calibData1[18];
    dig_P8 = (calibData1[21] << 8) | calibData1[20];
    dig_P9 = (calibData1[23] << 8) | calibData1[22];

    dig_H1 = calibData1[24];
    dig_H2 = (calibData2[1] << 8) | calibData2[0];
    dig_H3 = calibData2[2];
    dig_H4 = (calibData2[3] << 4) | (calibData2[4] & 0x0F);
    dig_H5 = (calibData2[4] << 4) | (calibData2[5] >> 4);
    dig_H6 = calibData2[6];
}

// Function to initialize the BME280 sensor
void BME280_init(void)
{
    clock_init_code();

    tempFSM_init();

    // Read trimming parameters from the sensor
    trimmingParametersRead();

    /*
    5.4.2 The "reset" register contains the soft reset word reset[7:0].
    If the value 0xB6 is written to the register, the device is reset using the complete power-on-reset procedure.
    The readout value is 0x00.
    */
    uint8_t resetSeq = 0xB6;
    uint8_t ctrlHum = 0x01;
    uint8_t ctrlMeas = 0xA3; // 0b10100011 in hexadecimal
    uint8_t config = 0x10;   // 0b00010000 in hexadecimal

    // Write reset sequence to the reset register
    SPI_Write(RESET_REG, &resetSeq, CMDWRITESIZE);
    HAL_Delay(BME_HAL_DELAY);

    // Write control settings to the control registers
    SPI_Write(CTRL_HUM, &ctrlHum, CMDWRITESIZE);
    HAL_Delay(BME_HAL_DELAY);

    SPI_Write(CTRL_MEAS, &ctrlMeas, CMDWRITESIZE);
    HAL_Delay(BME_HAL_DELAY);

    SPI_Write(CONFIG_REG, &config, CMDWRITESIZE);
    HAL_Delay(BME_HAL_DELAY);
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
static BME280_S32_t t_fine; // t_fine carries fine temperature as global value

// Temperature compensation formula taken from datasheet (please check page 25/60 for reference).
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
    BME280_S32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((BME280_S32_t)dig_T1 << 1))) * ((BME280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)dig_T1))) >> 12) * ((BME280_S32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Humidity compensation formula taken from datasheet (please check page 25/60 for reference).
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH.
static BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H)
{
    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (BME280_U32_t)(v_x1_u32r >> 12);
}

float BME280_getTemp(void)
{
    return temp;
}

float BME280_getHum(void)
{
    return hum;
}

static uint8_t BME280_read(void)
{
    uint8_t sensorData[8];
    uint8_t chipID;

    SPI_Read(CHIPIDREG, &chipID, MEMADDRESSSIZE);

    if (chipID == 0x60)
    {
#ifdef DEBUG_BME280
        // blocking delays affect clock display performance negatively (time-lcd lag)
        for (int i = 0; i <= 2; i++)
        {
            BSP_LED_Toggle(LED2); // sensor ID OK
            HAL_Delay(100);
        }
#endif
        SPI_Read(RAWDATAREG1, sensorData, RAWDATASIZE);

        /* Data readout is done by starting a burst read from 0xF7 to 0xFC (temperature and pressure) or from 0xF7 to 0xFE
         * (temperature, pressure and humidity). The data are read out in an unsigned 20-bit format both for pressure and
         * for temperature and in an unsigned 16-bit format for humidity. */

        tADC = (sensorData[3] << 12) | (sensorData[4] << 4) | (sensorData[5] >> 4); // WE CAN RETURN THIS WITH * & in the function signature
        hADC = (sensorData[6] << 8) | sensorData[7];

        return 0; // check state of the function return and pass the data with pointer * & for the return of tADC hADC
    }
    else
    {
        for (int i = 0; i <= 2; i++)
        {
            BSP_LED_Toggle(LED3); // sensor ID ERROR
            HAL_Delay(100);
        }

        return 1;
    }
}

void eval_data()
{
    uint8_t message[50];
    if (temp > THRESHOLD_TEMP)
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

void BME280_calculate(void)
{
    temp = ((float)BME280_compensate_T_int32(tADC)) / 100.0; // from integer to float
    hum = ((float)bme280_compensate_H_int32(hADC)) / 1024.0; // WATCHOUT FOR IMPLICIT TYPECASTS!!!!!!!!!
}

char tempStr[20];
char humStr[20];

void prepare_sensor_data_for_uart(uint8_t *message_1, uint8_t *message_2)
{
    int intPart = (int)temp;
    int fracPart = (int)((temp - intPart) * 100);
    strcpy((char *)message_1, "Temperature: ");
    itoa(intPart, tempStr, 10);
    strcat((char *)message_1, tempStr);
    strcat((char *)message_1, ".");
    itoa(fracPart, tempStr, 10);
    strcat((char *)message_1, tempStr);
    strcat((char *)message_1, " C\r\n");

    strcpy((char *)message_2, "Humidity: ");
    intPart = (int)hum;
    fracPart = (int)((hum - intPart) * 100);
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
    itoa((int)temp, lcdTempStr, 10);
    strcat(lcdTempStr, ".");
    itoa((int)((temp - (int)temp) * 100), lcdTempStr + strlen(lcdTempStr), 10);

    // Prepare humidity string for LCD
    itoa((int)hum, lcdHumStr, 10);
    strcat(lcdHumStr, ".");
    itoa((int)((hum - (int)hum) * 100), lcdHumStr + strlen(lcdHumStr), 10);
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
    uartInit();
    BME280_init();
    Init_Lcd();
}

void APP_update()
{
    update_lcd_clock();
    lcd_display_clock();

    BME280_read();

    FSM_update();

    BME280_calculate();

    uint8_t message_tem[50];
    uint8_t message_hum[50];
    prepare_sensor_data_for_uart(message_tem, message_hum);
    uart_display_data(message_tem, message_hum);

    prepare_sensor_data_for_lcd();
    lcd_display_sensor_data();
}
