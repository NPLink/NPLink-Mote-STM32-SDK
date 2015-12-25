#ifndef LORAMAC_OSAL_H
#define  LORAMAC_OSAL_H

/* macro -------------------------------------------------------------*/
//loraMAC system msg types
#define TXDONE 			1
#define RXDONE 			2
#define TXREQUEST		3

/* typedef -----------------------------------------------------------*/
typedef struct loraMAC_msg
{
	uint8	msgID;
	uint8 msgLen;
	uint8 msgData[64];		
}loraMAC_msg_t;

/* variables -----------------------------------------------------------*/
extern u8 	LoraMAC_taskID;

/* function prototypes -----------------------------------------------*/
void LoRaMAC_Init(u8 task_id);
u16  LoRaMAC_ProcessEvent( u8 task_id, u16 events );


#endif
