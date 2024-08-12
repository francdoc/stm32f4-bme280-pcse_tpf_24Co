#include "API_bme280.h"

/* Global public variables ----------------------------------------------------------*/

// Declare global variables for temperature and humidity, we will use them later in the finite-state machine app code.
float bme280_temperature;
float bme280_humidity;

/* Private variables ----------------------------------------------------------*/

// Calibration variables
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint8_t dig_H1;
static int16_t dig_H2;
static uint8_t dig_H3;
static int16_t dig_H4, dig_H5;
static int8_t dig_H6;

// Type definitions for signed and unsigned 32-bit integers used in compensation calculations
typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;

static BME280_S32_t temp_adc, hum_adc;
static BME280_S32_t t_fine;

/* Private Function Prototypes ---------------------------------------------- */
static uint16_t combineBytes(uint8_t msb, uint8_t lsb);
static uint8_t extractBits(uint8_t value, uint8_t mask, uint8_t shift);
static void errorLedSignal(void);
static void okLedSignal(void);
static void calibrationParams(void);
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);
static BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H);

/* Private Function Definitions --------------------------------------------- */

/* WARNING: If you try to pass a 64-bit variable (such as uint64_t) to the combineBytes  or extractBits functions,
 * C will implicitly cast the 64-bit value to an 8-bit value (uint8_t) because the function parameters are explicitly
 * defined as uint8_t. This casting truncates the higher bits, leading to unexpected results if the higher bits of 64-bit
 * value contain critical data for the system.
 * To avoid this potential problem, I added control input logic in combineBytes, extractBits, BME280_compensate_T_int32 and
 * BME280_compensate_H_int32 functions.*/

/**
 * @brief  Combines two bytes into a 16-bit integer.
 * @param  uint8_t msb: Most significant byte.
 * @param  uint8_t lsb: Least significant byte.
 * @retval uint16_t: Combined 16-bit integer.
 */
static uint16_t combineBytes(uint8_t msb, uint8_t lsb)
{
  // Safety check to ensure that inputs are 8 bits wide and are not being potentially truncated if they were bigger than uint8_t. .
  if (msb > 0xFF || lsb > 0xFF)
  {
    API_BME280_ErrorHandler();
  }
  return ((uint16_t)msb << 8) | lsb;
}

/**
 * @brief  Extracts specific bits from a byte value.
 * @param  uint8_t value: Byte value to extract bits from.
 * @param  uint8_t mask: Mask to apply for bit extraction.
 * @param  uint8_t shift: Number of bits to shift after masking.
 * @retval uint8_t: Extracted bits as a byte.
 */
static uint8_t extractBits(uint8_t value, uint8_t mask, uint8_t shift)
{
  // Safety check to ensure that inputs are 8 bits wide and are not being potentially truncated if they were bigger than uint8_t. .
  if (value > 0xFF || mask > 0xFF || shift > 7)
  {
    API_BME280_ErrorHandler();
  }
  return (value & mask) >> shift;
}

/**
 * @brief  Flashes LED to signal an error in BME280 operations.
 * @param  None
 * @retval None
 */
static void errorLedSignal(void)
{
  for (int i = 0; i <= NumErrorRxBlinks; i++)
  {
    BME280_HAL_Blink(LED3); // Sensor error.
    BME280_HAL_Delay(BME280_HAL_DELAY);
  }
}

/**
 * @brief  Flashes LED to signal a successful sensor data reception.
 * @param  None
 * @retval None
 */
static void okLedSignal(void)
{
  for (int i = 0; i <= NumOkRxBlinks; i++)
  {
    BME280_HAL_Blink(LED2); // Blink indicates sensor ID rx is OK.
    BME280_HAL_Delay(BME280_HAL_DELAY);
  }
}

/**
 * @brief  Reads the calibration parameters from the BME280 sensor (4.2.2 Trimming parameter readout).
 *         Each compensation word is a 16-bit signed or unsigned integer value stored in two’s complement.
 * @param  None
 * @retval None
 */
