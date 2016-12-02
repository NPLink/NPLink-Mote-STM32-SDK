#ifndef APP_OSAL_H
#define APP_OSAL_H

#include "LoRaMacUsr.h"

extern u8  APP_taskID;
extern u32 Tx_dutycycle;
extern LoRaMacAppPara_t g_appData;
extern LoRaMacMacPara_t g_macData;

/* Private macro -------------------------------------------------------------*/
//事件定义
#define  APP_PERIOD_SEND       0x0001
#define  APP_NET_JOIN      		 0x0002
#define  APP_LOWPOWER_MODE		 0x0004

void APP_Init(u8 task_id);
u16  APP_ProcessEvent( u8 task_id, u16 events );
void APP_ShowMoteID( u32 moteID );
void Error_Handler(void);
#endif
