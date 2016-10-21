/*
(C)2015 NPLink

Description: app task implementation，本APP例程可实现LORAMAC、PHYMAC、低功耗3个应用之间的切换
						用户应该根据需要进行逻辑修改
License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include "stm32l0xx_hal_rtc.h"
#include "stm32l0xx_hal_iwdg.h"
#include <string.h>
#include <stdio.h>

#include "osal_memory.h"
#include "oled_board.h"
#include "app_osal.h"
#include "loraMac_osal.h"
#include "LoRaMacUsr.h"

#include "radio.h"
#include "timer.h"
#include "uart_board.h"
#include "led_board.h"
#include "rtc_board.h"
#include "iwdg_board.h"
#include "sx1276.h"
#include "at.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
u8 APP_taskID;
u32 Rxpacket_count = 0 ;
u8 debugEnable = FALSE; 

LoRaMacAppPara_t g_appData;//定义APP 参数结构体
LoRaMacMacPara_t g_macData;//定义MAC参数结构体

/* variables -----------------------------------------------------------*/
extern UART_HandleTypeDef UartHandle;
extern u8 APP_taskID;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void APP_Init(u8 task_id)
{
	APP_taskID = task_id;

	//开启或关闭MAC层的串口打印信息
	#ifdef USE_DEBUG
	debugEnable = TRUE;
	#endif
	
  //设置发射功率
  g_appData.txPower = TX_POWER_20_DBM;
 	LoRaMac_setAppLayerParameter(&g_appData, PARAMETER_DEV_TXPOWER );
 	
	//显示Mote ID
	LoRaMac_getAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
	APP_ShowMoteID(g_appData.devAddr);

	memset( &g_macData , 0 , sizeof(g_macData) );
#if 1
#if defined( USE_BAND_433 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 433175000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 433375000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 433575000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 433775000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 434175000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 434375000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 434575000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 434775000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_470 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 470300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 470500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 470700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 470900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 471100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 471300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;	
	//设置信道7
	g_macData.channels[6].Frequency = 471500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 471700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_490 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 490300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 490500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 490700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 490900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 491100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 491300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;	
	//设置信道7
	g_macData.channels[6].Frequency = 491500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 491700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_780 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 779500000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 779700000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 779900000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 780100000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 786500000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;	
	//设置信道6
	g_macData.channels[5].Frequency = 786700000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 786900000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 787100000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_868 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 868100000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 868300000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 868500000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 868700000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 869100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
  //设置信道6
	g_macData.channels[5].Frequency = 869300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 869500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 869700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_US915 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 902300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 902500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 902700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 902900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 907300000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
  //设置信道6
	g_macData.channels[5].Frequency = 907500000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 907700000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 907900000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_AU915 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 915200000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 915400000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 915600000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 915800000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 916000000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 916200000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 916400000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 916600000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#endif	

#endif
#if 0
		//设置LORAMAC工作模式的参数(FSK调制)
		//设置信道1
		g_macData.channels[0].Frequency = 779700000;//频点
		g_macData.channels[0].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 ); //速率范围:((最高速率<<4) | (最低速率))
		g_macData.channels[0].Band = 0;
		//设置信道2
		g_macData.channels[1].Frequency = 779700000;
		g_macData.channels[1].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 );
		g_macData.channels[1].Band = 0;
		//设置信道3
		g_macData.channels[2].Frequency = 779700000;
		g_macData.channels[2].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 );
		g_macData.channels[2].Band = 0;
		//发送速率
		g_macData.datarate = DR_7;
		//ADR开启或关闭
		g_macData.lora_mac_adr_switch = FALSE;
		//设置FSK参数
		g_macData.fskDatarate= 200000;//200000;//FSK调制下的发送速率
		g_macData.fskFdev = 100000;//100000;//FSK调制下的频偏
		g_macData.fskBandwidth = 200000;//200000;//FSK调制下的接收带宽
		g_macData.fskAfcBandwidth = 249999;//249999;//FSK调制下的AFC带宽
		LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH \
																	| PARAMETER_DATARATE | PARAMETER_FSK_FDEV | PARAMETER_FSK_DATARATE \
																	| PARAMETER_FSK_BANDEIDTH | PARAMETER_FSK_AFC_BANDWIDTH );
		//设置使用LoRaMAC
		LoRaMac_setMode(MODE_LORAMAC);
#endif

#if 0

	//设置PHYMAC工作模式的参数(LoRa调制方式)
	g_macData.phyFrequency = 779700000;//频率(Hz)
	g_macData.phySF = 7; //扩频因子(7-12)
	g_macData.phyModulation = MODULATION_LORA;//调制方式(FSK or LORA)
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY | PARAMETER_PHY_SPREADING_FACTOR \
															| PARAMETER_PHY_MODULATION_MODE );	//设置使用PhyMAC
	LoRaMac_setMode(MODE_PHY);
