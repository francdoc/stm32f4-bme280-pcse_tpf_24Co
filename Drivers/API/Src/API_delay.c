#include "API_delay.h"

/**
 * @brief  Initializes delay data structure
 * @param  delay is a pointer to the delay structure
 * @param  duration: duration of delay in milliseconds
 * @retval None
 */
void delayInit(delay_t *delay, tick_t duration)
{
	if (delay != NULL)
	{
		delay->startTime = 0;
		delay->running = false;
		delay->duration = duration;
	}
}

/**
 * @brief  Checks if specified delay duration is done
 * @param  delay is a pointer to the delay structure
 * @retval returns true if the duration has elapsed, if not returns false
 */
bool_t delayRead(delay_t *delay)
{
	if (delay != NULL)
	{
		if (!delay->running)
		{
			delay->startTime = HAL_GetTick();
			delay->running = true;
			return false;
		}
		if (delay->running)
		{
			tick_t now = HAL_GetTick();
			tick_t timeElapsed = now - delay->startTime;
			if ((timeElapsed >= delay->duration))
			{
				delay->running = false;
				return true;
			}
			return false;
		}
	}
	return false; // fallback
}

/**
 * @brief  This function checks if delay is still running
 * @param  delay is a pointer to the delay structure
 * @retval Returns a copy of the field value running from delay_t structure
 */
bool_t delayIsRunning(delay_t *delay)
{
	if (delay != NULL)
	{
		return delay->running;
	}
	return false;
}

/**
 * @brief  Configures a new delay
 * @param  delay is a pointer to the delay structure
 * @param  duration: duration of delay in milliseconds
 * @retval None
 */
void delayWrite(delay_t *delay, tick_t duration)
{
	if (delay != NULL)
	{
		delay->duration = duration;
	}
}
