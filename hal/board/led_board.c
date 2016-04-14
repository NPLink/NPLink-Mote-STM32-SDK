/*
(C)2015 NPLink

Description: LED driver implementation, used for led display.

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#include "led_board.h"
	    
/**************************************************
 * @fn		LED_Init
 *
 * @brief 	Initialize PA0 as output and Enable GPIOA clock
 *
 * @return	void
 */ 	
void LED_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStruct;
	 /* -1- Enable GPIOB Clock (to be able to program the configuration registers) */
  __GPIOA_CLK_ENABLE();
  /* -2- Configure PA.0 IO in output push-pull mode to
         drive external LEDs */
  GPIO_InitStruct.Pin = (GPIO_PIN_0);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
}
 
