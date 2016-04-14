/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: MCU RTC timer and low power modes management

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <math.h>
#include <time.h>
#include "board.h"
#include "rtc_board.h"
#include "stm32l0xx_hal_rtc.h"
#include "stm32l051xx.h"
#include "led_board.h"


extern RTC_HandleTypeDef RTCHandle;

/*!
 * RTC Time base in us
 */
#define RTC_ALARM_TIME_BASE                             122.07 //1000000 //

/*!
 * MCU Wake Up Time
 */
#define MCU_WAKE_UP_TIME                                3400

/*!
 * \brief Configure the Rtc hardware
 */
static void RtcSetConfig( void );

/*!
 * \brief Start the Rtc Alarm (time base 1s)
 */
static void RtcStartWakeUpAlarm( uint32_t timeoutValue );

/*!
 * \brief Read the MCU internal Calendar value
 *
 * \retval Calendar value
 */
static TimerTime_t RtcGetCalendarValue( void );

/*!
 * \brief Clear the RTC flags and Stop all IRQs
 */
static void RtcClearStatus( void );

/*!
 * \brief RTC config from wake up Alarm
 */

void SystemClockConfig_STOP(void);

/*!
 * \brief Indicates if the RTC is already Initalized or not
 */
static bool RtcInitalized = false;

/*!
 * \brief Flag to indicate if the timestamps until the next event is long enough
 * to set the MCU into low power mode
 */
static bool RtcTimerEventAllowsLowPower = false;

/*!
 * \brief Flag to disable the LowPower Mode even if the timestamps until the
 * next event is long enough to allow Low Power mode
 */
static bool LowPowerDisableDuringTask = false;

/*!
 * Keep the value of the RTC timer when the RTC alarm is set
 */
static TimerTime_t RtcTimerContext = 0;

/*!
 * Number of seconds in a minute
 */
static const uint8_t SecondsInMinute = 60;

/*!
 * Number of seconds in an hour
 */
static const uint16_t SecondsInHour = 3600;

/*!
 * Number of seconds in a day
 */
static const uint32_t SecondsInDay = 86400;

/*!
 * Number of hours in a day
 */
static const uint8_t HoursInDay = 24;

/*!
 * Number of days in a standard year
 */
static const uint16_t DaysInYear = 365;

/*!
 * Number of days in a leap year
 */
static const uint16_t DaysInLeapYear = 366;

/*!
 * Number of days in a century
 */
static const double DaysInCentury = 36524.219;

/*!
 * Number of days in each month on a normal year
 */
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Number of days in each month on a leap year
 */
static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Hold the previous year value to detect the turn of a century
 */
static uint8_t PreviousYear = 0;

//static TimerTime_t calendarValue = 0;

/*!
 * Century counter
 */
static uint8_t Century = 0;

void RtcInit( void )
{
    if( RtcInitalized == false )
    {
        RtcSetConfig( );
        RtcInitalized = true;
    }
}