static void calibrationParams(void)
{
  uint8_t calibDataBuffer1[BME280_CALIBDATA_BLOCK1_SIZE];
  uint8_t calibDataBuffer2[BME280_CALIBDATA_BLOCK2_SIZE];

  /* Read the first block of calibration data from the sensor, storing the data read from memory addresses 0x88 to 0xA1.
   * This block contains the calibration values for temperature and pressure, covering a 26-byte range.
   * Although the pressure values won't be used, they will be extracted anyways.*/
  BME280_HAL_SPI_Read(BME280_CALIB_00_ADDR, calibDataBuffer1, BME280_CALIBDATA_BLOCK1_SIZE);

  /* Read the second block of calibration data from the sensor, storing the data read from memory addresses 0x88 to 0xA1.
   * This block contains the calibration values for humidity, covering a 7-byte range.*/
  BME280_HAL_SPI_Read(BME280_CALIB_26_ADDR, calibDataBuffer2, BME280_CALIBDATA_BLOCK2_SIZE);

  // The next operations rely heavily on datasheet table 16: Compensation parameter storage, naming and data type.

  // Combine the bytes read from the calibration memory into 16-bit integers for temperature
  dig_T1 = combineBytes(calibDataBuffer1[DIG_T1_MSB_INDEX], calibDataBuffer1[DIG_T1_LSB_INDEX]);
  dig_T2 = combineBytes(calibDataBuffer1[DIG_T2_MSB_INDEX], calibDataBuffer1[DIG_T2_LSB_INDEX]);
  dig_T3 = combineBytes(calibDataBuffer1[DIG_T3_MSB_INDEX], calibDataBuffer1[DIG_T3_LSB_INDEX]);

  // Extract data for first trimming humidity value (dig_H1)
  dig_H1 = calibDataBuffer1[DIG_H1_INDEX];

  dig_H2 = combineBytes(calibDataBuffer2[DIG_H2_MSB_INDEX], calibDataBuffer2[DIG_H2_LSB_INDEX]);
  dig_H3 = calibDataBuffer2[DIG_H3_INDEX];

  // For dig_H4, special bit manipulations are required to combine parts of two different bytes
  dig_H4 = (calibDataBuffer2[DIG_H4_MSB_INDEX] << 4) | extractBits(calibDataBuffer2[DIG_H4_LSB_INDEX], 0x0F, 0);

  // For dig_H5, special bit manipulations are required to combine parts of two different bytes
  dig_H5 = (calibDataBuffer2[DIG_H5_MSB_INDEX] << 4) | (calibDataBuffer2[DIG_H5_LSB_INDEX] >> 4);

  // Store the final humidity calibration value directly from the corresponding byte
  dig_H6 = calibDataBuffer2[DIG_H6_INDEX];
}

