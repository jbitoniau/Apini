#pragma once

class FlightParameters 
{
public:
    FlightParameters();

    static const int numMotors = 4;
    float motorPowerLevels[numMotors]; // [0..1]
    float motorPulseWidths[numMotors]; // microseconds
};