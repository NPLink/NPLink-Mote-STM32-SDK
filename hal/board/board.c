/*
(C)2015 NPLink

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "board.h"

/*!
 * Battery thresholds
 */
#define BATTERY_MAX_LEVEL                           4150 // mV
#define BATTERY_MIN_LEVEL                           3200 // mV
#define BATTERY_SHUTDOWN_LEVEL                      3100 // mV

uint16_t BoardGetPowerSupply( void )
{
    return 0 ;
}

uint8_t BoardMeasureBatterieLevel( void )
{
    __IO uint8_t batteryLevel = 0;
    uint16_t batteryVoltage = 0;

    if(1) //if( GpioRead( &UsbDetect ) == 1 )
    {
        batteryLevel = 0;
    }
    else
    {
        batteryVoltage = BoardGetPowerSupply( );

        if( batteryVoltage >= BATTERY_MAX_LEVEL )
        {
            batteryLevel = 254;
        }
        else if( ( batteryVoltage > BATTERY_MIN_LEVEL ) && ( batteryVoltage < BATTERY_MAX_LEVEL ) )
        {
            batteryLevel = ( ( 253 * ( batteryVoltage - BATTERY_MIN_LEVEL ) ) / ( BATTERY_MAX_LEVEL - BATTERY_MIN_LEVEL ) ) + 1;
        }
        else if( batteryVoltage <= BATTERY_SHUTDOWN_LEVEL )
        {
            batteryLevel = 255;
            //GpioInit( &DcDcEnable, DC_DC_EN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
            //GpioInit( &BoardPowerDown, BOARD_POWER_DOWN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );
        }
        else // BATTERY_MIN_LEVEL
        {
            batteryLevel = 1;
        }
    }
    return batteryLevel;
}

