#pragma once

#include "FlightControls.h"
#include "SensorsSample.h"
#include "FlightParameters.h"

class FlightController 
{
public:
    FlightController( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs );
    FlightParameters update( const FlightControls& flightControls, const SensorsSample& sensorsSample );

private:
    unsigned int mMinPulseWidthInUs;
    unsigned int mMaxPulseWidthInUs;
};