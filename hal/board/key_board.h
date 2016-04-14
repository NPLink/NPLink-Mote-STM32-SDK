/*
(C)2015 NPLink

Description: Key driver implementation, used for key press.

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#ifndef __KEY_BOARD_H
#define __KEY_BOARD_H	 
#include "stm32l0xx_hal.h"

#define KEY0  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)

#define KEY_RIGHT	1

void KEY_Init(void);
u8 KEY_Scan(u8); 

#endif //__KEY_BOARD_H