/**
 * @brief  Temperature compensation formula & function taken from datasheet (please check page 25/60 for reference).
 *         Added control input to avoid possible misuse of wider types as input.
 *         Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
 *         t_fine carries fine temperature as global value for BME280_compensate_H_int32 function to process its return humidity value.
 * @param  BME280_S32_t adc_T: Raw ADC temperature value.
 * @retval BME280_S32_t: Compensated temperature value.
 */
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
  // Ensure that adc_T is within the valid 20-bit range (since temperature is typically represented by a 20-bit value).
  if (adc_T < 0 || adc_T > 0xFFFFF) // 20-bit range check
  {
    API_BME280_ErrorHandler();
  }

  BME280_S32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((BME280_S32_t)dig_T1 << 1))) * ((BME280_S32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((BME280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)dig_T1))) >> 12) * ((BME280_S32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

/**
 * @brief  Humidity compensation formula & function taken from datasheet (please check page 25/60 for reference).
 * 		   Added control input to avoid possible misuse of wider types as input.
 *         Returns humidity in %RH as unsigned 32-bit integer in Q22.10 format (22 integer and 10 fractional bits).
 *         For example, an output value of “47445” represents 47445/1024 = 46.333 %RH.
 * @param  BME280_S32_t adc_H: Raw ADC humidity value.
 * @retval BME280_U32_t: Compensated humidity value.
 */
static BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H)
{
  // Ensure that adc_H is within the valid 16-bit range.
  if (adc_H < 0 || adc_H > 0xFFFF)
  {
    API_BME280_ErrorHandler();
  }

  BME280_S32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (BME280_U32_t)(v_x1_u32r >> 12);
}

/* Public Function Definitions ----------------------------------------------- */

/**
 * @brief  Initializes the BME280 sensor by configuring its control registers and reading calibration parameters.
 * @param  None
 * @retval None
 */
void API_BME280_Init(void)
{
  calibrationParams();

  /* 5.4.2 The "reset" register contains the soft reset word reset[7:0].
  If the value 0xB6 is written to the register, the device is reset using the complete power-on-reset procedure.
  The readout value is 0x00.*/
  uint8_t CmdReset = 0xB6;

  /* 4.3. Register 0xF2 “ctrl_hum”. The “ctrl_hum” register sets the humidity data acquisition options of the device.
   * For this system I chose humidity at oversampling x 16.*/
  uint8_t CmdCtrlHum = 0x05;

  /* Bit-map according to 5.4.5 Register 0xF4 “ctrl_meas”.
   * bit-7, bit-6, bit-5, bit-4, bit-3, bit-2, bit-1, bit-0
   * 0b10100011
   * Temperature at oversampling x 16.
   * Pressure is not necessary since we will not use it in this system.
   * Mode is Normal.*/
  uint8_t CmdCtrlMeasr = 0xA3;

  /* Bit-map according to 5.4.6 Register 0xF5 “config”.
   * bit-7, bit-6, bit-5, bit-4, bit-3, bit-2, bit-1, bit-0
   * 0b00011000
   * For this system I chose ts_tandby [ms] = 0.5 ms (bits 7->5 = 000). Chose this configuration so we have the smallest time interval between measurements. Making the system more reactive to changes in temperature.
   * See in datasheet section 3.3.4 Normal mode (figure 5: Normal mode timing diagram).
   * For this system I chose a filter coefficient of 8 (bits 4->2 = 011. When the IIR filter is enabled, the temperature resolution is 20 bit (see section 3.4.3 for more info on temperature measurement).
   * For this system we disable 3-wire SPI interface when bit-0 set to ‘0’. Please check section 6.3 for more information on this.*/
  uint8_t CmdConfig = 0x18;

  // Write reset sequence to the reset register
  BME280_HAL_SPI_Write(BME280_RESET_REG, &CmdReset, CMD_WRITE_SIZE);
  BME280_HAL_Delay(BME280_HAL_DELAY);

  // Write control settings to the control registers
  BME280_HAL_SPI_Write(BME280_CTRL_HUM_REG, &CmdCtrlHum, CMD_WRITE_SIZE);
  BME280_HAL_Delay(BME280_HAL_DELAY);

  BME280_HAL_SPI_Write(BME280_CTRL_MEASR_REG, &CmdCtrlMeasr, CMD_WRITE_SIZE);
  BME280_HAL_Delay(BME280_HAL_DELAY);

  BME280_HAL_SPI_Write(BME280_CTRL_CONFIG_REG, &CmdConfig, CMD_WRITE_SIZE);
  BME280_HAL_Delay(BME280_HAL_DELAY);
}

/**
 * @brief  Reads raw temperature and humidity data from the BME280 sensor, applies compensation formulas, and converts the data to human readable units.
 * @param  None
 * @retval uint8_t: Returns 0 if the read operation is successful, 1 if an error occurs.
 */
uint8_t API_BME280_ReadAndProcess(void)
{
  uint8_t sensorDataBuffer[8];
  uint8_t chip_Id;

  BME280_HAL_SPI_Read(CHIP_ID_REG, &chip_Id, CHIP_ID_BLOCK_SIZE);

  if (chip_Id == BME280_CHIP_ID)
  {
#ifdef DEBUG_BME280
    // blocking delays affect clock display performance negatively (time-lcd lag)
    okLedSignal();
#endif
    /* Data readout is done by starting a burst read from 0xF7 to 0xFE (temperature, pressure and humidity).
     * The data are read out in an unsigned 20-bit format both for pressure and for temperature and in an
     * unsigned 16-bit format for humidity.
     *
     * The sensor output data is organized as follows:
     * - 0xF7 to 0xF9: Raw pressure data (20 bits) -> Section 5.4.7.
     * - 0xFA to 0xFC: Raw temperature data (20 bits) -> Section 5.4.8.
     * - 0xFD to 0xFE: Raw humidity data (16 bits) -> Section 5.4.9.
     *
     * This means that with 46 bits (8 bytes) we can hold all the sampled data in 1 burst read.
     * See Table 18: Memory map for more context.
     *
     * BYTE 7 | BYTE 6 | BYTE 5 | BYTE 4 | BYTE 3 | BYTE 2 | BYTE 1 | BYTE 0
     * H_LSB    H_MSB    T_XLSB   T_LSB    T_MSB    P_XLSB   P_LSB    P_MSB
     * */

    BME280_HAL_SPI_Read(PRESSURE_MSB_REG, sensorDataBuffer, RAW_OUTPUT_DATA_SIZE);

    // The BME280 output consists of the ADC output values that have to be compensated afterwards.

    // Combine the bytes to form the 20-bit temperature value (temp_adc).
    temp_adc = (sensorDataBuffer[TEMP_MSB_INDEX] << TEMP_MSB_SHIFT) |
               (sensorDataBuffer[TEMP_LSB_INDEX] << TEMP_LSB_SHIFT) |
               (sensorDataBuffer[TEMP_XLSB_INDEX] >> TEMP_XLSB_SHIFT);

    // Apply compensation formula to temperature ADC value.
    bme280_temperature = ((float)BME280_compensate_T_int32(temp_adc)) / TEMPERATURE_SCALE_FACTOR;

    // Combine the bytes to form the 16-bit humidity value (hum_adc).
    hum_adc = (sensorDataBuffer[HUM_MSB_INDEX] << HUM_MSB_SHIFT) |
              sensorDataBuffer[HUM_LSB_INDEX];

    // Apply compensation formula to humidity ADC value.
    bme280_humidity = ((float)BME280_compensate_H_int32(hum_adc)) / HUMIDITY_SCALE_FACTOR;

    return 0;
  }
  else
  {
    errorLedSignal();
    return 1;
  }
}

/**
 * @brief  This function is executed in case of error occurrence. Program will get stuck in this part of the code. Indicating major BME280 error.
 * @retval None
 */
void API_BME280_ErrorHandler(void)
{
  while (1)
  {
    errorLedSignal();
  }
}
