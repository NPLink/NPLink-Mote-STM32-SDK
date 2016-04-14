/*
(C)2015 NPLink

Description: SPI1 driver implementation, used for communication with sx1276/9

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "spi_board.h"
#include "sx1276/sx1276.h"
#include "sx1276_board.h"

SPI_HandleTypeDef SpiHandle;

/*********************************************************************
 * @fn		 SPI1_MspInit
 *
 * @brief 	 SPI MSP Init
 *
 * @param 	hspi: SPI handle
 *
 * @return	void
 */
void SPI1_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  SPIx_GPIO_CLK_ENABLE();

  /* Configure SPI SCK */
  GPIO_InitStruct.Pin = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;  //GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = SPIx_AF;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MOSI */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_AF;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MISO */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_AF;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI NSS pin: */
  GPIO_InitStruct.Pin = SPIx_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);
  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  SPIx_CLK_ENABLE();
}

/*********************************************************************
 * @fn		 SPI1_Init
 *
 * @brief 	 SPIx Bus initialization
 *
 * @param 	None
 *
 * @return	void
 */
void SPI1_Init(void)
{
  SX1276.Spi.Spi = ( SPI_TypeDef* )SPI1_BASE;

  if(HAL_SPI_GetState(&SpiHandle) == HAL_SPI_STATE_RESET)
  {
    /* SPI Config */
    SpiHandle.Instance = SPIx;

    /* On STM32L0538-DISCO, EPD ID cannot be read then keep a common configuration */
    /* for EPD (SPI_DIRECTION_2LINES) */
    /* Note: To read a register a EPD, SPI_DIRECTION_1LINE should be set */
    SpiHandle.Init.Mode               = SPI_MODE_MASTER;
    SpiHandle.Init.Direction          = SPI_DIRECTION_2LINES;
    SpiHandle.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_8;
    SpiHandle.Init.DataSize           = SPI_DATASIZE_8BIT;
    SpiHandle.Init.CLKPhase           = SPI_PHASE_1EDGE;//SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity        = SPI_POLARITY_LOW;//SPI_POLARITY_HIGH;SPI_POLARITY_LOW
    SpiHandle.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS                = SPI_NSS_SOFT;
    SpiHandle.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.CRCPolynomial      = 7;
    SpiHandle.Init.TIMode             = SPI_TIMODE_DISABLED;

    SPI1_MspInit(&SpiHandle);
    HAL_SPI_Init(&SpiHandle);
  }
}

/*********************************************************************
 * @fn		 SPI1_DeMspInit
 *
 * @brief 	 SPI MSP DeInit
 *
 * @param 	hspi: SPI handle
 *
 * @return	void
 */
void SPI1_DeMspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef   GPIO_InitStruct;

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  //SPIx_GPIO_CLK_ENABLE();
  //SPIx_GPIO_CLK_DISABLE();  //20160317
  SPIx_CLK_DISABLE();
	
	  /* Configure SPI SCK */
  GPIO_InitStruct.Pin = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;  //GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = SPIx_AF;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MOSI */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_AF;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MISO */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_AF;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI NSS pin: */
  GPIO_InitStruct.Pin = SPIx_NSS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SPIx_GPIO_PORT, &GPIO_InitStruct);
}

/*********************************************************************
 * @fn		 SPI1_DeInit
 *
 * @brief
 *
 * @param 	None
 *
 * @return	void
 */
 void SPI1_DeInit( void )
{
	HAL_SPI_DeInit(&SpiHandle);
	SPI1_DeMspInit(&SpiHandle);
}

/**
  * @brief  SPI error treatment function.
  * @param  None
  * @retval None
  */
 void SPI1_Error (void)
{
  /* De-Initialize the SPI comunication BUS */
  HAL_SPI_DeInit(&SpiHandle);

  /* Re-Initiaize the SPI comunication BUS */
  SPI1_Init();
}

/*********************************************************************
 * @fn		 SPI1_Read
 *
 * @brief 	 SPI Read 4 bytes from device
 *
 * @param 	ReadSize Number of bytes to read (max 4 bytes)
 *
 * @return	Value read on the SPI
 */
 uint32_t SPI1_Read(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t readvalue = 0;
  uint32_t writevalue = 0xFFFFFFFF;

  status = HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*) &writevalue, (uint8_t*) &readvalue, 1, SPIx_TIMEOUT_MAX);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the BUS */
    SPI1_Error();
  }
  return readvalue;
}

/*********************************************************************
 * @fn		 SPI1_Write
 *
 * @brief 	 SPI Write a byte to device
 *
 * @param 	Value: value to be written
 *
 * @return	None
 */
 void SPI1_Write(uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_SPI_Transmit(&SpiHandle, (uint8_t*) &Value, 1, SPIx_TIMEOUT_MAX);

	/* Check the communication status */
	if(status != HAL_OK)
	{
	/* Re-Initiaize the BUS */
		SPI1_Error();
	}
}
