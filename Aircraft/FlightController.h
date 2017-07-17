#pragma once

#include "FlightControls.h"
#include "SensorsSample.h"
#include "FlightParameters.h"

class FlightController 
{
public:
    FlightController();
    FlightParameters update( const FlightControls& flightControls, const SensorsSample& sensorsSample );
};