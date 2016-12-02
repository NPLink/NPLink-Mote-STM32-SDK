/*
(C)2015 NPLink

Description: Uart driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
/**************************************************
 * @fn		HAL_UART_Init
 *
 * @brief 	initialize Uart
 *
 * @return	void
 */ 
#ifndef __UART_BOARD_H__
#define __UART_BOARD_H__

#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_uart.h"
#include "fifo.h"

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

#define RECEIVE_BUFF_LEN			(128)
extern struct bFIFO usart1_receive_fifo;
extern uint8_t usart1_receive_buffer[RECEIVE_BUFF_LEN];

#define USART1_ReceiveFifo_Length()	  bFIFO_GetLength(&usart1_receive_fifo)
#define USART1_ReceiveFifo_Init()		  bFIFO_Init(&usart1_receive_fifo, usart1_receive_buffer, sizeof(usart1_receive_buffer), USART1_IRQn)
#define USART1_ReceiveFifo_Clear()		bFIFO_Clear(&usart1_receive_fifo)
#define USART1_ReceiveFifo_Glance(a)	bFIFO_Glance(&usart1_receive_fifo, a)
#define USART1_ReceiveFifo_GetByte(a)	bFIFO_GetByte(&usart1_receive_fifo, a)
#define USART1_ReceiveFifo_PutByte(a)	bFIFO_PutByte(&usart1_receive_fifo, a)
  
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Exported functions ------------------------------------------------------- */

void UART_Init(void);
void HAL_UART_ReceiveString(void);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);
void HAL_UART_SendBytes(uint8_t * str,uint16_t count);
void HAL_UART_BandRateChange(uint32_t bandrate);
uint8_t HAL_USART_GET_FLAG(USART_TypeDef * usartx,uint32_t flag);
uint32_t HAL_UART_ReceiveLength(void);
int32_t HAL_UART_ReceiveChar(uint8_t *c, uint32_t timeout);

#endif //__UART_BOARD_H__
