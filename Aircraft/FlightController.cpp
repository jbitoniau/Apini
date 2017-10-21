#include "FlightController.h"

#include <stdio.h>
#include <cmath>
#include "Platform/LocoTime.h"

/*
    PID

    TODO:
    - add derivative support
*/  
PIDController::PIDController( float proportionalTerm, float integralTerm, float maxIntegralOutput ):
    mProportionalTerm(proportionalTerm),
    mIntegralTerm(integralTerm),
    mMaxIntegralOutput(maxIntegralOutput),
    mIntegralOutput(0.f)
{
}

float PIDController::update( float error, float deltaTime ) 
{
    // deltaTime should be >=0
    float proportionalOutput = mProportionalTerm * error;
    
    mIntegralOutput = mIntegralOutput + (error * mIntegralTerm * deltaTime);
    if ( mIntegralOutput < -mMaxIntegralOutput )  
    {
        mIntegralOutput = -mMaxIntegralOutput;
    }
    else if (mIntegralOutput > mMaxIntegralOutput) 
    {
        mIntegralOutput = mMaxIntegralOutput;
    }

    float output = proportionalOutput + mIntegralOutput;
    return output;
}
    
void PIDController::reset() 
{
    mIntegralOutput = 0.f;   
}

/*
    FlightController
*/
FlightController::FlightController( unsigned int minPulseWidth, unsigned int maxPulseWidth ): 
    mMinPulseWidth(0),
    mMaxPulseWidth(0),
    mRollPIDController(0.011f, 0.f, 0.f)   // proportionalTerm: motorPowerLevel per speed error (in degree per second)
{
    if ( minPulseWidth<=maxPulseWidth )
    {
        mMinPulseWidth = minPulseWidth;
        mMaxPulseWidth = maxPulseWidth;
    }
    else
    {
        printf("FlightController::FlightController: min and max pulse widths are the wrong way round. Values swapped\n");
        mMinPulseWidth = maxPulseWidth;
        mMaxPulseWidth = minPulseWidth;     
    }
}

FlightParameters FlightController::update( const FlightControls& flightControls, const SensorsSample& sensorsSample )
{
    FlightParameters flightParameters;

    // Set motors to minimum
    for ( int i=0; i<flightParameters.numMotors; i++ ) 
    {
        flightParameters.motorPowerLevels[i] = 0.f;
        flightParameters.motorPulseWidths[i] = mMinPulseWidth; 
    }
    flightParameters.targetRollSpeed = 0;
    flightParameters.measuredRollSpeed = sensorsSample.angularSpeedX;

    if ( flightControls.throttle>0.1 ) {
        float angularSpeedPerPerFlightControlUnit = 80;  // The angular speed in degrees per second that we want to achieve when control stick is at full extent
        flightParameters.targetRollSpeed = flightControls.ailerons * angularSpeedPerPerFlightControlUnit; // In degrees per second 
        flightParameters.measuredRollSpeed = sensorsSample.angularSpeedX;  // In degrees per second. Later there should transform+mapping somewhere to tell how gyro related to aircraf orientation
        float errorRollSpeed = flightParameters.measuredRollSpeed - flightParameters.targetRollSpeed;
        float time = Loco::Time::getTimeAsMilliseconds() / 1000;
        float correctionMotorPowerLevel = mRollPIDController.update(errorRollSpeed, time);
        flightParameters.motorPowerLevels[0] = clamp<float>( flightControls.throttle - correctionMotorPowerLevel, 0.f, 1.f );
        flightParameters.motorPowerLevels[1] = clamp<float>( flightControls.throttle + correctionMotorPowerLevel, 0.f, 1.f );
        printf("tr:%f mr:%f err:%f corr:%f m0:%f m1:%f t:%f\n", flightParameters.targetRollSpeed, flightParameters.measuredRollSpeed, errorRollSpeed, correctionMotorPowerLevel, flightParameters.motorPowerLevels[0], flightParameters.motorPowerLevels[1], time); 
    }

    // Set motor pulse widths from power levels
    for ( int i=0; i<flightParameters.numMotors; i++ ) 
    {
        flightParameters.motorPulseWidths[i] = convertMotorPowerLevelToPulseWidth( flightParameters.motorPowerLevels[i] );
    }
    return flightParameters;
}

unsigned int FlightController::convertMotorPowerLevelToPulseWidth( float powerLevel ) const 
{
    if ( powerLevel<0 ) 
    {
        printf("FlightController::convertMotorPowerLevelToPulseWidth: power level is negative. Setting to 0\n");
        powerLevel = 0;
    } 
    else if ( powerLevel>1 ) 
    {
        printf("FlightController::convertMotorPowerLevelToPulseWidth: power level is greater than 1. Setting to 1\n");
        powerLevel = 1;
    }

    unsigned int pulseWidthRange = mMaxPulseWidth - mMinPulseWidth;
    unsigned int pulseWidth = mMinPulseWidth + static_cast<unsigned int>( powerLevel * static_cast<float>(pulseWidthRange) ); 
    return pulseWidth;
}
