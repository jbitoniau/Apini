#pragma once

//#include <cstdint>

// TODO: write proper setters/getters which enforce validity!
class FlightControls
{
public:
    FlightControls():
        throttle(0.f),
        rudder(0.f),
        elevators(0.f),   
        ailerons(0.f)
    {
    }
    
    float throttle;     // [0..1]
    float rudder;       // [-0.5, 0.5]
    float elevators;    // [-0.5, 0.5]
    float ailerons;     // [-0.5, 0.5]
};