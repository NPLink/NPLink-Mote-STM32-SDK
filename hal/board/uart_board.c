/*
(C)2015 NPLink

Description: Uart driver implementation, used for uart press.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr 
*/
#include <string.h>
#include <stdio.h>
#include "uart_board.h"
#include "stm32l0xx_hal.h"
#include "loraMAC_osal.h"
#include "error.h"
#include "osal.h"
#include "at.h"

#define RECV_MAX_LEN 64

extern UART_HandleTypeDef UartHandle;

struct bFIFO usart1_receive_fifo;
uint8_t usart1_receive_buffer[RECEIVE_BUFF_LEN];

uint8_t head_frame_date = 0;
uint8_t head_frame_at = 0;
uint8_t recv_buf[RECV_MAX_LEN];

void UART_Init(void)
{
	UartHandle.Instance        = USARTx;
	UartHandle.Init.BaudRate   = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;

	HAL_UART_Init(&UartHandle);

		/* Enable the UART Parity Error Interrupt */
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_PE);

	/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_ERR);

	/* Enable the UART Data Register not empty Interrupt */
	__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
	
	USART1_ReceiveFifo_Init( );
	
}


/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
 GPIO_InitTypeDef  GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();
  /* Enable USART2 clock */
  USARTx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;//GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;
  
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;
    
  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
    
  /*##-3- Configure the NVIC for UART ########################################*/
  /* NVIC for USART1 */
  HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
  
  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: trasfer complete*/
}

void HAL_UART_SendBytes(uint8_t * str,uint16_t count)
{
	uint16_t i = 0 ;
	for(i = 0;i<count;i++)
	{
		USART1->TDR =  (uint8_t)(str[i]); 
		while( HAL_USART_GET_FLAG(USART1,UART_FLAG_TC)== RESET);
	}	
}

uint8_t HAL_USART_GET_FLAG(USART_TypeDef * usartx,uint32_t flag)
{
	if((usartx->ISR & flag) == flag)
		return SET;
	else
		return RESET;
}

uint32_t HAL_UART_ReceiveLength(void)
{
	return(USART1_ReceiveFifo_Length());
}

int32_t HAL_UART_ReceiveChar(uint8_t *c, uint32_t timeout)
{
		if(USART1_ReceiveFifo_Length())
		{
			USART1_ReceiveFifo_GetByte(c);
      return ERR_SUCCESS;
		}
		return ERR_TIME_OUT; 
}


void HAL_UART_ReceiveString()
{
	uint8_t temp_Byte;
	uint8_t count = 0;
	uint8_t len = RECV_MAX_LEN;
	
	while((len--) > 0)
	{
		USART1_ReceiveFifo_GetByte(&temp_Byte);

		if(temp_Byte == '$') 
		{
		  head_frame_date = 1;		
		}
		else if(temp_Byte == 'A') 
		{
		  head_frame_at = 1;		
		}
		
		if( head_frame_date == 1)
		{
			recv_buf[count] = temp_Byte;
			if((recv_buf[count] == 0x0A) && (recv_buf[count - 1] == 0x0D))
			{
				head_frame_date = 0;
				loraMAC_msg_t* pMsg = (loraMAC_msg_t*)osal_msg_allocate(9 + count + 1 );
				if(NULL != pMsg)
				{
					osal_memset(pMsg,0,9 + count + 1 );
					pMsg->msgID = TXREQUEST;
					pMsg->msgLen = count + 1;
					osal_memcpy(pMsg->msgData,recv_buf,count + 1);
					osal_msg_send(LoraMAC_taskID,(u8*)pMsg);
				}
				 osal_memset(recv_buf,0,RECV_MAX_LEN);
				 count = 0;
			}
			else
			{
				count++;
			}
		}
		if( head_frame_at == 1)	
		{
			recv_buf[count] = temp_Byte;
			if((recv_buf[count] == 0x0A) && (recv_buf[count - 1] == 0x0D) && (recv_buf[1] == 'T'))
			{
				head_frame_at = 0;
				at_command(recv_buf,count+1);
				osal_memset(recv_buf,0,RECV_MAX_LEN);
				count = 0;
			}
			else
			{
				count++;
			}
		}
	}
}

void HAL_UART_BandRateChange( uint32_t bandrate )
{
	UartHandle.Instance        = USARTx;
	UartHandle.Init.BaudRate   = bandrate;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;

	HAL_UART_Init(&UartHandle);
}

