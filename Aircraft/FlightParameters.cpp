#include "FlightParameters.h"

FlightParameters::FlightParameters()
{
    for ( int i=0; i<numMotors; i++ ) 
    {
        motorPowerLevels[i] = 0;
        motorPulseWidths[i] = 0;
    }
}