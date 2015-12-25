/*
(C)2015 NPLink

Description: hal task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project


*/
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include <string.h>
#include <stdio.h>
#include "osal_memory.h"
#include "osal.h"
#include "hal_osal.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */


u8 HardWare_taskID;

void HardWare_Init(u8 task_id)
{
	HardWare_taskID = task_id;
	HAL_Init();
}


u16 HardWare_ProcessEvent( u8 task_id, u16 events )
{
	return 0 ; 
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

