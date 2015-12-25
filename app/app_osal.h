#ifndef APP_OSAL_H
#define APP_OSAL_H

void APP_Init(u8 task_id);
u16 APP_ProcessEvent( u8 task_id, u16 events );

#endif