static void RtcSetConfig( void )
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

  //RTC_AlarmTypeDef salarmstructure;

	/* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();

	/* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_StopWakeUpClock_HSI);

  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RTCHandle.Instance = RTC;
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = 0x01;//RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = 0x01;//RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  //Initiate data and time

	RTC_TimeStruct.TimeFormat = RTC_HOURFORMAT12_AM;
  RTC_TimeStruct.Hours = 0;
  RTC_TimeStruct.Minutes = 0;
  RTC_TimeStruct.Seconds = 0;
  RTC_TimeStruct.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  RTC_TimeStruct.StoreOperation = RTC_STOREOPERATION_RESET;

   // RTC_TimeStructInit( &RTC_TimeStruct );
	RTC_DateStruct.WeekDay = RTC_WEEKDAY_MONDAY;
  RTC_DateStruct.Date = 1;
  RTC_DateStruct.Month = RTC_MONTH_JANUARY;
  RTC_DateStruct.Year = 0;

  HAL_RTC_SetDate(&RTCHandle,&RTC_DateStruct,FORMAT_BCD);
  HAL_RTC_SetTime(&RTCHandle,&RTC_TimeStruct,FORMAT_BCD);

	//HAL_RTC_WaitForSynchro(&RTCHandle);
}

void RtcStopTimer( void )
{
    RtcClearStatus( );
}

uint32_t RtcGetMinimumTimeout( void )
{
  //return RTC_ALARM_TIME_BASE;

  #if 1
    return( ceil( 3 * RTC_ALARM_TIME_BASE ) );
  #endif
}

void RtcSetTimeout( uint32_t timeout )
{
    uint32_t timeoutValue = 0;

    timeoutValue = timeout;

    #if 1
    if( timeoutValue < ( 3 * RTC_ALARM_TIME_BASE ) )
    {
        timeoutValue = 3 * RTC_ALARM_TIME_BASE;
    }
    #endif

    //if( timeoutValue < ( RTC_ALARM_TIME_BASE ) )
    //{
        //timeoutValue = RTC_ALARM_TIME_BASE;
    //}

    if( timeoutValue < 55000 )
    {
        // we don't go in Low Power mode for delay below 50ms (needed for LEDs)
        RtcTimerEventAllowsLowPower = false;
    }
    else
    {
        RtcTimerEventAllowsLowPower = true;
    }

    if( ( LowPowerDisableDuringTask == false ) && ( RtcTimerEventAllowsLowPower == true ) )
    {
        timeoutValue = timeoutValue - MCU_WAKE_UP_TIME;
    }

    RtcStartWakeUpAlarm( timeoutValue );
}


uint32_t RtcGetTimerElapsedTime( void )
{
  #if 1
    TimerTime_t CalendarValue = 0;

    CalendarValue = RtcGetCalendarValue( );

    //return( ( uint32_t )( ceil ( ( ( CalendarValue - RtcTimerContext ) ) * RTC_ALARM_TIME_BASE ) ) );
    return( ( uint32_t )( ceil ( ( ( CalendarValue - RtcTimerContext ) + 2 ) * RTC_ALARM_TIME_BASE ) ) );

  #endif

  //RtcGetCalendarValue( );
  //return( ( uint32_t )( ( ( ( calendarValue - RtcTimerContext ) ) * RTC_ALARM_TIME_BASE ) ) );
}

TimerTime_t RtcGetTimerValue( void )
{
	#if 1
    TimerTime_t CalendarValue = 0;

    CalendarValue = RtcGetCalendarValue( );

    return( ( CalendarValue + 2 ) * RTC_ALARM_TIME_BASE );
    //return( ( CalendarValue ) * RTC_ALARM_TIME_BASE );
	#endif

	//RtcGetCalendarValue( );
	//return( ( calendarValue ) * RTC_ALARM_TIME_BASE );
}

static void RtcClearStatus( void )
{
    /* Clear RTC Alarm Flag */
    //RTC_ClearFlag(RTC_FLAG_ALRAF);
	__HAL_RTC_ALARM_CLEAR_FLAG(&RTCHandle, RTC_FLAG_ALRAF );

    /* Enable RTC Alarm A Interrupt */
   /// RTC_ITConfig( RTC_IT_ALRA, DISABLE );

    /* Enable the Alarm A */
   // RTC_AlarmCmd( RTC_Alarm_A, DISABLE );

	 __HAL_RTC_ALARMA_DISABLE(&RTCHandle);
}

