#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_hal.h" /* <- HAL include */

/* Exported types ------------------------------------------------------------*/
#ifndef BOOL_TYPE_DEFINITIOON_
#define BOOL_TYPE_DEFINITIOON_
typedef bool bool_t;
#endif // BOOL_TYPE_DEFINITIOON_

/* Exported constants --------------------------------------------------------*/
#define MAXbUFFER 256
#define TxTIMEOUT 5000
#define RxTIMEOUT 10
#define USARTx USART3
#define USARTx_CLK_ENABLE() __HAL_RCC_USART3_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define USARTx_FORCE_RESET() __HAL_RCC_USART3_FORCE_RESET()
#define USARTx_RELEASE_RESET() __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN GPIO_PIN_8
#define USARTx_TX_GPIO_PORT GPIOD
#define USARTx_TX_AF GPIO_AF7_USART3
#define USARTx_RX_PIN GPIO_PIN_9
#define USARTx_RX_GPIO_PORT GPIOD
#define USARTx_RX_AF GPIO_AF7_USART3

/* Exported functions ------------------------------------------------------- */
bool_t uartInit(void);
void uartSendString(uint8_t *pstring);
void uartSendStringSize(uint8_t *pstring, uint16_t size);
void uartReceiveStringSize(uint8_t *pstring, uint16_t size);
int uartReceiveStringAndParseDate(uint8_t *pstring, uint16_t size, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate);
#endif /* API_INC_API_UART_H_ */
