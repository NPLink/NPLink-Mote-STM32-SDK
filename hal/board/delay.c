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
	 for( int i = 0; i < ms; i++ )
	{
		int base_time = 4200;
		while( base_time -- )
		{
			__nop();
		}
	}
}
