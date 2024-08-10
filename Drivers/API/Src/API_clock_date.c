#ifndef API_SRC_API_CLOCK_DATE_C_
#define API_SRC_API_CLOCK_DATE_C_

#include "API_clock_date.h"

RTC_TimeTypeDef sTime = {0}; // key user variables for RTC date
RTC_DateTypeDef sDate = {0};

void RTC_Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

void clock_init_code(void)
{
    sTime.Hours = 0x01;
    sTime.Minutes = 0x20;
    sTime.Seconds = 0x00;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
    	RTC_Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_AUGUST;
    sDate.Date = 0x05;
    sDate.Year = 0x24;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
    	RTC_Error_Handler();
    }
}

void update_lcd_clock(void)
{
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

#endif /* API_SRC_API_CLOCK_DATE_C_ */
