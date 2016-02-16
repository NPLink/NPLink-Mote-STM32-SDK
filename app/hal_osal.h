#ifndef HARDWARE_OSAL_H
#define HARDWARE_OSAL_H

extern u8 HardWare_taskID;

void HardWare_Init(u8 task_id);
u16 HardWare_ProcessEvent( u8 task_id, u16 events );

#endif
