#include "FlightController.h"

#include <stdio.h>
#include <cmath>

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
    float t = 0.f;      // goes to 0.5 maximum for now
    if ( flightControls.throttle>0.5f ) 
    {
        t = flightControls.throttle - 0.5f;
    }

    float e = 0.f;      // goes to 0.5 maximum for now
    if ( flightControls.elevators>0.f ) 
    {
        e = flightControls.elevators;       
    }

    flightParameters.pulseWidthMotor0 = mMinPulseWidthInUs + (t * pulseWidthRange);
    flightParameters.pulseWidthMotor1 = mMinPulseWidthInUs + (e * pulseWidthRange);
    flightParameters.pulseWidthMotor2 = mMinPulseWidthInUs + (0.2 * pulseWidthRange);
    flightParameters.pulseWidthMotor3 = mMinPulseWidthInUs + (0.4 * pulseWidthRange);
    return flightParameters;
}