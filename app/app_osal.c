/*
(C)2015 NPLink

Description: app task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project


*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include "stm32l0xx_hal_rtc.h"
#include <string.h>
#include <stdio.h>
#include "osal_memory.h"
#include "osal.h"
#include "oled_board.h"
#include "app_osal.h"
#include "loraMac_osal.h"
#include "LoRaMacUsr.h"

#include "radio.h"
#include "timer.h"
#include "uart_board.h"
#include "led_board.h"
#include "rtc_board.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define APP_PERIOD_SEND 0x0001
#define APP_TEST_UART		0x0002

/* Private variables ---------------------------------------------------------*/
u8 APP_taskID;
char Rx_buf[64]; //buffer for oled display
u32 Rxpacket_count = 0 ;
u8* RecieveBuff_flag = NULL;

//for uart test
__IO ITStatus UartReady = RESET;
u8 aTxBuffer[] = "uart test, hello!\n";
u8 aRxBuffer[RXBUFFERSIZE];
u8 g_number = 0;

extern UART_HandleTypeDef UartHandle;


/* Private function prototypes -----------------------------------------------*/
void APP_ShowMoteID( u32 moteID );


/* Private functions ---------------------------------------------------------*/
void APP_Init(u8 task_id)
{
	//LoRaMacAppPara_t appData;

	//LoRaMacMacPara_t macData;
	
	LoRaMacAppPara_t g_appData;
  LoRaMacMacPara_t g_macData;
	
	APP_taskID = task_id;

	g_appData.devAddr = 0x00120560;
	LoRaMac_setAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
	LoRaMac_getAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
	APP_ShowMoteID(g_appData.devAddr);


	memset( &g_macData , 0 , sizeof(g_macData) );
	g_macData.channels[0].Frequency = 779500000;
	g_macData.channels[0].DrRange.Value = 0x50;
	g_macData.channels[0].Band = 0;

#if 0
	g_macData.channels[1].Frequency = 779500000;
	g_macData.channels[1].DrRange.Value = 0x50;
	g_macData.channels[1].Band = 0;

	g_macData.channels[2].Frequency = 779500000;
	g_macData.channels[2].DrRange.Value = 0x50;
	g_macData.channels[2].Band = 0;

	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS);
#endif

 	osal_set_event(APP_taskID,APP_PERIOD_SEND);
  //osal_set_event(APP_taskID,APP_TEST_UART);
}

u16 APP_ProcessEvent( u8 task_id, u16 events )
{
	loraMAC_msg_t* pMsgSend = NULL;
  loraMAC_msg_t* pMsgRecieve = NULL; 

	u8 tmp_buf[64];
	u8 len = 0 ;
	
	//system event
	if(events & SYS_EVENT_MSG)
	{ //receive msg loop
		while(NULL != (pMsgRecieve = (loraMAC_msg_t*)osal_msg_receive(APP_taskID)))
		{ //pMsgRecieve[0] is system event type
		  switch(pMsgRecieve->msgID)
		  	{
		  		//tx done
		  	  case TXDONE :
						  //调度下一次发送
				      // osal_start_timerEx(APP_taskID,APP_PERIOD_SEND, 5000);

							//send a packet to LoRaMac osal (then can be send by the radio)
							pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(72);
							if(pMsgSend != NULL)
							{
								osal_memset(pMsgSend,0,72);
								pMsgSend->msgID = TXREQUEST;
								pMsgSend->msgLen = 16;
								for(u8 dataCount = 0; dataCount < 16; dataCount++)
								{
									pMsgSend->msgData[dataCount] = dataCount;
								}
									osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
							}
							
              break;

					//rx done
					case RXDONE:
						len = 0 ;
						g_number++ ;

						//display some message on the oled
						memset( tmp_buf ,0 ,sizeof(tmp_buf) );
						memset( Rx_buf,0,sizeof(Rx_buf) );

						sprintf( Rx_buf,"RX:%d ",g_number);
						len += strlen( Rx_buf );
						sprintf( Rx_buf+len,"len:%d ",pMsgRecieve->msgLen);
						OLED_ShowString( 0,12, (u8*)Rx_buf,12 );

						memset(Rx_buf , 0 ,sizeof(Rx_buf));
						osal_memcpy(Rx_buf,"DATA:",5);

						osal_memcpy(&Rx_buf[5],pMsgRecieve->msgData,pMsgRecieve->msgLen);
						
						OLED_ShowString( 0,24, (u8*)Rx_buf,12 );
						OLED_Refresh_Gram();

				    break;

            default:
					    break;
		  	}

			osal_msg_deallocate((u8*)pMsgRecieve);
		}

		return (events ^ SYS_EVENT_MSG);

	}

	//send a packet event
	if(events & APP_PERIOD_SEND)
	{
		//send a packet to LoRaMac osal (then can be send by the radio)
		pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(72);
		if(pMsgSend != NULL)
		{
			osal_memset(pMsgSend,0,72);
			pMsgSend->msgID = TXREQUEST;
			pMsgSend->msgLen = 16;
			for(u8 dataCount = 0; dataCount < 16; dataCount++)
			{
				pMsgSend->msgData[dataCount] = dataCount;
			}
				osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
		}

		return (events ^ APP_PERIOD_SEND);
	}

	//uart test event
	if(events & APP_TEST_UART)
	{
		if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
		{
		
		}
		while (UartReady != SET)
		{
		}

		UartReady = RESET;

		if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
		{

		}
	
		return (events ^ APP_TEST_UART);
	}

	return 0 ;
}


//display NPLink mote ID on the OLED
void APP_ShowMoteID( u32 moteID )
{
	u8 	MoteIDString[32] ;
	u8* pIDString = MoteIDString;
	u32 ZeroNum = 0 ;

	//count the zero num in front of moteID string
	for(u8 i = 28; i > 0; i = i - 4)
	{
		if((moteID >> i ) % 16 == 0)
		{
			ZeroNum = ZeroNum + 1 ;
		}
		else
		{
			break;
		}
	}

	sprintf((char*)pIDString,"ID:");
	pIDString += 3;
	while(ZeroNum--)
	{
		sprintf((char*)pIDString,"0");
		pIDString++;
	}
	sprintf((char*)pIDString,"%x",moteID);
	
	OLED_ShowString( 0,0,MoteIDString,12 );
	OLED_Refresh_Gram();
}


u16 Onboard_rand(void)
{
	return 0; //return TIM_GetCounter(TIM5);
}

/******************* (C) COPYRIGHT 2015 NPLink *****END OF FILE****/

