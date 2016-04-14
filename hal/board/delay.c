/*
(C)2015 NPLink

Description: 

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "board.h"
#include "rtc_board.h"

void DelayMs( uint32_t ms )
{
  TimerHwDelayMs( ms );
}
