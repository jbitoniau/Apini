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
    // std::vector<unsigned int> pwmChannelGPIOs;
    // pwmChannelGPIOs.push_back( 25 );
    // // pwmChannelGPIOs.push_back( 24 );
    // // pwmChannelGPIOs.push_back( 23 );
    // // pwmChannelGPIOs.push_back( 22 );
    // PWMGenerator pwmGenerator(1000, 2000, pwmChannelGPIOs);
    // printf(".\n");
    
    // pwmGenerator.setPulseWidthInUs( 0, 1000 );
    // Loco::Thread::sleep( 1000 );   
    // printf(".\n");
    
    // pwmGenerator.setPulseWidthInUs( 0, 1300 );   
    // Loco::Thread::sleep( 1000 );   
    // printf(".\n");
    
    // pwmGenerator.setPulseWidthInUs( 0, 1000 );   
    // Loco::Thread::sleep( 1000 );   
    // printf(".\n");
    // exit(0);

    int framePeriod = 10;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );
  
    FlightControlsReceiver flightControlsReceiver;
    Sensors sensors;
    FlightController flightController(1000, 2000);
    std::vector<unsigned int> pwmChannelGPIOs;
    pwmChannelGPIOs.push_back( 25 );
    pwmChannelGPIOs.push_back( 24 );
    // pwmChannelGPIOs.push_back( 23 );
    // pwmChannelGPIOs.push_back( 22 );
    PWMGenerator pwmGenerator(1000, 2000, pwmChannelGPIOs);
    TelemetrySender telemetrySender;
        
    printf("Aircraft started\n");
    std::uint32_t timestamp = 0;
    std::uint32_t lastFlightControlsTimestamp = 0;    
    
    std::uint32_t flightControlsTimeout = 1000;

    FlightControls flightControls;
    FlightParameters flightParameters;
    while (true)
    {   
        timestamp = Loco::Time::getTimeAsMilliseconds();
        
        SensorsSample sensorsSample;
        sensorsSample = sensors.getSensorsSample();
        
        if ( flightControlsReceiver.receive(flightControls) )
        {
            lastFlightControlsTimestamp = timestamp;
            //printf("throttle:%f rudder:%f elevators:%f ailerons:%f\n", flightControls.throttle, flightControls.rudder, flightControls.elevators, flightControls.ailerons);
        }

        if ( timestamp<=lastFlightControlsTimestamp+flightControlsTimeout )
        {
            flightParameters = flightController.update( flightControls, sensorsSample );
        } 
        else 
        {
            flightParameters.powerMotor0 = 0;
            flightParameters.powerMotor1 = 0;
            flightParameters.powerMotor2 = 0;
            flightParameters.powerMotor3 = 0;
            // + maintain flight controller in a reset mode... 
        }

        pwmGenerator.setPower( 0, flightParameters.powerMotor0 );
        pwmGenerator.setPower( 1, flightParameters.powerMotor1 );

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