/*
(C)2015 NPLink

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32l051xx.h"

#include "stm32l0xx.h"
#include "utilities.h"
#include "timer.h"
#include "delay.h"
#include "radio.h"
#include "sx1276/sx1276.h"
#include "timer_board.h"
#include "sx1276_board.h"

/*!
 * Unique Devices IDs register set ( STM32L0xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

/*!
 * Define indicating if an external IO expander is to be used
 */
#define BOARD_IOE_EXT

/*!
 * NULL definition
 */
#ifndef NULL
    #define NULL                                    ( ( void * )0 )
#endif

/*!
 * Generic definition
 */
//#ifndef SUCCESS
//#define SUCCESS                                     1
//#endif

#ifndef FAIL
#define FAIL                                        0
#endif

/*!
 * Board IO Extender pins definitions
 */
#define IRQ_MPL3115                                 IOE_0
#define IRQ_MAG3110                                 IOE_1
#define GPS_POWER_ON                                IOE_2
#define RADIO_PUSH_BUTTON                           IOE_3
#define BOARD_POWER_DOWN                            IOE_4
#define SPARE_IO_EXT_5                              IOE_5
#define SPARE_IO_EXT_6                              IOE_6
#define SPARE_IO_EXT_7                              IOE_7
#define N_IRQ_SX9500                                IOE_8
#define IRQ_1_MMA8451                               IOE_9
#define IRQ_2_MMA8451                               IOE_10
#define TX_EN_SX9500                                IOE_11
#define LED_1                                       IOE_12
#define LED_2                                       IOE_13
#define LED_3                                       IOE_14
#define LED_4                                       IOE_15

/*!
 * Board MCU pins definitions
 */

#define RADIO_RESET                                 PB_10

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5
#define RADIO_NSS                                   PA_4

#define RADIO_DIO_0                                 PB_11
#define RADIO_DIO_1                                 PC_13
#define RADIO_DIO_2                                 PB_9
#define RADIO_DIO_3                                 PB_4
#define RADIO_DIO_4                                 PB_3
#define RADIO_DIO_5                                 PA_15

#define RADIO_ANT_SWITCH_HF                         PA_0
#define RADIO_ANT_SWITCH_LF                         PA_1

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  PH_0
#define OSC_HSE_OUT                                 PH_1

#define USB_DM                                      PA_11
#define USB_DP                                      PA_12

#define I2C_SCL                                     PB_6
#define I2C_SDA                                     PB_7

#define BOOT_1                                      PB_2

#define GPS_PPS                                     PB_1
#define UART_TX                                     PA_9
#define UART_RX                                     PA_10

#define DC_DC_EN                                    PB_8
#define BAT_LEVEL                                   PB_0
#define WKUP1                                       PA_8
#define USB_ON                                      PA_2

#define RF_RXTX                                     PA_3

#define SWDIO                                       PA_13
#define SWCLK                                       PA_14

#define TEST_POINT1                                 PB_12
#define TEST_POINT2                                 PB_13
#define TEST_POINT3                                 PB_14
#define TEST_POINT4                                 PB_15

#define PIN_NC                                      PB_5

/*!
 * Board GPIO pin names
 */
typedef enum
{
    MCU_PINS,
    IOE_PINS,

    // Not connected
    NC = (int)0xFFFFFFFF
}PinNames;

/*!
 * Structure for the GPIO
 */
typedef struct
{
    PinNames  pin;
    uint16_t pinIndex;
    void *port;
    uint16_t portIndex;
}Gpio_t;


/*!
 * LED GPIO pins objects
 */

extern Gpio_t Led1;
extern Gpio_t Led2;
extern Gpio_t Led3;
extern Gpio_t Led4;


/*!
 * \brief Measure the Battery level
 *
 * \retval value  battery level ( 0: very low, 254: fully charged )
 */
uint8_t BoardMeasureBatterieLevel( void );

/*!
 */
void SpiInit( Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss );

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void );

#endif // __BOARD_H__
