/*
(C)2015 NPLink

Description: SX1276 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "board.h"
#include "radio.h"
#include "sx1276/sx1276.h"
#include "sx1276_board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer
};

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;

void SX1276IoInit( void )
{
	//Init IO
	//DIO0 -- PB10, DIO1 -- PB2, DIO2 -- PB1, DIO3 -- PB0, NRESET -- PB11
   GPIO_InitTypeDef  GPIO_InitStruct;
	 /* -1- Enable GPIOB Clock (to be able to program the configuration registers) */
  __GPIOB_CLK_ENABLE();
  /* -2- Configure PA.0 IO in output push-pull mode to
         drive external LEDs */
  GPIO_InitStruct.Pin = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10);
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
	uint8_t irqCounter = 0;

	for(; irqCounter < 6; irqCounter ++)
	{
		GpioIrq[irqCounter] = irqHandlers[irqCounter];
	}

	//DIO0 -- PB10, DIO1 -- PB2, DIO2 -- PB1, DIO3 -- PB0,

	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Enable GPIOB clock */
	__GPIOB_CLK_ENABLE();

	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable and set EXTI4_15 Interrupt to the lowest priority */
	//HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
	//HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_3_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

void SX1276IoDeInit( void )
{
	 GPIO_InitTypeDef  GPIO_InitStruct;

  GPIO_InitStruct.Pin = (GPIO_PIN_0 |GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 );
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);//PB11控制sx芯片的reset引脚
	//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET);

	
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //PA2控制sx芯片的有源晶振
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
	
	__GPIOB_CLK_DISABLE();  
	__GPIOA_CLK_DISABLE();   //20160317
}

uint8_t SX1276GetPaSelect( uint32_t channel )
{
    //if( channel < RF_MID_BAND_THRESH )
    {
        return RF_PACONFIG_PASELECT_PABOOST;
    }
    //else
    //{
        //return RF_PACONFIG_PASELECT_RFO;
    //}
}

void SX1276SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;

        if( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

void SX1276AntSwInit( void )
{
	//RADIO_ANT_SWITCH_HF -- PA1

   GPIO_InitTypeDef  GPIO_InitStruct;

  __GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = (GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET); //add by zjh
}

void SX1276AntSwDeInit( void )
{
	//RADIO_ANT_SWITCH_HF -- PA1

  GPIO_InitTypeDef  GPIO_InitStruct;
	
  //__GPIOA_CLK_DISABLE();

  GPIO_InitStruct.Pin = (GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET); //add by zjh
	

}

void SX1276SetAntSw( uint8_t rxTx )
{
  if( SX1276.RxTx == rxTx )
  {
    return;
  }

  SX1276.RxTx = rxTx;

  if( rxTx != 0 ) // 1: TX, 0: RX
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
  }
}

//sx1279 board use active crystal, and PA2 control it's power supply
//PA2 = 1: power on, PA2 = 0: power off.
void SX1276Q1CtrlInit( void )
{
   GPIO_InitTypeDef  GPIO_InitStruct;

  __GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = (GPIO_PIN_2);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
}

void SX1276Q1CtrlDeInit( void )
{
   GPIO_InitTypeDef  GPIO_InitStruct;

  //__GPIOA_CLK_DISABLE();

  GPIO_InitStruct.Pin = (GPIO_PIN_2);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SX1276SetQ1( uint8_t onOff )
{
  if( onOff != 0 ) // 1: ON, 0: OFF
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
  }
}


bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supportted
    return true;
}
