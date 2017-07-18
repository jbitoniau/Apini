#include "FlightController.h"

#include <stdio.h>

FlightController::FlightController( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs ): 
    mMinPulseWidthInUs(0),
    mMaxPulseWidthInUs(0)
{
    if ( mMinPulseWidthInUs<=mMaxPulseWidthInUs )
    {
        mMinPulseWidthInUs = minPulseWidthInUs;
        mMaxPulseWidthInUs = maxPulseWidthInUs;
    }
    else
    {
        mMinPulseWidthInUs = maxPulseWidthInUs;
        mMaxPulseWidthInUs = minPulseWidthInUs;     
        printf("FlightController::FlightController: min and max pulse widths are the wrong way round. Values swapped\n");
    }
}

FlightParameters FlightController::update( const FlightControls& flightControls, const SensorsSample& sensorsSample )
{
    FlightParameters flightParameters;
    unsigned int pulseWidthRange = mMaxPulseWidthInUs - mMinPulseWidthInUs;     
    flightParameters.pulseWidthMotor0 = mMinPulseWidthInUs + (flightControls.throttle * pulseWidthRange);
    flightParameters.pulseWidthMotor1 = mMinPulseWidthInUs + (flightControls.elevators * pulseWidthRange);
    flightParameters.pulseWidthMotor2 = mMinPulseWidthInUs;
    flightParameters.pulseWidthMotor3 = mMinPulseWidthInUs;
    return flightParameters;
}