#endif

#if 0

	//设置PHYMAC工作模式的参数(FSK调制方式)
	g_macData.phyFrequency = 779700000;//频率(Hz)
	g_macData.phyModulation = MODULATION_FSK;//调制方式(FSK or LORA)	
	g_macData.fskDatarate= 50000;//FSK调制下的发送速率
	g_macData.fskFdev = 25000;//FSK调制下的频偏
	g_macData.fskBandwidth = 50000;//FSK调制下的带宽
	g_macData.fskAfcBandwidth = 83333;//FSK调制下的AFC带宽
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY | PARAMETER_PHY_MODULATION_MODE \
															| PARAMETER_FSK_FDEV | PARAMETER_FSK_DATARATE | PARAMETER_FSK_BANDEIDTH \
															| PARAMETER_FSK_AFC_BANDWIDTH );
	//设置使用PhyMAC
	LoRaMac_setMode(MODE_PHY);
#endif

 	osal_set_event(APP_taskID,APP_PERIOD_SEND);//启动发包

}

u16 APP_ProcessEvent( u8 task_id, u16 events )
{
 loraMAC_msg_t* pMsgSend = NULL;
 loraMAC_msg_t* pMsgRecieve = NULL;
	
 u8 len = 0 ;
 static u16 txErrorNum = 0;
 static u16 txTimeoutNum = 0;
 u8 txErrorString[20];
 u8 Rx_buf[64]; //buffer for oled display

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
				#if 1//连续发包模式
					display_sx1276_tx_pac_parm( pMsgRecieve->frame_no );
					osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,5000);	
				#else//低功耗测试
					
					#ifdef USE_LOW_POWER_MODE
					RtcSetTimeout(20000000);
					LoRaMac_setlowPowerMode(TRUE);
					RtcEnterLowPowerStopMode();
					#endif
					LoRaMac_setMode(MODE_LORAMAC);
					osal_set_event(APP_taskID,APP_PERIOD_SEND);
					Radio.Sleep();
					
				#endif
	
					break;
				
				//发送错误，未找到空闲信道
				case TXERR_STATUS:
					txErrorNum++;
					sprintf((char*)txErrorString,"TxErr:%d,txTO:%d",txErrorNum,txTimeoutNum);
					OLED_Clear_Line(5,12);
					OLED_ShowString( 0,48, (u8*)txErrorString,12 );
					OLED_Refresh_Gram();
					osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,5000);
					break;

				
				//发送错误，发送超时
				case TXTIMEOUT:
					txTimeoutNum++;
					sprintf((char*)txErrorString,"TxErr:%d,txTO:%d",txErrorNum,txTimeoutNum);
					OLED_Clear_Line(5,12);
					OLED_ShowString( 0,48, (u8*)txErrorString,12 );
					OLED_Refresh_Gram();
					osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,5000);
					break;

			//rx done
			case RXDONE:
				
				HalLedSet (HAL_LED_2, HAL_LED_MODE_ON);
				display_sx1276_rx_pac_parm( pMsgRecieve->msgRxRssi,pMsgRecieve->msgRxSnr);//第3行显示接收参数
				OLED_Clear_Half();//先把屏幕下一半清空
				APP_ShowMoteID(g_appData.devAddr);
				len = 0 ;
				memset(Rx_buf , 0 ,sizeof(Rx_buf));                               
				osal_memcpy(Rx_buf,pMsgRecieve->msgData,pMsgRecieve->msgLen);
				len = pMsgRecieve->msgLen;
				Rx_buf[len] = 0;
				OLED_Clear_Line(4,12);//先清空数据，再显示
				OLED_Clear_Line(5,12);
				OLED_ShowString( 0,36, (u8*)Rx_buf,12 );//第4行显示数据内容
				OLED_Refresh_Gram();
			
				HalLedSet (HAL_LED_2, HAL_LED_MODE_OFF);
			
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
		//RedLED(OFF);
		 HalLedSet (HAL_LED_1, HAL_LED_MODE_OFF);
		//send a packet to LoRaMac osal (then can be send by the radio)
		pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(sizeof(loraMAC_msg_t));
		if(pMsgSend != NULL)
		{
			osal_memset(pMsgSend,0,sizeof(loraMAC_msg_t));
			pMsgSend->msgID = TXREQUEST;
			pMsgSend->msgLen = 8;
			for(u8 dataCount = 0; dataCount < 8; dataCount++)
			{
				pMsgSend->msgData[dataCount] = dataCount;
			}
				osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
		}

		#ifdef USE_DEBUG
		HAL_UART_SendBytes("app send start...\n", osal_strlen("app send start...\n"));
		#endif
	  //osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,300);//延时继续发送
		return (events ^ APP_PERIOD_SEND);
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

