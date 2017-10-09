#include "FlightController.h"

#include <stdio.h>
#include <cmath>

FlightController::FlightController( unsigned int minPulseWidth, unsigned int maxPulseWidth ): 
    mMinPulseWidth(0),
    mMaxPulseWidth(0)
{
    if ( minPulseWidth<=maxPulseWidth )
    {
        mMinPulseWidth = minPulseWidth;
        mMaxPulseWidth = maxPulseWidth;
    }
    else
    {
        printf("FlightController::FlightController: min and max pulse widths are the wrong way round. Values swapped\n");
        mMinPulseWidth = maxPulseWidth;
        mMaxPulseWidth = minPulseWidth;     
    }
}

FlightParameters FlightController::update( const FlightControls& flightControls, const SensorsSample& sensorsSample )
{
    FlightParameters flightParameters;

    for ( int i=0; i<flightParameters.numMotors; i++ ) 
    {
        flightParameters.motorPowerLevels[i] = 0;
        flightParameters.motorPulseWidths[i] = mMinPulseWidth; 
    }

    flightParameters.motorPulseWidths[0] = convertMotorPowerLevelToPulseWidth( flightControls.throttle );
    return flightParameters;
}

unsigned int FlightController::convertMotorPowerLevelToPulseWidth( float powerLevel ) const 
{
    if ( powerLevel<0 ) 
    {
        printf("FlightController::convertMotorPowerLevelToPulseWidth: power level is negative. Setting to 0\n");
        powerLevel = 0;
    } 
    else if ( powerLevel>1 ) 
    {
        printf("FlightController::convertMotorPowerLevelToPulseWidth: power level is greater than 1. Setting to 1\n");
        powerLevel = 1;
    }

    unsigned int pulseWidthRange = mMaxPulseWidth - mMinPulseWidth;
    unsigned int pulseWidth = mMinPulseWidth + static_cast<unsigned int>( powerLevel * static_cast<float>(pulseWidthRange) ); 
    return pulseWidth;
}
