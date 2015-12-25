
/*********************************************************************
 * INCLUDES
 */
#include "osal.h"
#include "osal_memory.h"

// Minimum wasted bytes to justify splitting a block before allocation.
#if !defined ( OSALMEM_MIN_BLKSZ )
  #define OSALMEM_MIN_BLKSZ    4
#endif

/* Profiling memory allocations showed that a significant % of very high
 * frequency allocations/frees are for block sizes less than or equal to 16.
 */
#if !defined ( OSALMEM_SMALL_BLKSZ )
  #define OSALMEM_SMALL_BLKSZ  64 //64
#endif


#if ( OSALMEM_PROFILER )
  #define OSALMEM_INIT   'X'
  #define OSALMEM_ALOC   'A'
  #define OSALMEM_REIN   'F'
#endif

#define MAXMEMHEAP   1*1024 //14*1024


/*********************************************************************
 * TYPEDEFS
 */

typedef u32  osalMemHdr_t;

/*********************************************************************
 * CONSTANTS
 */

#define OSALMEM_IN_USE  0x80000000

/* This number sets the size of the small-block bucket. Although profiling
 * shows max simultaneous alloc of 16x18, timing without profiling overhead
 * shows that the best worst case is achieved with the following.
 */
#define SMALLBLKHEAP    128 //4*1024

// To maintain data alignment of the pointer returned, reserve the greater
// space for the memory block header.
#define HDRSZ  ( (sizeof ( halDataAlign_t ) > sizeof( osalMemHdr_t )) ? \
                  sizeof ( halDataAlign_t ) : sizeof( osalMemHdr_t ) )

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static osalMemHdr_t *ff1;  // First free block in the small-block bucket.
static osalMemHdr_t *ff2;  // First free block after the small-block bucket.

#if ( OSALMEM_METRICS )
  static u16 blkMax;  // Max cnt of all blocks ever seen at once.
  static u16 blkCnt;  // Current cnt of all blocks.
  static u16 blkFree; // Current cnt of free blocks.
  static u16 memAlo;  // Current total memory allocated.
  static u16 memMax;  // Max total memory ever allocated at once.
#endif

// Memory Allocation Heap.

  static halDataAlign_t  _theHeap[ MAXMEMHEAP / sizeof( halDataAlign_t ) ];
  static u8 *theHeap = (u8 *)_theHeap;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      osal_mem_init
 *
 * @brief   Initialize the heap memory management system.
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_init( void )
{
  osalMemHdr_t *tmp;
  unsigned int i;
  for(i=0;i<MAXMEMHEAP;i++)
  {
    *(theHeap+i)= 0x00;
  }

  // Setup a NULL block at the end of the heap for fast comparisons with zero.
  tmp = (osalMemHdr_t *)theHeap + (MAXMEMHEAP / HDRSZ) - 1;
  *tmp = 0;

  // Setup a small-block bucket.
  tmp = (osalMemHdr_t *)theHeap;
  *tmp = SMALLBLKHEAP;

  // Setup the wilderness.
  tmp = (osalMemHdr_t *)theHeap + (SMALLBLKHEAP / HDRSZ);
  *tmp = ((MAXMEMHEAP / HDRSZ) * HDRSZ) - SMALLBLKHEAP - HDRSZ;

  // Setup a NULL block that is never freed so that the small-block bucket
  // is never coalesced with the wilderness.
  ff1 = tmp;
  ff2 = osal_mem_alloc( 0 );
  ff1 = (osalMemHdr_t *)theHeap;
#if ( OSALMEM_METRICS )
  /* Start with the small-block bucket and the wilderness - don't count the
   * end-of-heap NULL block nor the end-of-small-block NULL block.
   */
  blkCnt = blkFree = 2;
#endif
}

/*********************************************************************
 * @fn      osal_mem_kick
 *
 * @brief   Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *          Invoke this once after all long-lived blocks have been allocated -
 *          presently at the end of osal_init_system().
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_kick( void )
{
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  /* Logic in osal_mem_free() will ratchet ff1 back down to the first free
   * block in the small-block bucket.
   */
  ff1 = ff2;

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
}

