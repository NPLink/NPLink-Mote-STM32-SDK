/*
(C)2015 NPLink

Description: Key driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#include "key_board.h" 			

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
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = GPIO_PIN_8;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

/**************************************************
 * @fn		KEY_Scan
 *
 * @brief 	Key processing function
 *
 * @param 	u8 mode  0 - don't support continuous press    1 - support continuous press
 *
 * @return	the key value  
 */ 
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;
	if(mode)
		key_up=1;  //support continuous press		  
	if(key_up&&(KEY0==0))
	{
		HAL_Delay(10);
		key_up=0;
		if(KEY0==0)
			return KEY_RIGHT;
	}
	else if(KEY0==1)
		key_up=1; 	    
 	return 0;    // no key be press
}
