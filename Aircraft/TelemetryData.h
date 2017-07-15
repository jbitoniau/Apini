#pragma once

#include <cstdint>

class TelemetryData
{
public:
    TelemetryData():
        timestamp(0),
        accelerationX(0.0),
        accelerationY(0.0),
        accelerationZ(0.0),
        angularSpeedX(0.0),
        angularSpeedY(0.0),
        angularSpeedZ(0.0),
        temperature(22.f),
        magneticHeadingX(0.0),
        magneticHeadingY(0.0),
        magneticHeadingZ(0.0),
        temperature2(21.f),
        pressure(0.f),
        throttle(0),
        rudder(0),
        elevators(0),
        ailerons(0)
    {
    }
    
    std::uint32_t   timestamp;
    
    double          accelerationX;          // In Gs
    double          accelerationY;
    double          accelerationZ;
    double          angularSpeedX;          // In degrees per sec
    double          angularSpeedY;
    double          angularSpeedZ;
    float           temperature;            // In degrees celsius

    double          magneticHeadingX;       // In gauss. In Probably change that to int16
    double          magneticHeadingY;
    double          magneticHeadingZ;

    float           temperature2;           // In degrees celsius
    float           pressure;               // In HPa
    
    float           throttle;
    float           rudder;
    float           elevators;
    float           ailerons;
};