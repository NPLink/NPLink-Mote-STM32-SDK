#ifndef LORAMAC_OSAL_H
#define  LORAMAC_OSAL_H

#include <stdbool.h>
/* macro -------------------------------------------------------------*/
//loraMAC system msg types
#define TXDONE 			1
#define RXDONE 			2
#define TXREQUEST		3
#define TXERR_STATUS    4

/* typedef -----------------------------------------------------------*/
typedef struct loraMAC_msg
{
	uint8  msgID;
	uint8  msgLen;
    int8_t msgRxRssi;
	int8_t msgRxSnr;
    uint16 frame_no ;
    int8_t tx_packet_status;
    uint8  reserve;
	uint8 msgData[64];
}loraMAC_msg_t;

/* variables -----------------------------------------------------------*/
extern uint8_t NwkSKey[16];
extern uint8_t AppSKey[16];
extern uint32_t MoteIDAddr;
extern u8 	LoraMAC_taskID;

extern bool g_lora_mac_adr_switch ;

/* function prototypes -----------------------------------------------*/
void LoRaMAC_Init(u8 task_id);
u16  LoRaMAC_ProcessEvent( u8 task_id, u16 events );


#endif
