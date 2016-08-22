//------------------------------------------------------------------------
// $Id
//
// Copyright (C), 2015-, DS IOT Team Information Co.,Ltd,All Rights Reserved
//
// FileName: at.h
//
// Author: 
//
// Version: 
//
// Date: 
//
// Description: 主要用于配置设备
//
//
// Function List:
//
// History:
//--------------------------------------------------------------------------


#ifndef __AT_H__
#define __AT_H__

#include "uart_board.h"
#include <string.h>
#include <stdio.h>
#include <osal.h>

#define send_to_host(a,b )  HAL_UART_SendBytes(a, b)

/**
 * @brief parameter information
 */
typedef struct InsParam{
    uint8_t value[12];             /*!<param value*/
    uint32_t len;                  /*!<param length*/
}InsParam_t;

typedef struct lmote_config_at {
	uint8_t		tx_rx; 		
	int32_t 	freq_hz;		/*!> center frequ of the IF chain, relative to RF chain frequency */

}lmote_config_at_t ;

extern u8 g_at_set_tx;
extern lmote_config_at_t g_config_at_t;

void at_command( uint8_t * at_cmd , int32_t at_cmd_len );
#endif // __AT_H__

