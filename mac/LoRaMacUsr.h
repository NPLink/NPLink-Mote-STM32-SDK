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

//define return status
#define LORAMAC_USR_SUCCESS						0
#define LORAMAC_USR_INVALID_PARAMETER	1
#define LORAMAC_USR_FAILURE						0xFF

//define parameter IDs of LoRaMac application layer
#define PARAMETER_DEV_ADDR 			(u32)(1 << 0)
#define PARAMETER_APP_EUI				(u32)(1 << 1)
#define PARAMETER_APP_KEY				(u32)(1 << 2)
#define PARAMETER_NWK_SKEY			(u32)(1 << 3)
#define PARAMETER_APP_SKEY			(u32)(1 << 4)

//define parameter IDs of LoRaMac mac layer
#define PARAMETER_BANDS 				(u32)(1 << 0)
#define PARAMETER_CHANNELS			(u32)(1 << 1)
#define PARAMETER_ADR_SWITCH		(u32)(1 << 2)

//define length of parameters
#define APP_EUI_LEN						8
#define MOTE_DEV_ADDR_LEN			4
#define APP_KEY_LEN						16
#define NWK_SKEY_LEN					16
#define APP_SKEY_LEN					16

#define LORA_MAX_NB_BANDS 		1//only one band is supported currentlly.
#define LORA_MAX_NB_CHANNELS 	16//only 16 channels are supported currentlly.

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

#if defined(__CC_ARM) || defined(__GNUC__)
#define PACKED                                      __attribute__( ( __packed__ ) )
#elif defined( __ICCARM__ )
#define PACKED                                      __packed
#else
    #warning Not supported compiler type
#endif

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

/* typedef -----------------------------------------------------------*/
typedef struct LoRaMacMacPara
{
	Band_t bands[LORA_MAX_NB_BANDS];
	ChannelParams_t channels[LORA_MAX_NB_CHANNELS];
    bool lora_mac_adr_switch ;

}LoRaMacMacPara_t;

typedef struct LoRaMacAppPara
{
	u32 devAddr;
	u8  appEUI[APP_EUI_LEN];
	u8 	appKey[APP_KEY_LEN];
	u8 	nwkSKey[NWK_SKEY_LEN];
	u8 	appSKey[APP_SKEY_LEN];
}LoRaMacAppPara_t;

/* function prototypes -----------------------------------------------*/
u8 LoRaMac_setAppLayerParameter( void* pdata_in, u32 parameterIDs);
u8 LoRaMac_getAppLayerParameter( void* pdata_out, u32 parameterIDs);
u8 LoRaMac_setMacLayerParameter( void* pdata_in, u32 parameterIDs);
u8 LoRaMac_getMacLayerParameter( void* pdata_out, u32 parameterIDs);

#endif // __LORAMAC_H__
