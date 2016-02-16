/**************************************************************************************************
* Filename:             osal.h
* Revised:        
* Revision:       
* Description:    
**************************************************************************************************/
#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus

extern "C"
{
  
#endif
/*********************************************************************
* INCLUDES
*/
  
#include "stm32l0xx.h"
//#include "stm32f4xx.h"

#include "osal_mutex.h"
#include "osal_memory.h"
#include "osal_time.h"
/*********************************************************************
* MACROS
*/
#define TASK_NO_TASK      0xFF
  
/*** Generic Status Return Values ***/    //´ý¸Ä
#define SUCCESS                   0x00
#define FAILURE                   0x01
#define INVALIDPARAMETER          0x02
#define INVALID_TASK              0x03
#define MSG_BUFFER_NOT_AVAIL      0x04
#define INVALID_MSG_POINTER       0x05
#define INVALID_EVENT_ID          0x06
#define INVALID_INTERRUPT_ID      0x07
#define NO_TIMER_AVAIL            0x08
#define NV_ITEM_UNINIT            0x09
#define NV_OPER_FAILED            0x0A
#define INVALID_MEM_SIZE          0x0B
#define NV_BAD_ITEM_LEN           0x0C
  
#define OSAL_MSG_ID(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->dest_id
#define st(x)      do { x } while (__LINE__ == -1)
  
  
/*********************************************************************
* CONSTANTS
*/
#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

#ifndef CONST
  #define CONST const
#endif

#ifndef GENERIC
  #define GENERIC
#endif
//////////////////////  
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif
  
#define BUILD_u32(Byte0, Byte1, Byte2, Byte3) \
          ((u32)((u32)((Byte0) & 0x00FF) \
          + ((u32)((Byte1) & 0x00FF) << 8) \
          + ((u32)((Byte2) & 0x00FF) << 16) \
          + ((u32)((Byte3) & 0x00FF) << 24)))

#define BUILD_u16(loByte, hiByte) \
          ((u16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->next

#define HAL_ENABLE_INTERRUPTS()         st( __enable_irq(); )
#define HAL_DISABLE_INTERRUPTS()        st( __disable_irq(); )
//#define HAL_INTERRUPTS_ARE_ENABLED()    ()
#define HAL_ENTER_CRITICAL_SECTION(x)   st( x = __get_PRIMASK();  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( __set_PRIMASK(x);HAL_ENABLE_INTERRUPTS(); )
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )
/*** Task Synchronization  ***/

  /*
   * Set a Task Event
   */  
#define SYS_EVENT_MSG               0x8000  // A message is waiting event
#define INTS_ALL    0xFF
/*********************************************************************
* TYPEDEFS
*/

/////////////////////////
typedef u32           halDataAlign_t;
///////////////////////////
typedef struct
{
  void   *next;
  u16 len;
  u8  dest_id;
} osal_msg_hdr_t;

typedef struct
{
  u8  event;
  u8  status;
} osal_event_hdr_t;

typedef void * osal_msg_q_t;

typedef u32 halIntState_t;
//////////
typedef u16 (*pTaskEventHandlerFn)( u8 task_id, u16 event );
/*********************************************************************
* GLOBAL VARIABLES
*/
extern const pTaskEventHandlerFn tasksArr[];
extern const u8 tasksCnt;
extern u16 *tasksEvents;
/*********************************************************************
* FUNCTIONS
*/
extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );
extern u8 osal_set_event( u8 task_id, u16 event_flag );
extern int osal_strlen( char *pString );
extern void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len );
extern void *osal_revmemcpy( void *dst, const void GENERIC *src, unsigned int len );
extern void *osal_memdup( const void GENERIC *src, unsigned int len );
extern u8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );
extern void *osal_memset( void *dest, u8 value, int len );
extern u16 osal_build_u16( u8 *swapped );
extern u32 osal_build_u32( u8 *swapped, u8 len );
extern u16 osal_rand( void );
extern u8 * osal_msg_allocate( u16 len );
extern u8 osal_msg_deallocate( u8 *msg_ptr );
extern u8 osal_msg_send( u8 destination_task, u8 *msg_ptr );
extern u8 *osal_msg_receive( u8 task_id );
extern osal_event_hdr_t *osal_msg_find(u8 task_id, u8 event);
extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );
extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );
extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );
extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );
extern u8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, u8 max );
extern u8 osal_clear_event( u8 task_id, u16 event_flag );
extern u8 osal_int_enable( u8 interrupt_id );
extern u8 osal_int_disable( u8 interrupt_id );
extern u8 osal_init_system( void );
extern void osal_start_system( void );
extern void osalTimeUpdate( void );
/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif 
