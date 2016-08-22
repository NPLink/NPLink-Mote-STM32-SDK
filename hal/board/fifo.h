/*!
 * \file filo.h
 * \brief  FIFO
 */
#ifndef __FIFO__H__
#define __FIFO__H__

#include "stdint.h"
/*!
 * \brief fifo, data size is byte
 */
typedef struct bFIFO{
	uint32_t size;          /*!< fifo size */
	uint32_t length;        /*!< valid data length */
	uint32_t intLine;       /*!< interrupt line */
	uint8_t  *pBuffer;      /*!< fifo buffer */
	uint8_t  *pWrite;       /*!< pointer to pos for writing */
	uint8_t  *pRead;        /*!< pointer to pos for reading */
} bFIFO_t;


/*!
 * \brief fifo, data size is word(16bit)
 */
typedef struct wFIFO{
	uint32_t size;
	uint32_t length;
	uint32_t intLine;
	uint16_t *pBuffer;
	uint16_t *pWrite;
	uint16_t *pRead;
} wFIFO_t;


/* macro definition */
#define NONE_IRQn	(0xff)                      /*!< invalid interrupt line */
#define bFIFO_GetLength(a)	((a)->length)       /*!< get data length in fifo */
#define wFIFO_GetLength(a)	((a)->length)       /*!< get data length in fifo */


/*!
 * \brief init the fifo struct. If the interrupt line is valid, enable interrupt
 * \param [in] pfifo - fifo struct
 * \param [in] pbuffer - fifo buffer
 * \param [in] len - buffer size
 * \param [in] intLine - interrupt line. NONE_IRQn is invalid.
 * \retval ERR_SUCCESS ok
 * \retval ERR_PARAMETER parameter is invalid
 */
int bFIFO_Init(bFIFO_t *pfifo, uint8_t *pbuffer, uint32_t len, uint8_t intLine );

/*!
 * \brief clear fifo data.
 * \param [in] pfifo - fifo struct
 * \retval ERR_SUCCESS ok
 * \retval ERR_PARAMETER parameter is invalid
 */
int bFIFO_Clear(bFIFO_t *pfifo);

/*!
 * \brief put one byte to fifo
 * \param [in] pfifo - fifo struct
 * \param [in] data - data which will be writen to fifo
 * \retval ERR_SUCCESS ok
 * \retval ERR_PARAMETER parameter is invalid
 * \retval ERR_BUFFER_FULL fifo is full.
 */
int bFIFO_PutByte(bFIFO_t *pfifo, uint8_t data);


/*!
 * \brief get one byte from fifo, fifo is not changed.
 * \param [in] pfifo - fifo struct
 * \param [out] data - data
 * \retval ERR_SUCCESS ok
 * \retval ERR_PARAMETER parameter is invalid
 * \retval ERR_BUFFER_EMPTY fifo is empty
 */
int bFIFO_Glance(bFIFO_t *pfifo, uint8_t *data);

/*!
 * \brief get one byte from fifo and the data will be delete
 * \param [in] pfifo - fifo struct
 * \param [out] data - data
 * \retval ERR_SUCCESS ok
 * \retval ERR_PARAMETER parameter is invalid
 * \retval ERR_BUFFER_EMPTY fifo is empty
 */
int bFIFO_GetByte(bFIFO_t *pfifo, uint8_t *data);


/* fifo for word */
int wFIFO_Init(wFIFO_t *pfifo, uint16_t *pbuffer, uint32_t len, uint8_t intSouce);

int wFIFO_Clear(wFIFO_t *pfifo);

int wFIFO_PutByte(wFIFO_t *pfifo, uint16_t data);

int wFIFO_Glance(wFIFO_t *pfifo, uint16_t *data);

int wFIFO_GetByte(wFIFO_t *pfifo, uint16_t *data);

int bFIFO_PutString(bFIFO_t *pfifo, uint8_t *data, uint16_t len);

#endif
