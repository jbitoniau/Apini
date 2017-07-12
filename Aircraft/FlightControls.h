#pragma once

//#include <cstdint>

class FlightControls
{
public:
    FlightControls():
        throttle(0.0),
        rudder(0.0),
        elevators(0.0),   
        ailerons(0.0)
    {
    }
    
    float throttle;     // [0..1]
    float rudder;       // [-0.5, 0.5]
    float elevators;    // [-0.5, 0.5]
    float ailerons;     // [-0.5, 0.5]
};