static void RtcStartWakeUpAlarm( uint32_t timeoutValue )
{
    uint16_t rtcSeconds = 0;
    uint16_t rtcMinutes = 0;
    uint16_t rtcHours = 0;
    uint16_t rtcDays = 0;

    uint8_t rtcAlarmSeconds = 0;
    uint8_t rtcAlarmMinutes = 0;
    uint8_t rtcAlarmHours = 0;
    uint16_t rtcAlarmDays = 0;

    RTC_AlarmTypeDef RTC_AlarmStructure;
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    RtcClearStatus( );

    RtcTimerContext = RtcGetCalendarValue( );

    //RTC_GetTime( RTC_Format_BIN, &RTC_TimeStruct );
    //RTC_GetDate( RTC_Format_BIN, &RTC_DateStruct );

	  HAL_RTC_GetTime(&RTCHandle, &RTC_TimeStruct, FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &RTC_DateStruct, FORMAT_BIN);

    timeoutValue = timeoutValue / RTC_ALARM_TIME_BASE;

    if( timeoutValue > 2160000 ) // 25 "days" in tick
    {                            // drastically reduce the computation time
        rtcAlarmSeconds = RTC_TimeStruct.Seconds;
        rtcAlarmMinutes = RTC_TimeStruct.Minutes;
        rtcAlarmHours = RTC_TimeStruct.Hours;
        rtcAlarmDays = 25 + RTC_DateStruct.Date;  // simply add 25 days to current date and time

        if( ( RTC_DateStruct.Year == 0 ) || ( RTC_DateStruct.Year % 4 == 0 ) )
        {
            if( rtcAlarmDays > DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ] )
            {
                rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[ RTC_DateStruct.Month - 1];
            }
        }
        else
        {
            if( rtcAlarmDays > DaysInMonth[ RTC_DateStruct.Month - 1 ] )
            {
                rtcAlarmDays = rtcAlarmDays % DaysInMonth[ RTC_DateStruct.Month - 1];
            }
        }
    }
    else
    {
        rtcSeconds = ( timeoutValue % SecondsInMinute ) + RTC_TimeStruct.Seconds;
        rtcMinutes = ( ( timeoutValue / SecondsInMinute ) % SecondsInMinute ) + RTC_TimeStruct.Minutes;
        rtcHours = ( ( timeoutValue / SecondsInHour ) % HoursInDay ) + RTC_TimeStruct.Hours;
        rtcDays = ( timeoutValue / SecondsInDay ) + RTC_DateStruct.Date;

        rtcAlarmSeconds = ( rtcSeconds ) % 60;
        rtcAlarmMinutes = ( ( rtcSeconds / 60 ) + rtcMinutes ) % 60;
        rtcAlarmHours   = ( ( ( ( rtcSeconds / 60 ) + rtcMinutes ) / 60 ) + rtcHours ) % 24;
        rtcAlarmDays    = ( ( ( ( ( rtcSeconds / 60 ) + rtcMinutes ) / 60 ) + rtcHours ) / 24 ) + rtcDays;

        if( ( RTC_DateStruct.Year == 0 ) || ( RTC_DateStruct.Year % 4 == 0 ) )
        {
            if( rtcAlarmDays > DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ] )
            {
                rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ];
            }
        }
        else
        {
            if( rtcAlarmDays > DaysInMonth[ RTC_DateStruct.Month - 1 ] )
            {
                rtcAlarmDays = rtcAlarmDays % DaysInMonth[ RTC_DateStruct.Month - 1 ];
            }
        }
    }

    RTC_AlarmStructure.Alarm = RTC_ALARM_A;
    RTC_AlarmStructure.AlarmDateWeekDay = ( uint8_t )rtcAlarmDays;
    RTC_AlarmStructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    RTC_AlarmStructure.AlarmMask = RTC_ALARMMASK_NONE;
    RTC_AlarmStructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    RTC_AlarmStructure.AlarmTime.TimeFormat = RTC_TimeStruct.TimeFormat;
    RTC_AlarmStructure.AlarmTime.Hours = rtcAlarmHours;
    RTC_AlarmStructure.AlarmTime.Minutes = rtcAlarmMinutes;
    RTC_AlarmStructure.AlarmTime.Seconds = rtcAlarmSeconds;
    RTC_AlarmStructure.AlarmTime.SubSeconds = 0x00;

    if(HAL_RTC_SetAlarm_IT(&RTCHandle,&RTC_AlarmStructure,FORMAT_BIN) != HAL_OK)
    {

    }

    #if 0
    RTC_AlarmStructure.AlarmTime.Seconds = rtcAlarmSeconds;
    RTC_AlarmStructure.AlarmTime.Minutes = rtcAlarmMinutes;
    RTC_AlarmStructure.AlarmTime.Hours   = rtcAlarmHours;
    RTC_AlarmStructure.AlarmDateWeekDay      = ( uint8_t )rtcAlarmDays;
    RTC_AlarmStructure.AlarmTime.TimeFormat     = RTC_TimeStruct.TimeFormat;
    RTC_AlarmStructure.AlarmDateWeekDaySel   = RTC_ALARMDATEWEEKDAYSEL_DATE;
    RTC_AlarmStructure.AlarmMask             = RTC_ALARMMASK_NONE;


    RTC_SetAlarm( RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure );

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro( );
		HAL_RTC_WaitForSynchro(&RTCHandle);

    /* Enable RTC Alarm A Interrupt */
    RTC_ITConfig( RTC_IT_ALRA, ENABLE );

    /* Enable the Alarm A */
    RTC_AlarmCmd( RTC_Alarm_A, ENABLE );
    #endif

}

