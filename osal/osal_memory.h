/**************************************************************************************************
* Filename:             osal_Memory.h
* Revised:        
* Revision:       
* Description:    
**************************************************************************************************/
#ifndef osal_Memory_H
#define osal_Memory_H

#ifdef __cplusplus

extern "C"
{
  
#endif
/*********************************************************************
* INCLUDES
*/

/*********************************************************************
* MACROS
*/

/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
*/
#define OSALMEM_METRICS 1

/*********************************************************************
* GLOBAL VARIABLES
*/

/*********************************************************************
* FUNCTIONS
*/
 /*
  * Allocate a block of memory.
  */
void osal_mem_init( void );
void osal_mem_kick( void );
extern void *osal_mem_alloc( u16 size );
void osal_mem_free( void *ptr );
u16 osal_heap_block_max( void );
u16 osal_heap_block_cnt( void );
u16 osal_heap_block_free( void );
u16 osal_heap_mem_used( void );
u16 osal_heap_mem_max( void );
/*********************************************************************
*********************************************************************/
#ifdef __cplusplus
}
#endif

#endif 
