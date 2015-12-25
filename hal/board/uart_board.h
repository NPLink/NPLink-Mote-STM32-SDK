/*
(C)2015 NPLink

Description: Uart driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project


*/
/**************************************************
 * @fn		UART_Init
 *
 * @brief 	initialize Uart
 *
 * @return	void
 */ 
#ifndef __UART_BOARD_H__
#define __UART_BOARD_H__

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"

#define USARTx                            USART1
#define USARTx_CLK_ENABLE()              __USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_6
#define USARTx_TX_GPIO_PORT              GPIOB  
#define USARTx_TX_AF                     GPIO_AF0_USART1
#define USARTx_RX_PIN                    GPIO_PIN_7
#define USARTx_RX_GPIO_PORT              GPIOB 
#define USARTx_RX_AF                     GPIO_AF0_USART1

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* Exported functions ------------------------------------------------------- */

void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);
void UART_Init(void);


#endif //__UART_BOARD_H__
