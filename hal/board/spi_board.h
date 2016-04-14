/*
(C)2015 NPLink

Description: SPI1 driver implementation, used for communication with sx1276/9

License: Revised BSD License, see LICENSE.TXT file include in the project

*/

#ifndef _SPI_BOARD_H
#define _SPI_BOARD_H

#include "stm32l0xx_hal.h"

/*!
 * SPI driver structure definition, for sx1276/1279
 */
struct Spi_s
{
    SPI_TypeDef *Spi;
    //Gpio_t Mosi;
    //Gpio_t Miso;
    //Gpio_t Sclk;
    //Gpio_t Nss;
};


/*!
 * SPI object type definition
 */
typedef struct Spi_s Spi_t;

extern SPI_HandleTypeDef SpiHandle;
#define SPIx_TIMEOUT_MAX              ((uint32_t)0x1000)

/*############################### SPI1 #######################################*/
#define SPIx                          SPI1
#define SPIx_CLK_ENABLE()             __SPI1_CLK_ENABLE()
#define SPIx_GPIO_PORT                GPIOA                      /* GPIOA */
#define SPIx_AF                       GPIO_AF0_SPI1

#define SPIx_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define SPIx_GPIO_CLK_DISABLE()       __GPIOA_CLK_DISABLE()

#define SPIx_SCK_PIN                  GPIO_PIN_5                 /* PA.05 */
#define SPIx_MISO_PIN                 GPIO_PIN_6                 /* PA.06 */
#define SPIx_MOSI_PIN                 GPIO_PIN_7                 /* PA.07 */
#define SPIx_NSS_PIN                  GPIO_PIN_4

#define SPIx_NSS_LOW()            HAL_GPIO_WritePin(SPIx_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_RESET)
#define SPIx_NSS_HIGH()           HAL_GPIO_WritePin(SPIx_GPIO_PORT, SPIx_NSS_PIN, GPIO_PIN_SET)

 void               SPI1_Init(void);
 void               SPI1_DeInit(void);
 void               SPI1_Write(uint8_t Value);
 uint32_t 		    	SPI1_Read(void);

#endif //_SPI_BOARD_H
