#pragma once

#include "FlightControls.h"
#include "SensorsSample.h"
#include "FlightParameters.h"

/*
    PIDController
*/
class PIDController 
{
public:
    PIDController( float proportionalTerm, float integralTerm, float maxIntegralOutput );

    float update( float error, float deltaTime ); // deltaTime is in seconds
    void reset();

private:
    float mProportionalTerm;
    float mIntegralTerm;
    float mMaxIntegralOutput;

    float mIntegralOutput; 
};

/*
    FlightController
*/
class FlightController 
{
public:
    FlightController( unsigned int minPulseWidth, unsigned int maxPulseWidth /*, float angularRatePerControlValueUnit (how each ailerons, elevetors, rudder values from flight controls are converted/mapped into an angular rate in deg/sec */);
    FlightParameters update( const FlightControls& flightControls, const SensorsSample& sensorsSample );

private:
    unsigned int convertMotorPowerLevelToPulseWidth( float powerLevel ) const;

    unsigned int mMinPulseWidth;
    unsigned int mMaxPulseWidth;
};


