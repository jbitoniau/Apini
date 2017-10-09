#pragma once

#include "FlightControls.h"
#include "SensorsSample.h"
#include "FlightParameters.h"

class FlightController 
{
public:
    FlightController( unsigned int minPulseWidth, unsigned int maxPulseWidth );
    FlightParameters update( const FlightControls& flightControls, const SensorsSample& sensorsSample );

private:
    unsigned int convertMotorPowerLevelToPulseWidth( float powerLevel ) const;

    unsigned int mMinPulseWidth;
    unsigned int mMaxPulseWidth;
};


