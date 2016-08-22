/*
(C)2015 NPLink

Description: LED driver implementation, used for led display.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: v0.1 Robxr
hisory: v0.2 zhangjh
*/


/***************************************************************************************************
 *                                             INCLUDE
 ***************************************************************************************************/
#include "iwdg_board.h"
#include "stm32l0xx_hal_iwdg.h"
/***************************************************************************************************
*                                             CONSTANTS
***************************************************************************************************/



/***************************************************************************************************
*                                              MACROS
***************************************************************************************************/

/***************************************************************************************************
*                                              TYPEDEFS
***************************************************************************************************/


/***************************************************************************************************
*                                           GLOBAL VARIABLES
***************************************************************************************************/
IWDG_HandleTypeDef   IwdgHandle;
TIM_HandleTypeDef    Input_Handle;

__IO uint32_t uwLsiFreq = 0;
__IO uint32_t uwCaptureNumber = 0;
__IO uint32_t uwPeriodValue = 0;

/***************************************************************************************************
*                                            LOCAL FUNCTION
***************************************************************************************************/
 /**************************************************************************************************
  * @fn      IWDG_Init
  *
  * @brief   
  *
  * @return  void
  */
 void IWDG_Configuration(void)
 {
		/*##-2- Get the LSI frequency: TIM21 is used to measure the LSI frequency ###*/
		//uwLsiFreq = GetLSIFrequency();
		
		/*##-3- Configure & Initialize the IWDG peripheral ######################################*/
		/* Set counter reload value to obtain 250ms IWDG TimeOut.
			 IWDG counter clock Frequency = LsiFreq/32
			 Counter Reload Value = 250ms/IWDG counter clock period
														= 0.25s / (32/LsiFreq)
														= LsiFreq/(32 * 4)
														= LsiFreq/128 */
		IwdgHandle.Instance = IWDG;
		IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
		IwdgHandle.Init.Reload =0x4E2;
		IwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

		if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
		{
			/* Initialization Error */
			//ror_Handler();
		}
		
		/*##-4- Start the IWDG #####################################################*/ 
		if(HAL_IWDG_Start(&IwdgHandle) != HAL_OK)
		{
			//ror_Handler();
		}
  }

 /***************************************************************************************************
 ***************************************************************************************************/




