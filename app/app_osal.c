/*
(C)2015 NPLink

Description: app task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: 
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

/* Private variables ---------------------------------------------------------*/
u8 APP_taskID;
char Rx_buf[64]; //buffer for oled display
u32 Rxpacket_count = 0 ;
u8* RecieveBuff_flag = NULL;
u8  Txpacket_count = 0 ;
//for uart test
__IO ITStatus UartReady = RESET;
u8 aTxBuffer[] = "uart test, hello!\n";
u8 aRxBuffer[RXBUFFERSIZE];
u8 g_number = 0;

extern UART_HandleTypeDef UartHandle;

LoRaMacAppPara_t g_appData;

/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
void APP_Init(u8 task_id)
{
	//LoRaMacAppPara_t appData;

	//LoRaMacMacPara_t macData;

	//LoRaMacAppPara_t g_appData;
    //LoRaMacMacPara_t g_macData;

	APP_taskID = task_id;

	g_appData.devAddr = 0x00120560;
	LoRaMac_setAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
	LoRaMac_getAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
	APP_ShowMoteID(g_appData.devAddr);


	//memset( &g_macData , 0 , sizeof(g_macData) );
	//g_macData.channels[0].Frequency = 779500000;
	//g_macData.channels[0].DrRange.Value = 0x50;
	//g_macData.channels[0].Band = 0;

#if 0
	g_macData.channels[1].Frequency = 779500000;
	g_macData.channels[1].DrRange.Value = 0x50;
	g_macData.channels[1].Band = 0;

	g_macData.channels[2].Frequency = 779500000;
	g_macData.channels[2].DrRange.Value = 0x50;
	g_macData.channels[2].Band = 0;

	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS);
#endif

	//RedLED(OFF);
	//GreenLED(OFF);
	HalLedSet (HAL_LED_ALL, HAL_LED_MODE_OFF);

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
        {
					//receive msg loop
					while(NULL != (pMsgRecieve = (loraMAC_msg_t*)osal_msg_receive(APP_taskID)))
					{
							//pMsgRecieve[0] is system event type
							switch(pMsgRecieve->msgID)
							{
							//tx done
							case TXDONE :
									//调度下一次发送
									//RedLED(ON);
									HalLedSet (HAL_LED_1, HAL_LED_MODE_ON);
									if(Txpacket_count > 0)
									{
										Txpacket_count--;
										HAL_UART_SendBytes(uart1_rxBuf,uart1_Rxcount);
										pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(72);
										if(pMsgSend != NULL)
										{
											osal_memset(pMsgSend,0,72);//需要全部置0，要不然会出现发完串口数据包后不再进入TXDONE
											pMsgSend->msgID = TXREQUEST;
											pMsgSend->msgLen = uart1_Rxcount+2;
											osal_memcpy(pMsgSend->msgData,uart1_rxBuf,uart1_Rxcount);
											osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
											osal_msg_deallocate((u8*)pMsgSend);
										}
									}
									else
									{
										//send a packet to LoRaMac osal (then can be send by the radio)
										pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(72);
										if(pMsgSend != NULL)
										{
											osal_memset(pMsgSend,0,72);
											pMsgSend->msgID = TXREQUEST;
											pMsgSend->msgLen = 70;
											for(u8 dataCount = 0; dataCount < 70; dataCount++)
											{
												pMsgSend->msgData[dataCount] = dataCount;
											}
											osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
											osal_msg_deallocate((u8*)pMsgSend);

											HAL_UART_SendBytes("app send\n", osal_strlen("app send\n"));
										}
									}
        							//RedLED(OFF);
        							HalLedSet (HAL_LED_1, HAL_LED_MODE_OFF);
        							break;
        					//rx done
        					case RXDONE:
                          
                                HalLedSet (HAL_LED_2, HAL_LED_MODE_ON);
                                OLED_Clear_Half();//先把屏幕下一半清空
                                APP_ShowMoteID(g_appData.devAddr);
                                len = 0 ;
                                g_number++ ;
                                memset(Rx_buf , 0 ,sizeof(Rx_buf));                               
                                osal_memcpy(Rx_buf,pMsgRecieve->msgData,pMsgRecieve->msgLen);
				    len = pMsgRecieve->msgLen;
				    Rx_buf[len] = 0;
                                OLED_ShowString( 0,36, (u8*)Rx_buf,12 );
                                OLED_Refresh_Gram();
                                HAL_UART_SendBytes("\n",1);
                                HAL_UART_SendBytes((uint8_t *)Rx_buf,strlen(Rx_buf));
				     HalLedSet (HAL_LED_2, HAL_LED_MODE_OFF);

                                break;

                             case TXERR_STATUS:
                             {
                                //TODO MOTE send packet error deal
                                memset( tmp_buf ,0 ,sizeof(tmp_buf) );
                                sprintf( (char *)tmp_buf,"send err ret=%d,no=%d",pMsgRecieve->msgData[0],
                                                                                 pMsgRecieve->msgData[1]+( pMsgRecieve->msgData[2]<<8 ) );
                                OLED_ShowString( 0,36,tmp_buf,12 );
                                OLED_Refresh_Gram();
                                break;
                             }


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
		//RedLED(OFF);
		 HalLedSet (HAL_LED_1, HAL_LED_MODE_OFF);
		//send a packet to LoRaMac osal (then can be send by the radio)
		pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(72);
		if(pMsgSend != NULL)
		{
			osal_memset(pMsgSend,0,72);
			pMsgSend->msgID = TXREQUEST;
			pMsgSend->msgLen = 70;
			for(u8 dataCount = 0; dataCount < 70; dataCount++)
			{
				pMsgSend->msgData[dataCount] = dataCount;
			}
				osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
		}

	  //osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,1000);//延时继续发送

		return (events ^ APP_PERIOD_SEND);
	}

	//uart test event
	if(events & APP_TEST_UART)
	{

		 Txpacket_count = 1;//串口收到一个数据包，就发一遍无线包出去add by hxz
		//HAL_UART_SendBytes("hello,world!", 10);
		//osal_start_timerEx(APP_taskID, APP_TEST_UART,1000);//延时继续发送
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

