
/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "osal_time.h"
#include "osal_memory.h"
#include "osal_tick.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  void *next;
  u16 timeout;
  u16 event_flag;
  u8 task_id;
} osalTimerRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

osalTimerRec_t *timerHead;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
static u32  osal_systemClock;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
osalTimerRec_t  *osalAddTimer( u8 task_id, u16 event_flag, u16 timeout );
osalTimerRec_t *osalFindTimer( u8 task_id, u16 event_flag );
void osalDeleteTimer( osalTimerRec_t *rmTimer );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalTimerInit
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void osalTimerInit( void )
{
  osal_systemClock = 0;
  //SysTick_Configuration();
}

/*********************************************************************
 * @fn      osalAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  osalTimerRec_t * - pointer to newly created timer
 */
osalTimerRec_t * osalAddTimer( u8 task_id, u16 event_flag, u16 timeout )
{
  osalTimerRec_t *newTimer;
  osalTimerRec_t *srchTimer;

  // Look for an existing timer first
  newTimer = osalFindTimer( task_id, event_flag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->timeout = timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = osal_mem_alloc( sizeof( osalTimerRec_t ) );

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->task_id = task_id;
      newTimer->event_flag = event_flag;
      newTimer->timeout = timeout;
      newTimer->next = (void *)NULL;

      // Does the timer list already exist
      if ( timerHead == NULL )
      {
        // Start task list
        timerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = timerHead;

        // Stop at the last record
        while ( srchTimer->next )
          srchTimer = srchTimer->next;

        // Add to the list
        srchTimer->next = newTimer;
      }

      return ( newTimer );
    }
    else
      return ( (osalTimerRec_t *)NULL );
  }
}

/*********************************************************************
 * @fn      osalFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  osalTimerRec_t *
 */
osalTimerRec_t *osalFindTimer( u8 task_id, u16 event_flag )
{
  osalTimerRec_t *srchTimer;

  // Head of the timer list
  srchTimer = timerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->event_flag == event_flag &&
         srchTimer->task_id == task_id )
      break;

    // Not this one, check another
    srchTimer = srchTimer->next;
  }

  return ( srchTimer );
}

/*********************************************************************
 * @fn      osalDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void osalDeleteTimer( osalTimerRec_t *rmTimer )
{
  // Does the timer list really exist
  if ( rmTimer )
  {
    // Clear the event flag and osalTimerUpdate() will delete 
    // the timer from the list.
    rmTimer->event_flag = 0;
  }
}

/*********************************************************************
 * @fn      osal_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   u8 taskID - task id to set timer for
 * @param   u16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
u8 osal_start_timerEx( u8 taskID, u16 event_id, u16 timeout_value )
{
  halIntState_t intState;
  osalTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Add timer
  newTimer = osalAddTimer( taskID, event_id, timeout_value );

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      osal_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If ZSUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   u8 task_id - task id of timer to stop
 * @param   u16 event_id - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or INVALID_EVENT_ID
 */
u8 osal_stop_timerEx( u8 task_id, u16 event_id )
{
  halIntState_t intState;
  osalTimerRec_t *foundTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Find the timer to stop
  foundTimer = osalFindTimer( task_id, event_id );
  if ( foundTimer )
  {
    osalDeleteTimer( foundTimer );
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (foundTimer != NULL) ? SUCCESS : INVALID_EVENT_ID );
}

/*********************************************************************
 * @fn      osal_get_timeoutEx
 *
 * @brief
 *
 * @param   u8 task_id - task id of timer to check
 * @param   u16 event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
u16 osal_get_timeoutEx( u8 task_id, u16 event_id )
{
  halIntState_t intState;
  u16 rtrn = 0;
  osalTimerRec_t *tmr;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  tmr = osalFindTimer( task_id, event_id );

  if ( tmr )
  {
    rtrn = tmr->timeout;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return rtrn;
}

/*********************************************************************
 * @fn      osal_timer_num_active
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  u8 - number of timers
 */
u8 osal_timer_num_active( void )
{
  halIntState_t intState;
  u8 num_timers = 0;
  osalTimerRec_t *srchTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Head of the timer list
  srchTimer = timerHead;

  // Count timers in the list
  while ( srchTimer != NULL )
  {
    num_timers++;
    srchTimer = srchTimer->next;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return num_timers;
}

/*********************************************************************
 * @fn      osalTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osalTimerUpdate( u16 updateTime )
{
  halIntState_t intState;
  osalTimerRec_t *srchTimer;
  osalTimerRec_t *prevTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
  // Update the system time
  osal_systemClock += updateTime;
  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  // Look for open timer slot
  if ( timerHead != NULL )
  {
    // Add it to the end of the timer list
    srchTimer = timerHead;
    prevTimer = (void *)NULL;

    // Look for open timer slot
    while ( srchTimer )
    {
      osalTimerRec_t *freeTimer = NULL;
     
      HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
      
      if (srchTimer->timeout <= updateTime)
      {
        srchTimer->timeout = 0;
      }
      else
      {
        srchTimer->timeout = srchTimer->timeout - updateTime;
      }
      
      // When timeout or delete (event_flag == 0)
      if ( srchTimer->timeout == 0 || srchTimer->event_flag == 0 )
      {
        // Take out of list
        if ( prevTimer == NULL )
          timerHead = srchTimer->next;
        else
          prevTimer->next = srchTimer->next;

        // Setup to free memory
        freeTimer = srchTimer;

        // Next
        srchTimer = srchTimer->next;
      }
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->next;
      }
      
      HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.
      
      if ( freeTimer )
      {
        if ( freeTimer->timeout == 0 )
        {
          osal_set_event( freeTimer->task_id, freeTimer->event_flag );
        }
        osal_mem_free( freeTimer );
      }
    }
  }
}

/*********************************************************************
 * @fn      osal_GetSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
u32 osal_GetSystemClock( void )
{
  return ( osal_systemClock );
}

/*********************************************************************
*********************************************************************/
