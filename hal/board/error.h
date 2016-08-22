/*!
 * \file error.h
 * \brief define return code
 */
#ifndef __ERROR_H__
#define __ERROR_H__


/* generic error */
#define ERR_SUCCESS	 			(0)         /*!< ok */
#define ERR_UNDEFINE			(-1)        /*!< unknown error */
#define ERR_PARAMETER			(-2)        /*!< invalid parameter */
#define ERR_TIME_OUT			(-3)        /*!< time is out */
#define ERR_PARITY              (-4)        /* */

/* buffer error */
#define ERR_BUFFER_INITIALIZED  (-10)       /*!< buffer is not initalized */
#define ERR_BUFFER_EMPTY        (-11)       /*!< buffer is empty */
#define ERR_BUFFER_FULL         (-12)       /*!< buffer is full */

/* spi flash error */
#define ERR_FLASH_PROGRAM       (-20)       /*!< error on flash programming */
#define ERR_FLASH_ERASE         (-21)       /*!< error on flash erasing */

/*instruction match*/
#define ERR_INS_MATCHING		(-22)
#define ERR_INS_MISMATCH		(-23)

/* xmodem error */
#define ERR_XMODEM_IDLE         (-30)
#define ERR_XMODEM_END          (-31)

#define DEV_TEST_START         (1)   //设备内测启动
#define DEV_TEST_STOP          (2)   //设备内测关闭

#endif

