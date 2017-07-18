#include "Aircraft.h"

#include <stdio.h>
#include <string>
#include <cmath>        // for std::floor() & co

#include "Platform/LocoTime.h"
#include "Platform/LocoThread.h"

#include "FlightControlsReceiver.h"
#include "Sensors.h"
#include "FlightController.h"
#include "PWMGenerator.h"
#include "TelemetrySender.h"

Aircraft::Aircraft()
{
}

void Aircraft::run()
{
    int framePeriod = 20;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );
  
    FlightControlsReceiver flightControlsReceiver;
    
    Sensors sensors;
    
    FlightController flightController(1000000, 2000000);
    
    std::vector<unsigned int> pwmChannelGPIOs;
    pwmChannelGPIOs.push_back( 25 );
    pwmChannelGPIOs.push_back( 24 );
    pwmChannelGPIOs.push_back( 23 );
    pwmChannelGPIOs.push_back( 22 );
    PWMGenerator pwmGenerator(1000000, 2000000, pwmChannelGPIOs);
  
    TelemetrySender telemetrySender;
        
    printf("Aircraft started\n");
    std::uint32_t timestamp = 0;
    while (true)
    {   
        timestamp = Loco::Time::getTimeAsMilliseconds();

        FlightControls flightControls;
        if ( flightControlsReceiver.receive(flightControls) )
        {
            //printf("throttle:%f rudder:%f elevators:%f ailerons:%f\n", flightControls.throttle, flightControls.rudder, flightControls.elevators, flightControls.ailerons);
        }

        SensorsSample sensorsSample;
        sensors.getSensorsSample();
    
        FlightParameters flightParameters;
        flightParameters = flightController.update( flightControls, sensorsSample );

        pwmGenerator.setPulseWidthInUs( 0, flightParameters.pulseWidthMotor0 );
        pwmGenerator.setPulseWidthInUs( 1, flightParameters.pulseWidthMotor1 );
        pwmGenerator.setPulseWidthInUs( 2, flightParameters.pulseWidthMotor2 );
        pwmGenerator.setPulseWidthInUs( 3, flightParameters.pulseWidthMotor3 );

        telemetrySender.send( timestamp, flightControls, sensorsSample, flightParameters );

        int loopIndex = static_cast<int>( std::floor( Loco::Time::getTimeAsMilliseconds()/framePeriod) );
        while ( loopIndex==lastLoopIndex )
        {
            Loco::Thread::sleep( 1 );
            loopIndex = static_cast<int>( std::floor( Loco::Time::getTimeAsMilliseconds()/framePeriod) );
        }
        lastLoopIndex = loopIndex;

        if ( loopIndex % 50 == 0 )
        {
            fflush(stdout);
        }
    }
}