/*
(C)2015 NPLink

Description: MAC layer osal interface.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#ifndef LORAMAC_OSAL_H
#define  LORAMAC_OSAL_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <timer.h>
/* Macros -------------------------------------------------------------*/
//loraMAC system msg types
#define TXDONE 							1//发送完成
#define RXDONE 							2//接收完成
#define TXREQUEST						3//发送请求
#define TXERR_STATUS    		4//发送错误，找不到信道
#define TXTIMEOUT						5//发送超时
#define NETJOINREQUEST			6//入网请求
#define NETJOINOK			  	  7//成功加入网络
#define NETJOINFAIL			    8//加入网络失败

//LORAMAC时事件定义
#define  LORAMAC_LOWPOWER_MODE   0x0001 
#define  LORAMAC_NET_JION        0x0002 

/* Typedefs -----------------------------------------------------------*/
typedef struct loraMAC_msg
{
	uint8   msgID;//消息ID号
	uint8   msgLen;//消息长度
	uint16  frame_no ;//发送序列号
  int16_t msgRxRssi;//接收信号强度
	int8_t  msgRxSnr;//接收信噪比
  int8_t  tx_packet_status;//发送数据包状态
  uint8   reserve;//保留字节
	uint8   msgData[256];//消息数据payload,最大64字节
}loraMAC_msg_t;


/* Variables -----------------------------------------------------------*/
extern bool g_lora_mac_adr_switch ;
extern uint8_t IsTxConfirmed;
extern uint8_t NwkSKey[16];
extern uint8_t AppSKey[16];
extern uint8_t AppKey[16];
extern uint8_t DevEui[8];
extern uint8_t AppEui[8];

extern u8 LoraMAC_taskID;
extern u8	mode;
extern TimerEvent_t JoinReqTimer;

/* function prototypes -----------------------------------------------*/
//MAC任务初始化函数
void LoRaMAC_Init(u8 task_id);
//MAC任务消息处理函数
u8 LoRaMAC_GetRxSolt(void);
u16  LoRaMAC_ProcessEvent( u8 task_id, u16 events );
void LoRaMac_SetChannelsTxPower( u8 txPower );
void LoRaMAC_SetOtaaJoinNet( u8 otaajoinnet );
void LoRa_SetDifFrq( u32 frq );

#endif
