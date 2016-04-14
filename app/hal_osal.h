/*
(C)2015 NPLink

Description: hal task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#ifndef HARDWARE_OSAL_H
#define HARDWARE_OSAL_H

/* Variables ---------------------------------------------------------*/
extern u8 HardWare_taskID;

/* Functions ---------------------------------------------------------*/
void HardWare_Init(u8 task_id);
u16 HardWare_ProcessEvent( u8 task_id, u16 events );

#endif
