#include "API_bme280.h"

// Declare global variables for temperature and humidity,
// we will use them later in the finite-state machine app code.
float temp, hum;

// Calibration variables
uint16_t dig_T1;
int16_t dig_T2, dig_T3;

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4, dig_H5;
int8_t dig_H6;

static BME280_S32_t tADC, hADC;

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void BME280_Error_Handler(void)
{
    while (1)
    {
    }
}

static void SPI_Write(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg & WRITE_CMD_BIT; // Apply the write command mask.
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

static void SPI_Read(uint8_t reg, uint8_t *data, uint16_t size)
{
    uint8_t regAddress = reg | READ_CMD_BIT; // Apply the read command mask.
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateLow);
    HAL_SPI_Transmit(&hspi1, &regAddress, sizeof(regAddress), TIMEOUT);
    HAL_SPI_Receive(&hspi1, data, size, TIMEOUT);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, PinStateHigh);
}

// Define the memory addresses for calibration data in the BME280 sensor
#define BME280_CALIB_00_ADDR 0x88 // Starting address for the first block of calibration data (temperature and pressure)
#define BME280_CALIB_26_ADDR 0xE1 // Starting address for the second block of calibration data (humidity)

// Define the sizes of the calibration data blocks
#define BME280_CALIBDATA1_SIZE 26 // Size of the first block of calibration data (temperature and pressure)
#define BME280_CALIBDATA2_SIZE 7  // Size of the second block of calibration data (humidity)

// Define indices for calibration data bytes
#define DIG_T1_LSB_INDEX  0
#define DIG_T1_MSB_INDEX  1
#define DIG_T2_LSB_INDEX  2
#define DIG_T2_MSB_INDEX  3
#define DIG_T3_LSB_INDEX  4
#define DIG_T3_MSB_INDEX  5

#define DIG_H1_INDEX      24
#define DIG_H2_LSB_INDEX  0
#define DIG_H2_MSB_INDEX  1
#define DIG_H3_INDEX      2
#define DIG_H4_MSB_INDEX  3
#define DIG_H4_LSB_INDEX  4
#define DIG_H5_MSB_INDEX  4
#define DIG_H5_LSB_INDEX  5
#define DIG_H6_INDEX      6

// Function to combine two bytes into a 16-bit integer
static uint16_t combineBytes(uint8_t msb, uint8_t lsb) {
    return ((uint16_t)msb << 8) | lsb;
}

// Function to extract specific bits from a byte value
static uint8_t extractBits(uint8_t value, uint8_t mask, uint8_t shift) {
    return (value & mask) >> shift;
}

// Function to read the calibration parameters from the BME280 sensor
static void trimmingParametersRead(void) {
    uint8_t calibData1[BME280_CALIBDATA1_SIZE]; // Array to store the first block of calibration data
    uint8_t calibData2[BME280_CALIBDATA2_SIZE]; // Array to store the second block of calibration data

    // Read the first block of calibration data from the sensor
    SPI_Read(BME280_CALIB_00_ADDR, calibData1, BME280_CALIBDATA1_SIZE);

    // Read the second block of calibration data from the sensor
    SPI_Read(BME280_CALIB_26_ADDR, calibData2, BME280_CALIBDATA2_SIZE);

    // Combine the bytes read from the calibration memory into 16-bit integers for temperature and pressure
    dig_T1 = combineBytes(calibData1[DIG_T1_MSB_INDEX], calibData1[DIG_T1_LSB_INDEX]);
    dig_T2 = combineBytes(calibData1[DIG_T2_MSB_INDEX], calibData1[DIG_T2_LSB_INDEX]);
    dig_T3 = combineBytes(calibData1[DIG_T3_MSB_INDEX], calibData1[DIG_T3_LSB_INDEX]);

    // Store the humidity calibration data directly from the corresponding bytes
    dig_H1 = calibData1[DIG_H1_INDEX];

    dig_H2 = combineBytes(calibData2[DIG_H2_MSB_INDEX], calibData2[DIG_H2_LSB_INDEX]);
    dig_H3 = calibData2[DIG_H3_INDEX];

    // For dig_H4, special bit manipulations are required to combine parts of two different bytes
    dig_H4 = (calibData2[DIG_H4_MSB_INDEX] << 4) | extractBits(calibData2[DIG_H4_LSB_INDEX], 0x0F, 0);

    // For dig_H5, special bit manipulations are required to combine parts of two different bytes
    dig_H5 = (calibData2[DIG_H5_MSB_INDEX] << 4) | (calibData2[DIG_H5_LSB_INDEX] >> 4);

    // Store the final humidity calibration value directly from the corresponding byte
    dig_H6 = calibData2[DIG_H6_INDEX];
}

// Function to initialize the BME280 sensor
void BME280_init(void)
{
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

static BME280_S32_t t_fine;

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

uint8_t BME280_read(void)
{
    uint8_t sensorData[8];
    uint8_t chipID;

    SPI_Read(CHIPIDREG, &chipID, MEMADDRESSSIZE);

    if (chipID == 0x60)
    {
#ifdef DEBUG_BME280
        // blocking delays affect clock display performance negatively (time-lcd lag)
        for (int i = 0; i <= NumBlinks; i++)
        {
            BSP_LED_Toggle(LED2); // blink indicates sensor ID rx is OK
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

void BME280_calculate(void)
{
    temp = ((float)BME280_compensate_T_int32(tADC)) / 100.0;
    hum = ((float)bme280_compensate_H_int32(hADC)) / 1024.0;
}

