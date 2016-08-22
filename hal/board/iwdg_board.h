/*
(C)2015 NPLink

Description: LED driver implementation, used for led display.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#ifndef __IWDG_BOARD_H
#define __IWDG_BOARD_H

#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */
extern IWDG_HandleTypeDef   IwdgHandle;
/*
 * Initialize LED Service.
 */
void IWDG_Configuration(void);

#ifdef __cplusplus
}
#endif


#endif //__IWDG_BOARD_H
