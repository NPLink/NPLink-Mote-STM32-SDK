/*
(C)2015 NPLink

Description: 

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#include "board.h"
#include "rtc_board.h"

void DelayMs( uint32_t ms )
{
#ifdef LOW_POWER_MODE_ENABLE  
    RtcDelayMs( ms );
#else
    TimerHwDelayMs( ms );
#endif
}
