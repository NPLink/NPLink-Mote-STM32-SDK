	/*
(C)2015 NPLink

Description: Main program routine

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

/* Includes ------------------------------------------------------------------*/
#include "osal.h"
#include "stm32l0xx_hal_rtc_ex.h"

/**
 * Main application entry point.
 */
int main( void )
{
	osal_init_system();
	osal_start_system();
	while(1);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
