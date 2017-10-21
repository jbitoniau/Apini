#include "FlightParameters.h"

FlightParameters::FlightParameters():
    measuredRollSpeed(0.f),
    targetRollSpeed(0.f)
{
    for ( int i=0; i<numMotors; i++ ) 
    {
        motorPowerLevels[i] = 0.f;
        motorPulseWidths[i] = 0;
    }
}