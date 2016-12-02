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
#include "osal_time.h"
#include "led_board.h"

/***************************************************************************************************
*                                             CONSTANTS
***************************************************************************************************/



/***************************************************************************************************
*                                              MACROS
***************************************************************************************************/




/***************************************************************************************************
*                                              TYPEDEFS
***************************************************************************************************/

/* LED control structure */
typedef struct {
  uint8 mode;       /* Operation mode */
  uint8 todo;       /* Blink cycles left */
  uint8 onPct;      /* On cycle percentage */
  uint16 time;      /* On/off cycle time (msec) */
  uint32 next;      /* Time for next change */
} HalLedControl_t;


 typedef struct
 {
   HalLedControl_t HalLedControlTable[HAL_LED_DEFAULT_MAX_LEDS];
   uint8           sleepActive;
 } HalLedStatus_t;



/***************************************************************************************************
*                                           GLOBAL VARIABLES
***************************************************************************************************/
extern u8 HardWare_taskID;

static uint8 HalLedState;              // LED state at last set/clr/blink update

#if HAL_LED == TRUE
  //static uint8 HalSleepLedState;         // LED state at last set/clr/blink update
  static uint8 preBlinkState;            // Original State before going to blink mode
                                         // bit 0, 1, 2, 3 represent led 0, 1, 2, 3
#endif

#ifdef BLINK_LEDS
    static HalLedStatus_t HalLedStatusControl;
#endif

/***************************************************************************************************
*                                            LOCAL FUNCTION
***************************************************************************************************/
#if (HAL_LED == TRUE)
  void HalLedUpdate (void);
  void HalLedOnOff (uint8 leds, uint8 mode);
#endif /* HAL_LED */


/***************************************************************************************************
*                                            FUNCTIONS - API
***************************************************************************************************/
extern u8 osal_set_event( u8 task_id, u16 event_flag );

