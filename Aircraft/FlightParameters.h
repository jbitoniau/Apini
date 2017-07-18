#pragma once

class FlightParameters 
{
public:
    FlightParameters();

    unsigned int pulseWidthMotor0;      // in microseconds
    unsigned int pulseWidthMotor1;
    unsigned int pulseWidthMotor2;
    unsigned int pulseWidthMotor3;
};