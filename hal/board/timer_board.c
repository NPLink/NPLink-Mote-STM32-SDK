/*
(C)2015 NPLink

Description: MCU RTC timer and low power modes management, for loraMAC schedule.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include <math.h>
#include "board.h"
#include "timer_board.h"
#include "stm32l051xx.h"

TIM_HandleTypeDef    TimHandle;

/*!
 * Hardware Time base in us
 */
#define HW_TIMER_TIME_BASE                              100 //us

/*!
 * Hardware Timer tick counter
 */
volatile uint64_t TimerTickCounter = 1;

/*!
 * Saved value of the Tick counter at the start of the next event
 */
static uint64_t TimerTickCounterContext = 0;

/*!
 * Value trigging the IRQ
 */
volatile uint64_t TimeoutCntValue = 0;


uint32_t uwPrescalerValue = 0;

void TimerHwInit( void )
{
	/* Compute the prescaler value to have TIMx counter clock equal to 10 KHz */
	uwPrescalerValue = (uint32_t) ((SystemCoreClock / 10000) - 1);

	/*##-1- Configure the TIM peripheral #######################################*/
	/* Set TIMx instance */
	TimHandle.Instance = TIMx;

	/* Initialize TIMx peripheral as follow:
			 + Period = 10000 - 1
			 + Prescaler = SystemCoreClock/10000 Note that APB clock = TIMx clock if
										 APB prescaler = 1.
			 + ClockDivision = 0
			 + Counter direction = Up
	*/
	//100us ¶¨Ê±
	TimHandle.Init.Period = 3199;//249999;//3199;//10000 - 1; //
	TimHandle.Init.Prescaler = 0;//uwPrescalerValue;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;//0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		/* Initialization Error */
		//ErrorHandler();
	}

	/*##-2- Start the TIM Base generation in interrupt mode ####################*/
	/* Start Channel1 */
	if(HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
	{
		/* Starting Error */
		//ErrorHandler();
	}
}

void TimerHwDeInit( void )
{
    /* Deinitialize the timer */
    //TIM_DeInit( TIM2 );
	HAL_TIM_Base_DeInit(&TimHandle);
}

uint32_t TimerHwGetMinimumTimeout( void )
{
    return( ceil( 2 * HW_TIMER_TIME_BASE ) );
}

void TimerHwStart( uint32_t val )
{
    TimerTickCounterContext = TimerHwGetTimerValue( );

    if( val <= HW_TIMER_TIME_BASE + 1 )
    {
        TimeoutCntValue = TimerTickCounterContext + 1;
    }
    else
    {
        TimeoutCntValue = TimerTickCounterContext + ( ( val - 1 ) / HW_TIMER_TIME_BASE );
    }
}

void TimerHwStop( void )
{
    //TIM_ITConfig( TIM2, TIM_IT_CC1, DISABLE );
    //TIM_Cmd( TIM2, DISABLE );
}

void TimerHwDelayMs( uint32_t delay )
{
    uint64_t delayValue = 0;
    uint64_t timeout = 0;

    delayValue = delay * 1000;

    timeout = TimerHwGetTimerValue( );

    while( ( ( TimerHwGetTimerValue( ) - timeout  ) * HW_TIMER_TIME_BASE ) < delayValue )
    {
    }
}

TimerTime_t TimerHwGetElapsedTime( void )
{
     return( ( ( TimerHwGetTimerValue( ) - TimerTickCounterContext ) + 1 )  * HW_TIMER_TIME_BASE );
}

TimerTime_t TimerHwGetTime( void )
{

    return TimerHwGetTimerValue( ) * HW_TIMER_TIME_BASE;
}

TimerTime_t TimerHwGetTimerValue( void )
{
    TimerTime_t val = 0;

    __disable_irq( );

    val = TimerTickCounter;

    __enable_irq( );

    return( val );
}

void TimerIncrementTickCounter( void )
{
    __disable_irq( );

    TimerTickCounter++;

    __enable_irq( );
}

void TimerHwEnterLowPowerStopMode( void )
{

}
