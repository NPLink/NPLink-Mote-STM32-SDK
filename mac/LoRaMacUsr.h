/*
(C)2015 NPLink

Description: LoRa MAC layer user application interface.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr

history: V1.0 Robxr create file and add maintainer functions
         		V1.1 zhangjh Modify functions parameters type, include 4 functions as flow:
              LoRaMac_setAppLayerParameter
				LoRaMac_getAppLayerParameter
				LoRaMac_setMacLayerParameter
				LoRaMac_getMacLayerParameter
			V1.2 :
			  1) XLH/Robxr Add mac mode set function and low power mode set function:
				LoRaMac_setMode
				LoRaMac_setlowPowerMode
			  2) add three MAC Layer parameters:
			   PARAMETER_PHY_FREQUENCY
			   PARAMETER_PHY_SPREADING_FACTOR
			   PARAMETER_PHY_MODULATION_MODE
*/

#ifndef __LORAMACUSR_H__
#define __LORAMACUSR_H__

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "oled_board.h"
#include <string.h>
#include "utilities.h"

#if defined(__CC_ARM) || defined(__GNUC__)
#define PACKED                                      __attribute__( ( __packed__ ) )
#elif defined( __ICCARM__ )
#define PACKED                                      __packed
#else
    #warning Not supported compiler type
#endif

/*Macros--------------------------------------------------------------------*/
//define work mode of mote (LoRaMac or PhyMac)
//定义MAC的工作模式，可以是LoRaMac(带协议)或phyMac(不带协议)
#define MODE_LORAMAC									0 
#define MODE_PHY											1

//define modulation mode of mote (LoRa or FSK)
//定义物理层芯片的调制方式，可以是LoRa调制或FSK调制
#define MODULATION_FSK								0 
#define MODULATION_LORA								1

//define return status
//定义MAC层处理状态
#define LORAMAC_USR_SUCCESS							0//成功
#define LORAMAC_USR_INVALID_PARAMETER		1//无效参数
#define LORAMAC_USR_FAILURE							0xFF//失败

//define parameter IDs of LoRaMac application layer
//定义应用层参数ID号
#define PARAMETER_DEV_ADDR 						(u32)(1 << 0)//设备地址(4B)
#define PARAMETER_APP_EUI							(u32)(1 << 1)//LoRaWAN AppEUI值
#define PARAMETER_APP_KEY							(u32)(1 << 2)//LoRaWAN AppKey，当使用over-the-air activation时使用
#define PARAMETER_NWK_SKEY						(u32)(1 << 3)//LoRaWAN NwkSkey，当activation by personalization时使用
#define PARAMETER_APP_SKEY						(u32)(1 << 4)//LoRaWAN AppSkey，当activation by personalization时使用

//define parameter IDs of LoRaMac mac layer
//定义MAC层参数ID号
#define PARAMETER_BANDS 								(u32)(1 << 0)//LoRaWAN MAC模式工作时使用的频点，当前支持1个频点
#define PARAMETER_CHANNELS							(u32)(1 << 1)//LoRaWAN MAC模式工作时，在频点上使用的信道，当前支持最多16个信道
#define PARAMETER_DATARATE							(u32)(1 << 2)//LoRaWAN MAC模式工作时，发送速率
#define PARAMETER_ADR_SWITCH						(u32)(1 << 3)//LoRaWAN MAC模式工作时，ADR使能或去使能
#define PARAMETER_PHY_FREQUENCY			    (u32)(1 << 4)//phy MAC模式工作时，工作频点
#define PARAMETER_PHY_SPREADING_FACTOR	(u32)(1 << 5)//phy MAC模式工作时，LORA调制方式下的扩频因子，有效值为7-12
#define PARAMETER_PHY_MODULATION_MODE		(u32)(1 << 6)//phy MAC模式工作时，调制方式的选择

//define length of parameters
//定义参数长度
#define APP_EUI_LEN									8
#define MOTE_DEV_ADDR_LEN						4
#define APP_KEY_LEN									16
#define NWK_SKEY_LEN								16
#define APP_SKEY_LEN								16

#define LORA_MAX_NB_BANDS 							1//only one band is supported currentlly.
#define LORA_MAX_NB_CHANNELS 						16//only 16 channels are supported currentlly.

/*!
 * LoRaMac TxPower definition
 */
#define TX_POWER_20_DBM                             0
#define TX_POWER_14_DBM                             1
#define TX_POWER_11_DBM                             2
#define TX_POWER_08_DBM                             3
#define TX_POWER_05_DBM                             4
#define TX_POWER_02_DBM                             5

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK

/* typedef -----------------------------------------------------------*/
typedef struct
{
    uint16_t DCycle;
    int8_t TxMaxPower;
    uint64_t LastTxDoneTime;
    uint64_t TimeOff;
} PACKED Band_t;

/*!
 * LoRaMAC channels parameters definition
 */
