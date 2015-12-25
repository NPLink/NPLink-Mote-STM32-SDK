/*
(C)2015 NPLink

Description: GPIO interrupt handle (for 1276/1279)

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#include "board.h"
#include "gpio_board.h"
#include "rtc_board.h"

GpioIrqHandler *GpioIrq[16] ;

void EXTI0_1_IRQHandler( void )
{
	#if 0
#ifdef LOW_POWER_MODE_ENABLE
    RtcRecoverMcuStatus( );

#endif
    if( EXTI_GetITStatus( EXTI_Line0 ) != RESET )
    {
        if( GpioIrq[0] != NULL )
        {
            GpioIrq[0]( );
        }
        EXTI_ClearITPendingBit( EXTI_Line0 );
    }
		#endif

  if( TimerGetLowPowerEnable( ) == true )
  {
      RtcRecoverMcuStatus( );
  }

  //PB0 -- DIO3
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
	{
	  if( GpioIrq[3] != NULL )
	  {
		  GpioIrq[3]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}

  //PB1 -- DIO2
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET)
	{
	  if( GpioIrq[2] != NULL )
	  {
		  GpioIrq[2]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	}
}

void EXTI2_3_IRQHandler( void )
{
	#if 0
#ifdef LOW_POWER_MODE_ENABLE
    RtcRecoverMcuStatus( );
#endif
    if( EXTI_GetITStatus( EXTI_Line1 ) != RESET )
    {
        if( GpioIrq[1] != NULL )
        {
            GpioIrq[1]( );
        }
        EXTI_ClearITPendingBit( EXTI_Line1 );
    }
		#endif

  if( TimerGetLowPowerEnable( ) == true )
  {
      RtcRecoverMcuStatus( );
  }

  //PB2 -- DIO1
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
	{
	  if( GpioIrq[1] != NULL )
	  {
		  GpioIrq[1]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	}
}

void EXTI4_15_IRQHandler( void )
{
	#if 0
#ifdef LOW_POWER_MODE_ENABLE
    RtcRecoverMcuStatus( );
#endif
    if( EXTI_GetITStatus( EXTI_Line2 ) != RESET )
    {
        if( GpioIrq[2] != NULL )
        {
            GpioIrq[2]( );
        }
        EXTI_ClearITPendingBit( EXTI_Line2 );
    }
		#endif

  if( TimerGetLowPowerEnable( ) == true )
  {
      RtcRecoverMcuStatus( );
  }

  //PB10 -- DIO0
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET)
	{
	  if( GpioIrq[0] != NULL )
	  {
		  GpioIrq[0]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
	}
}

