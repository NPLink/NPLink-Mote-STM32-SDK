#include <stdio.h>
#include <string.h>
#include <stdint.h>
 
#include "error.h" 
//#include "system.h"
#include "fifo.h"



/* enable the interrupt */
void FIFO_IntEnable(uint8_t intLine)
{
    if(intLine != NONE_IRQn )
    { 
        //NVIC_IntEnable( intLine ); 
    }   
}


/* disable the interrupt */
void FIFO_IntDisable(uint8_t intLine)
{
    if(intLine != NONE_IRQn)
    {
        //NVIC_IntDisable( intLine );
    }   
}


int bFIFO_Init(bFIFO_t *pfifo, uint8_t *pbuffer, uint32_t len, uint8_t intLine)
{
	if((pfifo == NULL) || (pbuffer == NULL) || (len == 0))
	{
		return ERR_PARAMETER;
	}
	
    /* initialize the fifo */	
	pfifo->size = len;		
	pfifo->pBuffer = pbuffer;
	pfifo->intLine = intLine;
	pfifo->length = 0;	/* there is no valid data length */
	memset(pfifo->pBuffer, 0, pfifo->size);
	pfifo->pWrite = pfifo->pRead = pfifo->pBuffer; /* write head = read head = buffer head on no valid data */
	
	return ERR_SUCCESS;
}

int bFIFO_Clear(bFIFO_t *pfifo)
{
	if(pfifo == NULL)
	{
		return ERR_PARAMETER;
	}
			
	FIFO_IntDisable(pfifo->intLine);/* disable the interrupt */

    /* clear the buffer */
	pfifo->length = 0;	
	memset(pfifo->pBuffer, 0, pfifo->size);
	pfifo->pWrite = pfifo->pRead = pfifo->pBuffer;

	FIFO_IntEnable(pfifo->intLine);/* enable the interrupt */

	return ERR_SUCCESS;
}


int bFIFO_Glance(bFIFO_t *pfifo, uint8_t *data)
{	
	if((pfifo == NULL) || (data == NULL))
	{
		return ERR_PARAMETER;
	}
    
    FIFO_IntDisable(pfifo->intLine); /* disable the interrupt */

    if(pfifo->length == 0) /* fifo is empty */
	{
        FIFO_IntEnable(pfifo->intLine);
		return ERR_BUFFER_EMPTY;
	}
	
	*data = *(pfifo->pRead); /* get the data, but the read head pointer is kept */
	FIFO_IntEnable(pfifo->intLine); /* enable the interrupt */

	return ERR_SUCCESS;
}


int bFIFO_GetByte(bFIFO_t *pfifo, uint8_t *data)
{	
	if((pfifo == NULL) || (data == NULL))
	{
		return ERR_PARAMETER;
	}    

	FIFO_IntDisable(pfifo->intLine); /* disable the interrupt */

	if(pfifo->length == 0) /* fifo is empty */
	{
		FIFO_IntEnable(pfifo->intLine);	
		return ERR_BUFFER_EMPTY;
	}

	*data = *(pfifo->pRead);/* get the data */

	(pfifo->length)--;
	(pfifo->pRead)++;

	if(pfifo->pRead >= (pfifo->pBuffer + pfifo->size)) /* roll back */
	{
		pfifo->pRead = pfifo->pBuffer;	
	}

	FIFO_IntEnable(pfifo->intLine);/* enable the interrupt */

	return ERR_SUCCESS;
}


int bFIFO_PutByte(bFIFO_t *pfifo, uint8_t data)
{
	if(pfifo == NULL)
	{
		return ERR_PARAMETER;
	}

	FIFO_IntDisable(pfifo->intLine); /* disable the interrupt */

	if(pfifo->length == pfifo->size) /* fifo is empty */
	{
		FIFO_IntEnable(pfifo->intLine);
		return ERR_BUFFER_FULL;
	}
	
	*(pfifo->pWrite) = data;

	(pfifo->length)++;
	(pfifo->pWrite)++;

	if(pfifo->pWrite >= (pfifo->pBuffer + pfifo->size)) /* buffer roll back */
	{
		pfifo->pWrite = pfifo->pBuffer;	
	}

	FIFO_IntEnable(pfifo->intLine); /* enable the interrupt */

	return ERR_SUCCESS;
}