typedef union
{
    int8_t Value;
    struct
    {
        int8_t Min : 4;
        int8_t Max : 4;
    }PACKED Fields;
}PACKED DrRange_t;


typedef struct
{
    uint32_t Frequency; // Hz
    DrRange_t DrRange;  // Max datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
                        // Min datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
    uint8_t Band;       // Band index
}PACKED ChannelParams_t;

typedef struct LoRaMacMacPara
{
	Band_t bands[LORA_MAX_NB_BANDS];//LORA MAC的频点定义
	ChannelParams_t channels[LORA_MAX_NB_CHANNELS];//LORA MAC的信道定义
	u8 datarate;//LORA MAC的发送速率
	bool lora_mac_adr_switch ;//LORA MAC的ADR使能与否
	u32 phyFrequency;//phy MAC的频点定义
	u8  phySF;//phy MAC的LORA调制方式时的扩频因子 
	u8  phymodulation;//phy MAC的调制方式定义
}LoRaMacMacPara_t;

typedef struct LoRaMacAppPara
{
	u32 devAddr;//设备地址
	u8  appEUI[APP_EUI_LEN];//LoRaWAN AppEUI值
	u8 	appKey[APP_KEY_LEN];//LoRaWAN AppKey
	u8 	nwkSKey[NWK_SKEY_LEN];//LoRaWAN NwkSkey
	u8 	appSKey[APP_SKEY_LEN];//LoRaWAN AppSkey
}LoRaMacAppPara_t;

/* function prototypes -----------------------------------------------*/
/*!
 * LoRaMac_setAppLayerParameter -- 设置APP 层参数
 *
 * \param [IN]  pdata_in -- 指向参数数据区的指针，数据空间由调用者申请
 *                   parameterIDs -- 参数ID号，可使用位或方式同时设置多个参数，支持的ID号如下:
 							PARAMETER_DEV_ADDR
 							PARAMETER_APP_EUI
 							PARAMETER_APP_KEY
 							PARAMETER_NWK_SKEY
 							PARAMETER_APP_SKEY
 */
u8 LoRaMac_setAppLayerParameter( void* pdata_in, u32 parameterIDs);

/*!
 * LoRaMac_getAppLayerParameter -- 获取APP 层当前设置参数值
 *
 * \param [IN]  pdata_out -- 指向参数数据区的指针，数据空间由调用者申请
 *                   parameterIDs -- 参数ID号，可使用位或方式同时设置多个参数，支持的ID号如下:
 							PARAMETER_DEV_ADDR
 							PARAMETER_APP_EUI
 							PARAMETER_APP_KEY
 							PARAMETER_NWK_SKEY
 							PARAMETER_APP_SKEY
 */
u8 LoRaMac_getAppLayerParameter( void* pdata_out, u32 parameterIDs);

/*!
 * LoRaMac_setMacLayerParameter -- 设置MAC 层参数
 *
 * \param [IN]  pdata_in -- 指向参数数据区的指针，数据空间由调用者申请
 *                   parameterIDs -- 参数ID号，可使用位或方式同时设置多个参数，支持的ID号如下:
 							PARAMETER_BANDS
 							PARAMETER_CHANNELS
 							PARAMETER_ADR_SWITCH
 							PARAMETER_PHY_FREQUENCY
 							PARAMETER_PHY_SPREADING_FACTOR
 							PARAMETER_PHY_MODULATION_MODE
 */
u8 LoRaMac_setMacLayerParameter( void* pdata_in, u32 parameterIDs);

/*!
 * LoRaMac_getMacLayerParameter -- 获取MAC 层当前设置参数值
 *
 * \param [IN]  pdata_out -- 指向参数数据区的指针，数据空间由调用者申请
 *                   parameterIDs -- 参数ID号，可使用位或方式同时设置多个参数，支持的ID号如下:
 							PARAMETER_BANDS
 							PARAMETER_CHANNELS
 							PARAMETER_ADR_SWITCH
 							PARAMETER_PHY_FREQUENCY
 							PARAMETER_PHY_SPREADING_FACTOR
 							PARAMETER_PHY_MODULATION_MODE
 */
u8 LoRaMac_getMacLayerParameter( void* pdata_out, u32 parameterIDs);

/*!
 * LoRaMac_setMode -- 设置MAC层工作模式
 *
 * \param [IN]  mode -- 工作模式，取值为:
 											MODE_LORAMAC -- LORA MAC方式工作
 											MODE_PHY -- phy MAC方式工作
 *                   
 */
u8 LoRaMac_setMode(u8 mode);

/*!
 * LoRaMac_setlowPowerMode -- 设置radio部分低功耗,使能后，radio部分将关闭晶振，并进入sleep状态
 *
 * \param [IN]  enable -- 使能与否，取值为:
 							TRUE -- 使能低功耗
 							FALSE -- 去使能低功耗			
 *                   
 */
void LoRaMac_setlowPowerMode(u8 enable);


#endif // __LORAMAC_H__
