#include "FlightParameters.h"

FlightParameters::FlightParameters()
{
    for ( int i=0; i<numMotors; i++ ) 
    {
        motorPowerLevels[i] = 0.f;
        motorPulseWidths[i] = 0;
    }
}