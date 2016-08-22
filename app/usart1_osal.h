//------------------------------------------------------------------------
// $Id
//
// Copyright (C), 2015-, IOT Team Information Co.,Ltd,All Rights Reserved
//
// FileName: usart1_osal.h
//
// Author: 
//
// Version: 1.0
//
// Date: 
//
// Description: 
//
//
// Function List:
//
// History:
//--------------------------------------------------------------------------
#include "uart_board.h"

#ifndef USART1_OSAL_H
#define USART1_OSAL_H

/**
*@Description: gateway主机监控串口1数据任务初始化
*@param: [in]: task_id -- 任务ID
*@      [out]: NONE
*@return: NONE
*@others: Parameter validation checking performs in the caller
*/
void Usart1_Init(u8 task_id);

/**
*@Description: gateway主机监控串口1数据任务,并进行相应处理
*@param: [in]: task_id -- 任务ID
*@             events -- 任务队列中的事件
*@      [out]: NONE
*@return: NONE
*@others: Parameter validation checking performs in the caller
*/
u16 Usart1_ProcessEvent( u8 task_id, u16 events );

/**
*@Description: 接收数据的处理函数
*@param: [in]: data -- 指向接收数据的指针
*@             data_len -- 接收数据的长度
*@      [out]: NONE
*@return: NONE
*@others: Parameter validation checking performs in the caller
*/
void deal_receive_date( uint8_t * data , int32_t data_len );

#endif

