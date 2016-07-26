/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Src/stm32l0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_it.h"
#include "osal_tick.h"
#include "timer_board.h"
#include "uart_board.h"
#include "timer.h"
#include "uart_board.h"
#include "osal.h"
#include "app_osal.h"
#include "delay.h"
#include "led_board.h"

#include "LoraMac_osal.h"
/** @addtogroup STM32L0xx_HAL_Examples
  * @{
  */

/** @addtogroup TIM_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern RTC_HandleTypeDef RTCHandle;
extern UART_HandleTypeDef UartHandle;
extern  u8 txuartdataflag ;

u8 step;
u8 stepflag = 1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  osal_tick++;
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/*!
 * Timer2 IRQ handler,Timer2 is used for MAC schedule
 */
void TIM2_IRQHandler( void )
{
	HAL_TIM_IRQHandler(&TimHandle);

  TimerIncrementTickCounter( );

	if( TimerTickCounter == TimeoutCntValue )
	{
	  TimerIrqHandler( );
	}
}

/**
  * @brief  This function handles RTC Auto wake-up interrupt request. RTC is used for low power mode timer
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	HAL_RTC_AlarmIRQHandler(&RTCHandle);
}

/******************************************************************************/
/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA stream 
  *         used for USART data transmission     
  */
void USART1_IRQHandler(void)
{
  UART_HandleTypeDef *huart = &UartHandle;
  u8 ch = 0;

  /* UART parity error interrupt occurred ------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_PE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE) != RESET))
  {
          __HAL_UART_CLEAR_IT(huart, UART_CLEAR_PEF);

          huart->ErrorCode |= HAL_UART_ERROR_PE;
          /* Set the UART state ready to be able to start again the process */
          huart->State = HAL_UART_STATE_READY;
  }

  /* UART frame error interrupt occured --------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_FE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  {
          __HAL_UART_CLEAR_IT(huart, UART_CLEAR_FEF);

          huart->ErrorCode |= HAL_UART_ERROR_FE;
          /* Set the UART state ready to be able to start again the process */
          huart->State = HAL_UART_STATE_READY;
  }

  /* UART noise error interrupt occured --------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_NE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  {
          __HAL_UART_CLEAR_IT(huart, UART_CLEAR_NEF);

          huart->ErrorCode |= HAL_UART_ERROR_NE;
          /* Set the UART state ready to be able to start again the process */
          huart->State = HAL_UART_STATE_READY;
  }

  /* UART Over-Run interrupt occured -----------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_ORE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR) != RESET))
  {
          __HAL_UART_CLEAR_IT(huart, UART_CLEAR_OREF);

          huart->ErrorCode |= HAL_UART_ERROR_ORE;
          /* Set the UART state ready to be able to start again the process */
          huart->State = HAL_UART_STATE_READY;
  }

  /* Call UART Error Call back function if need be --------------------------*/
  if(huart->ErrorCode != HAL_UART_ERROR_NONE)
  {
          HAL_UART_ErrorCallback(huart);
  }

  /* UART Wake Up interrupt occured ------------------------------------------*/
  if((__HAL_UART_GET_IT(huart, UART_IT_WUF) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_WUF) != RESET))
  {
          __HAL_UART_CLEAR_IT(huart, UART_CLEAR_WUF);
          /* Set the UART state ready to be able to start again the process */
          huart->State = HAL_UART_STATE_READY;
          HAL_UARTEx_WakeupCallback(huart);
  }

  /* UART in mode Receiver ---------------------------------------------------*/	
  if((__HAL_UART_GET_IT(huart, UART_IT_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE) != RESET))
  {
		 ch = (uint8_t)(USART1->RDR );
		 switch(step)
			{
				case 0:
					if(ch == '$')
					{
					step++;
					uart1_Rxcount= 0;
					memset(uart1_rxBuf,0,80);//清空数据
					uart1_rxBuf[uart1_Rxcount] = ch;
					uart1_Rxcount++;
					}
					break;
					
				case 1:
					uart1_rxBuf[uart1_Rxcount++] = ch;
					if((uart1_rxBuf[uart1_Rxcount-1] == 0X0A)&& (uart1_rxBuf[uart1_Rxcount-2] == 0X0D))
					{                        				
					  txuartdataflag = 1; //osal_msg_send
					  step = 0;

						loraMAC_msg_t* pMsg = (loraMAC_msg_t*)osal_msg_allocate(8 + uart1_Rxcount);
						if(NULL != pMsg)
						{
							osal_memset(pMsg,0,8 + uart1_Rxcount);
							pMsg->msgID = TXREQUEST;
							pMsg->msgLen = uart1_Rxcount;
							osal_memcpy(pMsg->msgData,uart1_rxBuf,uart1_Rxcount);
							osal_msg_send(LoraMAC_taskID,(u8*)pMsg);//发消息给MAC层，将串口数据通过无线发送出去
						}
					}
					if(uart1_Rxcount >= 70)
					{
						step = 0;
					}				
					break;
					
				default:
					step = 0;
					break;
			}
  }

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
