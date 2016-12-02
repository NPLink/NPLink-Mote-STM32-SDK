//------------------------------------------------------------------------
// $Id
//
// Copyright (C), 2015-, DS IOT Team Information Co.,Ltd,All Rights Reserved
//
// FileName: at.c
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

#include <string.h>
#include "LoRaMacUsr.h"
#include "LoRaMac_osal.h"
#include "uart_board.h"
#include "app_osal.h"
#include "board.h"
#include "error.h"
#include "at.h"
#include "osal.h"

#define AT_ERROR  ("ERROR\n\r")
#define AT_OK     ("OK\n\r")

lmote_config_at_t g_config_at_t ;
u8 g_at_set_tx = 1;
u8 g_at_set_diffrq = 0;

int32_t get_ins_param( uint8_t *src_data ,  uint32_t len , InsParam_t *ins_param )
{
    int32_t i,j,n,param_num;

    if( (NULL == src_data) || (NULL ==ins_param ) || (len<=0) )
    {
       return ERR_PARAMETER ;
    }
    param_num = 0 ;
    for( i=0,j=0,n=0 ; i<len ; i++ )
    {
        //0x2c -- ','
        if( 0x2C == src_data[i] )
        {
            j++;
            n = 0 ;
            param_num++ ;
        }
        else
        {
            ins_param[j].value[n++] = src_data[i] ;
            ins_param[j].len++ ;
        }
    }

    param_num++ ; //以','判断参数个数，需要加1才是实际参数个数

    return param_num ;
}

int32_t get_ins_ats_action( uint8_t *src_data,uint8_t *action1,uint8_t *action2 )
{
    int32_t i,j;
		uint8_t param[8];
    uint8_t len ;

    if( NULL == src_data )
    {
     	return ERR_PARAMETER ;
    }
		memset( param , 0 ,sizeof(param) );
    len = 5 ; //ATSn=X,故i的初始值为3且n的长度为1,故len=5;
    for( i=3, j=0 ; i<len ; i++ )
    {
        if( 0x3D == src_data[i] )
        {
          *action1 = 0x3D ;// =
          break;
        }
        else if( 0x3F == src_data[i] )
        {
          *action1 = 0x3F ; //?
          break;
        }
				param[j++] = src_data[i];
    }
		
		*action2 = atoi((char *)param );
		
    return ERR_SUCCESS ;
}

void at_s( uint8_t *at_cmd , int32_t at_cmd_len )
{
		int32_t		ret;
		static uint8_t		idx;
		static uint8_t    ats_action1,ats_action2 ;
		uint8_t ack_buf[64];
		InsParam_t ins_param_at[8] ;
		static uint8_t param_num = 0  ;

		memset( ack_buf , 0 ,sizeof(ack_buf) );

		ret = get_ins_ats_action( at_cmd , &ats_action1, &ats_action2 );
		if( ERR_SUCCESS != ret )
						return ;
		switch( ats_action1 )
		{
		case '?':
						break;

		case '=':
					switch ( ats_action2 )
					{
					 case 0x00:
						  memset( ins_param_at , 0 ,sizeof(ins_param_at) );
	            idx = 5 ;
	            param_num = get_ins_param( &at_cmd[idx],at_cmd_len-idx-2, ins_param_at );//2//--表示0d,0a
	            if( 1 != param_num )
	            {
	                send_to_host(AT_ERROR ,strlen(AT_ERROR) );
	                break;
	            }
	            g_config_at_t.banud = atoi( (char*)&ins_param_at[0].value[0] );
	            HAL_UART_BandRateChange( g_config_at_t.banud);
						 break;
							 
						case 0x01:
							memset( ins_param_at , 0 ,sizeof(ins_param_at) );
							idx = 5;
							param_num = get_ins_param( &at_cmd[idx],at_cmd_len-idx-2, ins_param_at );
							if( 4 != param_num )
							{
									send_to_host( AT_ERROR ,strlen(AT_ERROR) );
									break;
							}
							g_config_at_t.tx_rx = ins_param_at[0].value[0];
							g_config_at_t.phy_modulation= atoi((char *)&ins_param_at[1].value[0]);
							g_config_at_t.phy_freq = atoi((char *)&ins_param_at[2].value[0]);
							g_config_at_t.phy_sf = atoi((char *)&ins_param_at[3].value[0]);
							
							if( g_config_at_t.tx_rx == 'S' )
							{		
									g_at_set_tx = true;
									Tx_dutycycle = 1000;
									g_macData.phySF = g_config_at_t.phy_sf;
									g_macData.phyFrequency = g_config_at_t.phy_freq;	
									g_macData.phyModulation = g_config_at_t.phy_modulation;
									LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY|PARAMETER_PHY_SPREADING_FACTOR|PARAMETER_PHY_MODULATION_MODE);
									
									LoRaMac_setMode(MODE_PHY);
									osal_set_event(APP_taskID,APP_PERIOD_SEND);
									
							}
							else if( g_config_at_t.tx_rx == 'R' )
							{

									g_at_set_tx = false;	
									
									g_macData.phySF = g_config_at_t.phy_sf;
									g_macData.phyFrequency = g_config_at_t.phy_freq;	
									g_macData.phyModulation = g_config_at_t.phy_modulation;
									LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY|PARAMETER_PHY_SPREADING_FACTOR|PARAMETER_PHY_MODULATION_MODE);
									
									LoRaMac_setMode(MODE_PHY);
									
							}
							break;
							
						case 0x02:
						  memset( ins_param_at , 0 ,sizeof(ins_param_at) );
	            idx = 5 ;
	            param_num = get_ins_param( &at_cmd[idx],at_cmd_len-idx-2, ins_param_at );//2//--表示0d,0a
	            if( 2 != param_num )
	            {
	               send_to_host( AT_ERROR ,strlen(AT_ERROR) );
	               break;
	            }
	            g_at_set_diffrq = atoi( (char*)&ins_param_at[0].value[0] );
							
							if( g_at_set_diffrq == 1 )
							{
								LoRa_SetDifFrq( atoi( (char*)&ins_param_at[1].value[0] ) );
							}
	            
						 break;

					default:
							break;
					}
						
					break;
		default:
			break;
		}
}

void at_i( uint8_t *at_cmd )
{
    switch (at_cmd[3]) {
	case '\0':
	case '0':
		break;
		
	case '1':
		
		system_reset = 1;
		break;
		
	case '2':
		
		Tx_dutycycle = 1000;
		g_macData.phySF = 7;
		g_macData.phyModulation = MODULATION_LORA;
		LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_MODULATION_MODE|PARAMETER_PHY_SPREADING_FACTOR);
		LoRaMac_setMode(MODE_PHY);
		osal_set_event(APP_taskID,APP_PERIOD_SEND);
		break;
		
	case '3':
		
		Tx_dutycycle = 1000;
		g_macData.phyModulation = MODULATION_FSK;
		LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_MODULATION_MODE);
		LoRaMac_setMode(MODE_PHY);
		osal_set_event(APP_taskID,APP_PERIOD_SEND);
		break;

	default:
		break;
	}

}

void at_command( uint8_t * at_cmd , int32_t at_cmd_len )
{
    if (at_cmd_len >= 2)
    {
        // look at the next byte to determine what to do
        switch (at_cmd[2])
        {
        case '\0':      // no command -> OK
                //at_ok();
                break;

        case 'S':
                at_s( at_cmd,at_cmd_len );
                break;

				case 'I':
                at_i( at_cmd );
                break;

        default:
                break;
        }
    }

    //return ret ;
}