int bFIFO_PutString(bFIFO_t *pfifo, uint8_t *data, uint16_t len)
{
	if(pfifo == NULL)
	{
		return ERR_PARAMETER;
	}

	FIFO_IntDisable(pfifo->intLine); /* disable the interrupt */

	if(pfifo->length+len > pfifo->size) /* fifo has no enough space */
	{
		FIFO_IntEnable(pfifo->intLine);
		return ERR_BUFFER_FULL;
	}

	while(len--)
	{
		*(pfifo->pWrite) = *data++;

		(pfifo->length)++;
		(pfifo->pWrite)++;

		if(pfifo->pWrite >= (pfifo->pBuffer + pfifo->size)) /* buffer roll back */
		{
			pfifo->pWrite = pfifo->pBuffer;	
		}
	}
	

	FIFO_IntEnable(pfifo->intLine); /* enable the interrupt */

	return ERR_SUCCESS;
}


int wFIFO_Init(wFIFO_t *pfifo, uint16_t *pbuffer, uint32_t len, uint8_t intLine)
{
	if((pfifo == NULL) || (pbuffer == NULL) || (len == 0))
	{
		return ERR_PARAMETER;
	}

	pfifo->size = len;		
	pfifo->pBuffer = pbuffer;
	pfifo->intLine = intLine;
	pfifo->length = 0;	
	memset(pfifo->pBuffer, 0, pfifo->size);
	pfifo->pWrite = pfifo->pRead = pfifo->pBuffer;
	
	return ERR_SUCCESS;
}

int wFIFO_Clear(wFIFO_t *pfifo)
{
	if(pfifo == NULL)
	{
		return ERR_PARAMETER;
	}
			
	FIFO_IntDisable(pfifo->intLine);

	pfifo->length = 0;	
	memset(pfifo->pBuffer, 0, pfifo->size);
	pfifo->pWrite = pfifo->pRead = pfifo->pBuffer;

	FIFO_IntEnable(pfifo->intLine);

	return ERR_SUCCESS;
}


int wFIFO_Glance(wFIFO_t *pfifo, uint16_t *data)
{	
	if((pfifo == NULL) || (data == NULL))
	{
		return ERR_PARAMETER;
	}

	FIFO_IntDisable(pfifo->intLine);

	if(pfifo->length == 0)
	{
		FIFO_IntEnable(pfifo->intLine);	
		return ERR_BUFFER_EMPTY;
	}

	*data = *(pfifo->pRead);

	FIFO_IntEnable(pfifo->intLine);

	return ERR_SUCCESS;
}

int wFIFO_GetByte(wFIFO_t *pfifo, uint16_t *data)
{	
	if((pfifo == NULL) || (data == NULL))
	{
		return ERR_PARAMETER;
	}

	FIFO_IntDisable(pfifo->intLine);

	if(pfifo->length == 0)
	{
		FIFO_IntEnable(pfifo->intLine);	
		return ERR_BUFFER_EMPTY;
	}

	*data = *(pfifo->pRead);

	(pfifo->length)--;
	(pfifo->pRead)++;

	if(pfifo->pRead >= (pfifo->pBuffer + pfifo->size))
	{
		pfifo->pRead = pfifo->pBuffer;	
	}

	FIFO_IntEnable(pfifo->intLine);

	return ERR_SUCCESS;
}


int wFIFO_PutByte(wFIFO_t *pfifo, uint16_t data)
{
	if(pfifo == NULL)
	{
		return ERR_PARAMETER;
	}

	FIFO_IntDisable(pfifo->intLine);

	if(pfifo->length == pfifo->size)
	{
		FIFO_IntEnable(pfifo->intLine);
		return ERR_BUFFER_FULL;
	}
	
	*(pfifo->pWrite) = data;

	(pfifo->length)++;
	(pfifo->pWrite)++;

	if(pfifo->pWrite >= (pfifo->pBuffer + pfifo->size))
	{
		pfifo->pWrite = pfifo->pBuffer;	
	}

	FIFO_IntEnable(pfifo->intLine);

	return ERR_SUCCESS;
}


