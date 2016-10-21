/*
(C)2015 NPLink

Description: hal task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include <string.h>
#include <stdio.h>
#include "osal_memory.h"
#include "osal.h"
#include "timer.h"
#include "hal_osal.h"
#include "led_board.h"
#include "key_board.h"
/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 HardWare_taskID;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  HAL task hardware Initiate.
  * @param  u8 task_id -- task ID allocated by OSAL
  * @retval None
  */
void HardWare_Init(u8 task_id)
{
	HardWare_taskID = task_id;
	HAL_Init();
}

/**
  * @brief  HAL task event process routine
  * @param  u8 task_id -- task ID allocated by OSAL
  					  u16 events -- happened events of this task
  * @retval u16 -- events that haven't been processed
  */
u16 HardWare_ProcessEvent( u8 task_id, u16 events )
{
    if ( events & HAL_LED_BLINK_EVENT )
    {
        #if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)

        HalLedUpdate();

        #endif /* BLINK_LEDS && HAL_LED */

        return events ^ HAL_LED_BLINK_EVENT;
    }

    if (events & HAL_KEY_EVENT)
    {
        #if (defined HAL_KEY) && (HAL_KEY == TRUE)

        //TODO when key was pressed
        HalKeyPoll();

        #endif

        return events ^ HAL_KEY_EVENT;
    }
		
		if (events & HAL_EVENT_TIMER)
    {
				TimerIrqHandler( );
			
        return events ^ HAL_EVENT_TIMER;
    }

    return 0 ;
}


/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

