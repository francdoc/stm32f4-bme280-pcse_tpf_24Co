#ifndef API_SRC_API_CLOCK_DATE_C_
#define API_SRC_API_CLOCK_DATE_C_

#include "API_clock_date.h"

// Global variables for storing time and date settings
RTC_TimeTypeDef sTime = {0}; // Structure to store time values
RTC_DateTypeDef sDate = {0}; // Structure to store date values

/**
 * @brief  Error handler for the clock and date functions.
 *         This function enters an infinite loop, indicating a critical error.
 * @param  None
 * @retval None
 */
void Clock_Error_Handler(void)
{
    while (1)
    {
        // Stay in an infinite loop to indicate an error
    }
}

/**
 * @brief  Initializes the RTC clock with a predefined time and date.
 * @param  None
 * @retval None
 */
void ClockInit(void)
{
    // Set initial time values (in BCD format)
    sTime.Hours = 0x01;
    sTime.Minutes = 0x20;
    sTime.Seconds = 0x00;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    // Set the RTC time, check for errors
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Clock_Error_Handler();
    }

    // Set initial date values (in BCD format)
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_AUGUST;
    sDate.Date = 0x05;
    sDate.Year = 0x24;

    // Set the RTC date, check for errors
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Clock_Error_Handler();
    }
}

/**
 * @brief  Updates the global time and date variables with the current RTC time and date.
 * @param  None
 * @retval None
 */
void ClockUpdateTimeDate(void)
{
    // Get the current time from the RTC
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

    // Get the current date from the RTC
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

#endif /* API_SRC_API_CLOCK_DATE_C_ */
