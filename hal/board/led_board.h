/*
(C)2015 NPLink

Description: LED driver implementation, used for led display.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#ifndef __LED_BOARD_H
#define __LED_BOARD_H

#include "stm32l0xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
//#include "hal_board.h"
#include <stdint.h>
/*********************************************************************
 * MACROS
 */

#define HAL_LED      TRUE
#define BLINK_LEDS


#define OFF GPIO_PIN_RESET
#define ON GPIO_PIN_SET


#define HAL_LED_BLINK_EVENT                 0x0020


/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_1     0x01   //red led
#define HAL_LED_2     0x02   //green led

#define HAL_LED_ALL   (HAL_LED_1 | HAL_LED_2 )

/* Modes */
#define HAL_LED_MODE_OFF     0x00
#define HAL_LED_MODE_ON      0x01
#define HAL_LED_MODE_BLINK   0x02
#define HAL_LED_MODE_FLASH   0x04
#define HAL_LED_MODE_TOGGLE  0x08

/* Defaults */
#define HAL_LED_DEFAULT_MAX_LEDS      2
#define HAL_LED_DEFAULT_DUTY_CYCLE    5
#define HAL_LED_DEFAULT_FLASH_COUNT   50
#define HAL_LED_DEFAULT_FLASH_TIME    1000

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Initialize LED Service.
 */
void HalLedInit(void);

/*
 * Set the LED ON/OFF/TOGGLE.
 */
uint8_t HalLedSet( uint8_t led, uint8_t mode );

/*
 * Blink the LED.
 */
void HalLedBlink( uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time );


void HalLedUpdate (void);

void HalLedOnOff (uint8 leds, uint8 mode);

/*
 * Put LEDs in sleep state - store current values
 */
//void HalLedEnterSleep( void );

/*
 * Retore LEDs from sleep state
 */
//void HalLedExitSleep( void );

/*
 * Return LED state
 */
uint8_t HalLedGetState ( void );

/*********************************************************************
*********************************************************************/

//#define   RedLED(PinState)  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,PinState)
//#define   GreenLED(PinState)  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,PinState)





#ifdef __cplusplus
}
#endif


#endif //__LED_BOARD_H
