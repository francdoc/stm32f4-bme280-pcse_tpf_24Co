#ifndef API_INC_API_CLOCK_DATE_H_
#define API_INC_API_CLOCK_DATE_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

// Global variables for storing RTC time and date settings
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;
extern RTC_HandleTypeDef hrtc;

/* Exported functions ------------------------------------------------------- */
void ClockInit(void);
void ClockUpdateTimeDate(void);
void Clock_Error_Handler(void);

#endif /* API_INC_API_CLOCK_DATE_H_ */