#define  HAL_TURN_ON_LED1()     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,ON)
#define  HAL_TURN_OFF_LED1()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,OFF)
#define  HAL_TURN_ON_LED2()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,ON)
#define  HAL_TURN_OFF_LED2()    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,OFF)


 /**************************************************
  * @fn      LED_Init
  *
  * @brief   Initialize PA0 as output and Enable GPIOA clock
  *
  * @return  void
  */
 void HalLedInit(void)
 {
    GPIO_InitTypeDef  GPIO_InitStruct;
    /* -1- Enable GPIOB Clock (to be able to program the configuration registers) */
    __GPIOA_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
    /* -2- Configure PA.0 IO in output push-pull mode to
    drive external LEDs */
    GPIO_InitStruct.Pin = (GPIO_PIN_0);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = (GPIO_PIN_13);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

 }


 /***************************************************************************************************
  * @fn      HalLedSet
  *
  * @brief   Tun ON/OFF/TOGGLE given LEDs
  *
  * @param   led - red led(led1) , green led(led2)
  *          mode - BLINK, ON, OFF
  * @return  None
  ***************************************************************************************************/
 uint8 HalLedSet_tmp (uint8 led, uint8 mode)
 {

   switch (mode)
   {
     case HAL_LED_MODE_BLINK:
        /* Default blink, 1 time, D% duty cycle */
        //HalLedBlink (leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
        if( HAL_LED_1 == led )
        {

        }
        else if( HAL_LED_2 == led )
        {

        }
        break;

     case HAL_LED_MODE_FLASH:
        /* Default flash, N times, D% duty cycle */
        if( HAL_LED_1 == led )
        {

        }
        else if( HAL_LED_2 == led )
        {

        }
        break;

     case HAL_LED_MODE_ON:
        if( HAL_LED_1 == led )
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,ON);
        }
        else if( HAL_LED_2 == led )
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,ON);
        }
        break;

     case HAL_LED_MODE_OFF:

        if( HAL_LED_1 == led )
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,OFF);
        }
        else if( HAL_LED_2 == led )
        {
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,OFF);
        }
        break;

     default:
       break;
   }



   return 0 ;

 }


 /***************************************************************************************************
  * @fn      HalLedSet
  *
  * @brief   Tun ON/OFF/TOGGLE given LEDs
  *
  * @param   led - bit mask value of leds to be turned ON/OFF/TOGGLE
  *          mode - BLINK, FLASH, TOGGLE, ON, OFF
  * @return  None
  ***************************************************************************************************/
 uint8 HalLedSet (uint8 leds, uint8 mode)
 {

#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
   uint8 led;
   HalLedControl_t *sts;

   switch (mode)
   {
     case HAL_LED_MODE_BLINK:
       /* Default blink, 1 time, D% duty cycle */
       HalLedBlink (leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
       break;

     case HAL_LED_MODE_FLASH:
       /* Default flash, N times, D% duty cycle */
       HalLedBlink (leds, HAL_LED_DEFAULT_FLASH_COUNT, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
       break;

     case HAL_LED_MODE_ON:
     case HAL_LED_MODE_OFF:
     case HAL_LED_MODE_TOGGLE:

       led = HAL_LED_1;
       leds &= HAL_LED_ALL;
       sts = HalLedStatusControl.HalLedControlTable;

       while (leds)
       {
         if (leds & led)
         {
           if (mode != HAL_LED_MODE_TOGGLE)
           {
             sts->mode = mode;  /* ON or OFF */
           }
           else
           {
             sts->mode ^= HAL_LED_MODE_ON;  /* Toggle */
           }
           HalLedOnOff (led, sts->mode);
           leds ^= led;
         }
         led <<= 1;
         sts++;
       }
       break;

     default:
       break;
   }

#elif (HAL_LED == TRUE)
   LedOnOff(leds, mode);
#else
   // HAL LED is disabled, suppress unused argument warnings
   (void) leds;
   (void) mode;
#endif /* BLINK_LEDS && HAL_LED   */

   return ( HalLedState );

 }

 /***************************************************************************************************
  * @fn      HalLedBlink
  *
  * @brief   Blink the leds
  *
  * @param   leds       - bit mask value of leds to be blinked
  *          numBlinks  - number of blinks
  *          percent    - the percentage in each period where the led
  *                       will be on
  *          period     - length of each cycle in milliseconds
  *
  * @return  None
  ***************************************************************************************************/
 void HalLedBlink (uint8 leds, uint8 numBlinks, uint8 percent, uint16 period)
 {
#if (defined (BLINK_LEDS)) && (HAL_LED == TRUE)
   uint8 led;
   HalLedControl_t *sts;

   if (leds && percent && period)
   {
     if (percent < 100)
     {
       led = HAL_LED_1;
       leds &= HAL_LED_ALL;
       sts = HalLedStatusControl.HalLedControlTable;

       while (leds)
       {
         if (leds & led)
         {
           /* Store the current state of the led before going to blinking if not already blinking */
           if(sts->mode < HAL_LED_MODE_BLINK )
             preBlinkState |= (led & HalLedState);

           sts->mode  = HAL_LED_MODE_OFF;                    /* Stop previous blink */
           sts->time  = period;                              /* Time for one on/off cycle */
           sts->onPct = percent;                             /* % of cycle LED is on */
           sts->todo  = numBlinks;                           /* Number of blink cycles */
           if (!numBlinks) sts->mode |= HAL_LED_MODE_FLASH;  /* Continuous */
           sts->next = osal_GetSystemClock();                /* Start now */
           sts->mode |= HAL_LED_MODE_BLINK;                  /* Enable blinking */
           leds ^= led;
         }
         led <<= 1;
         sts++;
       }
       // Cancel any overlapping timer for blink events
       osal_stop_timerEx(HardWare_taskID, HAL_LED_BLINK_EVENT);
       osal_set_event (HardWare_taskID, HAL_LED_BLINK_EVENT);
     }
     else
     {
       HalLedSet (leds, HAL_LED_MODE_ON);                    /* >= 100%, turn on */
     }
   }
   else
   {
     HalLedSet (leds, HAL_LED_MODE_OFF);                     /* No on time, turn off */
   }
#elif (HAL_LED == TRUE)
   percent = (leds & HalLedState) ? HAL_LED_MODE_OFF : HAL_LED_MODE_ON;
   HalLedOnOff (leds, percent);                              /* Toggle */
#else
   // HAL LED is disabled, suppress unused argument warnings
   (void) leds;
   (void) numBlinks;
   (void) percent;
   (void) period;
#endif /* BLINK_LEDS && HAL_LED */
 }

#if (HAL_LED == TRUE)
 /***************************************************************************************************
  * @fn      HalLedUpdate
  *
  * @brief   Update leds to work with blink
  *
  * @param   none
  *
  * @return  none
  ***************************************************************************************************/
 void HalLedUpdate (void)
 {
   uint8 led;
   uint8 pct;
   uint8 leds;
   HalLedControl_t *sts;
   uint32 time;
   uint16 next;
   uint16 wait;

   next = 0;
   led  = HAL_LED_1;
   leds = HAL_LED_ALL;
   sts = HalLedStatusControl.HalLedControlTable;

   /* Check if sleep is active or not */
   if (!HalLedStatusControl.sleepActive)
   {
     while (leds)
     {
       if (leds & led)
       {
         if (sts->mode & HAL_LED_MODE_BLINK)
         {
           time = osal_GetSystemClock();
           if (time >= sts->next)
           {
             if (sts->mode & HAL_LED_MODE_ON)
             {
               pct = 100 - sts->onPct;               /* Percentage of cycle for off */
               sts->mode &= ~HAL_LED_MODE_ON;        /* Say it's not on */
               HalLedOnOff (led, HAL_LED_MODE_OFF);  /* Turn it off */

               if (!(sts->mode & HAL_LED_MODE_FLASH))
               {
                 sts->todo--;                        /* Not continuous, reduce count */
               }
             }
             else if ( (!sts->todo) && !(sts->mode & HAL_LED_MODE_FLASH) )
             {
               sts->mode ^= HAL_LED_MODE_BLINK;      /* No more blinks */
             }
             else
             {
               pct = sts->onPct;                     /* Percentage of cycle for on */
               sts->mode |= HAL_LED_MODE_ON;         /* Say it's on */
               HalLedOnOff (led, HAL_LED_MODE_ON);   /* Turn it on */
             }
             if (sts->mode & HAL_LED_MODE_BLINK)
             {
               wait = (((uint32)pct * (uint32)sts->time) / 100);
               sts->next = time + wait;
             }
             else
             {
               /* no more blink, no more wait */
               wait = 0;
               /* After blinking, set the LED back to the state before it blinks */
               HalLedSet (led, ((preBlinkState & led)!=0)?HAL_LED_MODE_ON:HAL_LED_MODE_OFF);
               /* Clear the saved bit */
               preBlinkState &= (led ^ 0xFF);
             }
           }
           else
           {
             wait = sts->next - time;  /* Time left */
           }

           if (!next || ( wait && (wait < next) ))
           {
             next = wait;
           }
         }
         leds ^= led;
       }
       led <<= 1;
       sts++;
     }

     if (next)
     {
       osal_start_timerEx(HardWare_taskID, HAL_LED_BLINK_EVENT, next);   /* Schedule event */
     }
   }
 }

 /***************************************************************************************************
  * @fn      HalLedOnOff
  *
  * @brief   Turns specified LED ON or OFF
  *
  * @param   leds - LED bit mask
  *          mode - LED_ON,LED_OFF,
  *
  * @return  none
  ***************************************************************************************************/
 void HalLedOnOff (uint8 leds, uint8 mode)
 {
   if (leds & HAL_LED_1)
   {
     if (mode == HAL_LED_MODE_ON)
     {
       HAL_TURN_ON_LED1();
     }
     else
     {
       HAL_TURN_OFF_LED1();
     }
   }

   if (leds & HAL_LED_2)
   {
     if (mode == HAL_LED_MODE_ON)
     {
       HAL_TURN_ON_LED2();
     }
     else
     {
       HAL_TURN_OFF_LED2();
     }
   }



   /* Remember current state */
   if (mode)
   {
     HalLedState |= leds;
   }
   else
   {
     HalLedState &= (leds ^ 0xFF);
   }
 }
#endif /* HAL_LED */

 /***************************************************************************************************
  * @fn      HalGetLedState
  *
  * @brief   Dim LED2 - Dim (set level) of LED2
  *
  * @param   none
  *
  * @return  led state
  ***************************************************************************************************/
 uint8 HalLedGetState ()
 {
#if (HAL_LED == TRUE)
   return HalLedState;
#else
   return 0;
#endif
 }

#if 0
 /***************************************************************************************************
  * @fn      HalLedEnterSleep
  *
  * @brief   Store current LEDs state before sleep
  *
  * @param   none
  *
  * @return  none
  ***************************************************************************************************/
 void HalLedEnterSleep( void )
 {
#ifdef BLINK_LEDS
   /* Sleep ON */
   HalLedStatusControl.sleepActive = TRUE;
#endif /* BLINK_LEDS */

#if (HAL_LED == TRUE)
   /* Save the state of each led */
   HalSleepLedState = 0;
   HalSleepLedState |= HAL_STATE_LED1();
   HalSleepLedState |= HAL_STATE_LED2() << 1;
   HalSleepLedState |= HAL_STATE_LED3() << 2;
   HalSleepLedState |= HAL_STATE_LED4() << 3;

   /* TURN OFF all LEDs to save power */
   HalLedOnOff (HAL_LED_ALL, HAL_LED_MODE_OFF);
#endif /* HAL_LED */

 }

 /***************************************************************************************************
  * @fn      HalLedExitSleep
  *
  * @brief   Restore current LEDs state after sleep
  *
  * @param   none
  *
  * @return  none
  ***************************************************************************************************/
 void HalLedExitSleep( void )
 {
#if (HAL_LED == TRUE)
   /* Load back the saved state */
   HalLedOnOff(HalSleepLedState, HAL_LED_MODE_ON);

   /* Restart - This takes care BLINKING LEDS */
   HalLedUpdate();
#endif /* HAL_LED */

#ifdef BLINK_LEDS
   /* Sleep OFF */
   HalLedStatusControl.sleepActive = FALSE;
#endif /* BLINK_LEDS */
 }
#endif

 /***************************************************************************************************
 ***************************************************************************************************/




