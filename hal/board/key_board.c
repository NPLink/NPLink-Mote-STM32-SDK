/*
(C)2015 NPLink

Description: Key driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: V0.1 Robxr
update history: V0.2 zhangjh
*/
#include "osal_time.h"
#include "delay.h"
#include "key_board.h"
#include "gpio_board.h"
#include "oled_board.h"
#include "led_board.h"
#include "app_osal.h"


#if (defined HAL_KEY) && (HAL_KEY == TRUE)

/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/
extern u8 osal_set_event( u8 task_id, u16 event_flag );


/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
void halProcessKeyInterrupt(void);


/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
#define HAL_KEY_DEBOUNCE_VALUE  25


/**************************************************************************************************
 *                                            TYPEDEFS
 **************************************************************************************************/


/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/
extern u8 HardWare_taskID;

bool Hal_KeyIntEnable;            /* interrupt enable/disable flag */

u8 g_key1_push_dowm = false ;


#define HAL_PUSH_BUTTON1()        ( g_key1_push_dowm )//HAL_GPIO_ReadPin

/**************************************************
 * @fn		KEY_Init
 *
 * @brief 	initialize key
 *
 * @return	void
 */
void KEY_Init(void)
{
		GPIO_InitTypeDef   GPIO_InitStructure;
		/* Enable GPIOA clock */
		__GPIOA_CLK_ENABLE();
		/* Configure PA8 pin as input floating */
		GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Pin = GPIO_PIN_8;
		GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if 0
		if(GPIO_Pin == GPIO_PIN_8)
		{
				OLED_Clear();	//按一次按键清屏
		}
#else
		{
			if(GPIO_Pin == GPIO_PIN_8)
			{
				if(GPIO_Pin == GPIO_PIN_8)
				{
					g_key1_push_dowm = true ; //按键1按下
				}
			}
			else
			{
				 g_key1_push_dowm = false ; //按键1未按下
			}

			halProcessKeyInterrupt();
		}
#endif
}





/**************************************************************************************************
 * @fn      HalKeyInit
 *
 * @brief   Initilize Key Service
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void HalKeyInit( void )
{
		/* Initialize previous key to 0 */
		KEY_Init(  );

		g_key1_push_dowm = false ;

		Hal_KeyIntEnable = true ;
}


/**************************************************************************************************
 * @fn      HalKeyRead
 *
 * @brief   Read the current value of a key
 *
 * @param   None
 *
 * @return  keys - current keys status
 **************************************************************************************************/
uint8 HalKeyRead ( void )
{
		uint8 keys = 0;

		if (HAL_PUSH_BUTTON1())
		{
						keys |= HAL_KEY_1;
		}

		return keys;
}


/**************************************************************************************************
 * @fn      HalKeyPoll
 *
 * @brief   Called by hal_driver to poll the keys
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalKeyPoll (void)
{
		if (HAL_PUSH_BUTTON1())//if(GPIO_Pin == GPIO_PIN_8)
		{
				OLED_Clear();	//按一次按键清屏
				APP_ShowMoteID( g_appData.devAddr);
		}
		else
		{
		}


}

/**************************************************************************************************
 * @fn      halProcessKeyInterrupt
 *
 * @brief   Checks to see if it's a valid key interrupt, saves interrupt driven key states for
 *          processing by HalKeyRead(), and debounces keys by scheduling HalKeyRead() 25ms later.
 *
 * @param
 *
 * @return
 **************************************************************************************************/
void halProcessKeyInterrupt (void)
{
		osal_start_timerEx (HardWare_taskID, HAL_KEY_EVENT, HAL_KEY_DEBOUNCE_VALUE);
}



#endif /* HAL_KEY */