void RtcEnterLowPowerStopMode( void )
{
  if( ( LowPowerDisableDuringTask == false ) && ( RtcTimerEventAllowsLowPower == true ) )
  {
      // Disable IRQ while the MCU is being deinitialized to prevent race issues
      __disable_irq( );

      Radio.Sleep( );
      HAL_MspDeInit();

      __enable_irq( );

      /* Enter Stop Mode */
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  }

  #if 0
    if( ( LowPowerDisableDuringTask == false ) && ( RtcTimerEventAllowsLowPower == true ) )
    {
        // Disable IRQ while the MCU is being deinitialized to prevent race issues
        __disable_irq( );

        BoardDeInitMcu( );

        __enable_irq( );

        /* Disable the Power Voltage Detector */
        PWR_PVDCmd( DISABLE );

        /* Set MCU in ULP (Ultra Low Power) */
        PWR_UltraLowPowerCmd( ENABLE );

        /*Disable fast wakeUp*/
        PWR_FastWakeUpCmd( DISABLE );

        /* Enter Stop Mode */
        //PWR_EnterSTOPMode( PWR_Regulator_LowPower, PWR_STOPEntry_WFI );
    }
    #endif
}

void RtcRecoverMcuStatus( void )
{
  if( TimerGetLowPowerEnable( ) == true )
  {
      //if( ( LowPowerDisableDuringTask == false ) && ( RtcTimerEventAllowsLowPower == true ) )
      {
          // Disable IRQ while the MCU is not running on HSE
          __disable_irq( );

          SystemClockConfig_STOP();
          //需要恢复哪些??
          LED_Init();
          SPI1_Init();
          SX1276IoInit( );
          //SX1276Init(RadioEvents_t * events) ????

          __enable_irq( );
      }
  }

	#if 0
    if( TimerGetLowPowerEnable( ) == true )
    {
        if( ( LowPowerDisableDuringTask == false ) && ( RtcTimerEventAllowsLowPower == true ) )
        {
            // Disable IRQ while the MCU is not running on HSE
            __disable_irq( );

            /* After wake-up from STOP reconfigure the system clock */
            /* Enable HSE */
            RCC_HSEConfig( RCC_HSE_ON );

            /* Wait till HSE is ready */
            while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
            {}

            /* Enable PLL */
            RCC_PLLCmd( ENABLE );

            /* Wait till PLL is ready */
            while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET )
            {}

            /* Select PLL as system clock source */
      //      RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

            /* Wait till PLL is used as system clock source */
            while( RCC_GetSYSCLKSource( ) != 0x0C )
            {}

            /* Set MCU in ULP (Ultra Low Power) */
            PWR_UltraLowPowerCmd( DISABLE ); // add up to 3ms wakeup time

            /* Enable the Power Voltage Detector */
            PWR_PVDCmd( ENABLE );

            BoardInitMcu( );

            __enable_irq( );
        }
    }
		#endif
}

void BlockLowPowerDuringTask( bool status )
{
    if( status == true )
    {
      RtcRecoverMcuStatus( );
			//SystemClockConfig_STOP();
    }

    LowPowerDisableDuringTask = status;
}

void RtcDelayMs( uint32_t delay )
{
    TimerTime_t delayValue = 0;
    TimerTime_t timeout = 0;

    delayValue = ( TimerTime_t )( delay * 1000 );

    // Wait delay ms
    timeout = RtcGetTimerValue( );
    while( ( ( RtcGetTimerValue( ) - timeout ) ) < delayValue )
    {
			//__NOP( );
    }
}

