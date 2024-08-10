/*
 * API_clock_date.h
 *
 *  Created on: Aug 10, 2024
 *      Author: franco
 */

#ifndef API_INC_API_CLOCK_DATE_H_
#define API_INC_API_CLOCK_DATE_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h" /* <- BSP include */

extern RTC_TimeTypeDef sTime; // key user variables for RTC date
extern RTC_DateTypeDef sDate;

extern RTC_HandleTypeDef hrtc;

extern void update_lcd_clock(void);
extern void clock_init_code(void);

#endif /* API_INC_API_CLOCK_DATE_H_ */
