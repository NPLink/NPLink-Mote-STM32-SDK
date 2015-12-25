#include "osal.h"
#include "osal_memory.h"
#include "osal_mutex.h"

osal_mutex_t *osal_mutex_head = NULL;
/*********************************************************************
 * @fn      osal_mutex_create
 *
 * @brief   create a muter
 *
 * @param   none
 *
 * @return  a pointer point to creater muter
 */
osal_mutex_t* osal_mutex_create(void)
{
 osal_mutex_t *ptr;
 osal_mutex_t *pseach;
 ptr = (osal_mutex_t*)osal_mem_alloc(sizeof(osal_mutex_t));
 if(ptr != NULL)
 	{
		 ptr->next_mutex = NULL;
		 ptr->mutex_value = FALSE;
		 if(osal_mutex_head == NULL)
		 	{
		 		osal_mutex_head = ptr;
		 	}
		 else
		 	{
		 		pseach = osal_mutex_head;
		 		while(pseach->next_mutex != NULL)
		 			{
		 			 pseach = pseach->next_mutex;
		 			}
				pseach->next_mutex = ptr;
		 	}
 	}
return ptr;
}

/*********************************************************************
 * @fn      osal_mutex_delete
 *
 * @brief  delete a muter
 *
 * @param  point to be delete muter
 *
 * @return none
 */
void osal_mutex_delete(osal_mutex_t** mutex)
{
	osal_mutex_t* pseach = osal_mutex_head;
	if(pseach == NULL )return;
	if(pseach == *mutex)
		{
			osal_mutex_head = (*mutex)->next_mutex;
			osal_mem_free((u8*)(*mutex));
			*mutex = NULL;

		}
	else
		{
			while( (pseach->next_mutex != *mutex)&&(pseach->next_mutex != NULL) )
				{
					pseach = pseach->next_mutex;
				}
			if(pseach->next_mutex == *mutex)
				{
					pseach->next_mutex = (*mutex)->next_mutex;
					osal_mem_free((u8*)(*mutex));
					*mutex = NULL;
				}
		}

}
/*********************************************************************
 * @fn      osal_mutex_take
 *
 * @brief  take a muter
 *
 * @param   a pointer point to be taken muter
 *
 * @return none
 */
void osal_mutex_take(osal_mutex_t** mutex,u32 mutex_overtime)
{
	if(*mutex == NULL)
		{
		 *mutex = osal_mutex_create();
		}
	if(*mutex != NULL)
		{
		(*mutex)->mutex_value = mutex_overtime;
		}

}
/*********************************************************************
 * @fn      osal_mutex_check
 *
 * @brief  cheak muter state
 *
 * @param   a pointer point to be check muter
 *
 * @return  muter state   1 - take  0 - release
 */
u32 osal_mutex_check(osal_mutex_t* mutex)
{
	if(mutex == NULL)return FALSE;
	if(mutex->mutex_value != 0) return TRUE;
	return FALSE;
}
/*********************************************************************
 * @fn      osal_mutex_release
 *
 * @brief  release a mutex
 *
 * @param   a pointer point to be  release  mutex
 *
 * @return none
 */
void osal_mutex_release(osal_mutex_t** mutex)
{
	if((*mutex) == NULL) return;
	(*mutex)->mutex_value = 0;
	osal_mutex_delete(mutex);
}


/*********************************************************************
 * @fn      osal_mutex_updata
 *
 * @brief   update mutex clock information
 *
 * @param  NONE
 *
 * @return NONE
 */
void osal_mutex_updata(void)
{
	osal_mutex_t *pseach;
	pseach = osal_mutex_head;
	while(pseach != NULL)
		{
			if(pseach->mutex_value>0)
				{
					pseach->mutex_value--;
				}
			pseach = pseach->next_mutex;
		}
}