TimerTime_t RtcGetCalendarValue( void )
{
    TimerTime_t calendarValue = 0;
	  //calendarValue = 0;
    uint8_t i = 0;

    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    // RTC_GetTime( RTC_Format_BIN, &RTC_TimeStruct );
    //RTC_GetDate( RTC_Format_BIN, &RTC_DateStruct );
	  HAL_RTC_GetTime(&RTCHandle, &RTC_TimeStruct, FORMAT_BIN);

    HAL_RTC_GetDate(&RTCHandle, &RTC_DateStruct, FORMAT_BIN);

    //RTC_WaitForSynchro( );
	  HAL_RTC_WaitForSynchro(&RTCHandle);

    if( ( PreviousYear == 99 ) && ( RTC_DateStruct.Year == 0 ) )
    {
        Century++;
    }
    PreviousYear = RTC_DateStruct.Year;

    // century
    for( i = 0; i < Century; i++ )
    {
        calendarValue += ( TimerTime_t )( DaysInCentury * SecondsInDay );
    }

    // years
    for( i = 0; i < RTC_DateStruct.Year; i++ )
    {
        if( ( i == 0 ) || ( i % 4 == 0 ) )
        {
            calendarValue += DaysInLeapYear * SecondsInDay;
        }
        else
        {
            calendarValue += DaysInYear * SecondsInDay;
        }
    }

    // months
    if( ( RTC_DateStruct.Year == 0 ) || ( RTC_DateStruct.Year % 4 == 0 ) )
    {
        for( i = 0; i < ( RTC_DateStruct.Month - 1 ); i++ )
        {
            calendarValue += DaysInMonthLeapYear[i] * SecondsInDay;
        }
    }
    else
    {
        for( i = 0; i < ( RTC_DateStruct.Month - 1 ); i++ )
        {
            calendarValue += DaysInMonth[i] * SecondsInDay;
        }
    }

    // days
    calendarValue += ( ( uint32_t )RTC_TimeStruct.Seconds +
                      ( ( uint32_t )RTC_TimeStruct.Minutes * SecondsInMinute ) +
                      ( ( uint32_t )RTC_TimeStruct.Hours * SecondsInHour ) +
                      ( ( uint32_t )( RTC_DateStruct.Date * SecondsInDay ) ) );

    return( calendarValue );
}


//休眠唤醒后的时钟初始化
//clock initiate after wakeup
void SystemClockConfig_STOP(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Get the Oscillators configuration according to the internal RCC registers */
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  /* After wake-up from STOP reconfigure the system clock: Enable HSI and PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {

  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {

  }
}



/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_AlarmConfig(void)
{
  RTC_DateTypeDef  sdatestructure;
  RTC_TimeTypeDef  stimestructure;
  RTC_AlarmTypeDef salarmstructure;

	/* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();

	/* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_StopWakeUpClock_HSI);

  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RTCHandle.Instance = RTC;
  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-2- Configure the RTC Alarm peripheral #################################*/
  /* Set Alarm to 02:20:30
     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  salarmstructure.Alarm = RTC_ALARM_A;
  salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_None;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  salarmstructure.AlarmTime.Hours = 0x02;
  salarmstructure.AlarmTime.Minutes = 0x20;
  salarmstructure.AlarmTime.Seconds = 0x30;
  salarmstructure.AlarmTime.SubSeconds = 0x56;

  if(HAL_RTC_SetAlarm_IT(&RTCHandle,&salarmstructure,FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-3- Configure the Date #################################################*/
  /* Set Date: Monday April 14th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_APRIL;
  sdatestructure.Date = 0x14;
  sdatestructure.WeekDay = RTC_WEEKDAY_MONDAY;

  if(HAL_RTC_SetDate(&RTCHandle,&sdatestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-4- Configure the Time #################################################*/
  /* Set Time: 02:20:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x20;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if(HAL_RTC_SetTime(&RTCHandle,&stimestructure,FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 32000000
  *            HCLK(Hz)                       = 32000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_MUL                        = 4
  *            PLL_DIV                        = 2
  *            Flash Latency(WS)              = 1
  *            Main regulator output voltage  = Scale1 mode
  * @param  None
  * @retval None
  */

/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param hrtc: RTC handle pointer
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *        the RTC clock source; in this case the Backup domain will be reset in
  *        order to modify the RTC Clock source, as consequence RTC registers (including
  *        the backup registers) and RCC_CSR register are set to their reset values.
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Configue the RTC clock soucre ######################################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  /* -a- Enable LSE Oscillator */
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  /* -b- Select LSE as RTC clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    while(1);
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)
  /* -a- Enable LSI Oscillator */
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /* -b- Select LSI as RTC clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    while(1);
  }
#else
#error Please select the RTC Clock source inside the main.h file
#endif /*RTC_CLOCK_SOURCE_LSE*/

  /*##-2- Enable the RTC peripheral Clock ####################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();

  /*##-3- Configure the NVIC for RTC Wake up Tamper  #########################*/
  HAL_NVIC_SetPriority(RTC_IRQn, 0x0, 0);
  HAL_NVIC_EnableIRQ(RTC_IRQn);
}

/**
  * @brief RTC MSP De-Initialization
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();
}


void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Clear Wake Up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}


void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  RtcRecoverMcuStatus();
  TimerIrqHandler( );
}

