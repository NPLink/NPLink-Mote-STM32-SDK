#ifndef APP_OSAL_H
#define APP_OSAL_H

#include "LoRaMacUsr.h"

extern uint8_t APP_taskID;

extern LoRaMacAppPara_t g_appData;

/* Private macro -------------------------------------------------------------*/
#define APP_PERIOD_SEND 0x0001
#define APP_TEST_UART		0x0002

void APP_Init(u8 task_id);
u16 APP_ProcessEvent( u8 task_id, u16 events );

void APP_ShowMoteID( u32 moteID );

#endif
