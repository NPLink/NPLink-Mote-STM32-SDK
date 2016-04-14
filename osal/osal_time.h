/**************************************************************************************************
* Filename:             osal_time.h
* Revised:        
* Revision:       
* Description:    
**************************************************************************************************/
#ifndef osal_time_H
#define osal_time_H

#ifdef __cplusplus

extern "C"
{
  
#endif
  /*********************************************************************
  * INCLUDES
  */
 #include "stm32l051xx.h" 
	
  /*********************************************************************
  * MACROS
  */
  
  /*********************************************************************
  * CONSTANTS
  */
  
  /*********************************************************************
  * TYPEDEFS
  */
  
  /*********************************************************************
  * GLOBAL VARIABLES
  */
  
  /*********************************************************************
  * FUNCTIONS
  */
  
extern void osalTimerUpdate( u16 updateTime );  
extern void osalTimerInit( void );
extern u8 osal_start_timerEx( u8 taskID, u16 event_id, u16 timeout_value );
extern u8 osal_stop_timerEx( u8 task_id, u16 event_id );
extern u16 osal_get_timeoutEx( u8 task_id, u16 event_id );
extern u8 osal_timer_num_active( void );
extern u32 osal_GetSystemClock( void );
  /*********************************************************************
  *********************************************************************/
#ifdef __cplusplus
}
#endif

#endif 