/*********************************************************************
 * @fn      osal_mem_alloc
 *
 * @brief   Implementation of the allocator functionality.
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *osal_mem_alloc( u16 size )
{
  osalMemHdr_t *prev = NULL;
  osalMemHdr_t *hdr;
  halIntState_t intState;
  u32 tmp;
  u8 coal = 0;
  size += HDRSZ;

  // Calculate required bytes to add to 'size' to align to halDataAlign_t.
  if ( sizeof( halDataAlign_t ) == 2 )
  {
    size += (size & 0x01);
  }
  else if ( sizeof( halDataAlign_t ) != 1 )
  {
    const u8 mod = size % sizeof( halDataAlign_t );

    if ( mod != 0 )
    {
      size += (sizeof( halDataAlign_t ) - mod);
    }
  }

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Smaller allocations are first attempted in the small-block bucket.
  if ( size <= OSALMEM_SMALL_BLKSZ )
  {
    hdr = ff1;
  }
  else
  {
    hdr = ff2;
  }
  tmp = *hdr;

  do
  {
    if ( tmp & OSALMEM_IN_USE )
    {
      tmp ^= OSALMEM_IN_USE;
      coal = 0;
    }
    else
    {
      if ( coal != 0 )
      {
#if ( OSALMEM_METRICS )
        blkCnt--;
        blkFree--;
#endif
        *prev += *hdr;

        if ( *prev >= size )
        {
          hdr = prev;
          tmp = *hdr;
          break;
        }
      }
      else
      {
        if ( tmp >= size )
        {
          break;
        }

        coal = 1;
        prev = hdr;
      }
    }

    hdr = (osalMemHdr_t *)((u8 *)hdr + tmp);

    tmp = *hdr;
    if ( tmp == 0 )
    {
      hdr = NULL;
      break;
    }

  } while ( 1 );

  if ( hdr != NULL )
  {
    tmp -= size;

    // Determine whether the threshold for splitting is met.
    if ( tmp >= OSALMEM_MIN_BLKSZ )
    {
      // Split the block before allocating it.
      osalMemHdr_t *next = (osalMemHdr_t *)((u8 *)hdr + size);
      *next = tmp;
      *hdr = (size | OSALMEM_IN_USE);
#if ( OSALMEM_METRICS )
      blkCnt++;
      if ( blkMax < blkCnt )
      {
        blkMax = blkCnt;
      }
      memAlo += size;
#endif
    }
    else
    {
#if ( OSALMEM_METRICS )
      memAlo += *hdr;
      blkFree--;
#endif
      *hdr |= OSALMEM_IN_USE;
    }
#if ( OSALMEM_METRICS )
    if ( memMax < memAlo )
    {
      memMax = memAlo;
    }
#endif
    hdr++;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.

  return (void *)hdr;
}

/*********************************************************************
 * @fn      osal_mem_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   ptr - pointer to the memory to free.
 *
 * @return  void
 */
void osal_mem_free( void *ptr )
{
  osalMemHdr_t *currHdr;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  currHdr = (osalMemHdr_t *)ptr - 1;


  *currHdr &= ~OSALMEM_IN_USE;
#if ( OSALMEM_METRICS )
  memAlo -= *currHdr;
  blkFree++;
#endif
  if ( ff1 > currHdr )
  {
    ff1 = currHdr;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );  // Re-enable interrupts.
}


#if ( OSALMEM_METRICS )
/*********************************************************************
 * @fn      osal_heap_block_max
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
u16 osal_heap_block_max( void )
{
  return blkMax;
}

/*********************************************************************
 * @fn      osal_heap_block_cnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
u16 osal_heap_block_cnt( void )
{
  return blkCnt;
}

/*********************************************************************
 * @fn      osal_heap_block_free
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
u16 osal_heap_block_free( void )
{
  return blkFree;
}

/*********************************************************************
 * @fn      osal_heap_mem_used
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
u16 osal_heap_mem_used( void )
{
  return memAlo;
}

u16 osal_heap_mem_max( void )
{
  return memMax;
}
#endif
/*********************************************************************
*********************************************************************/
