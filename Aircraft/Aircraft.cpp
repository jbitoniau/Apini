#include "Aircraft.h"

#include <stdio.h>
#include <string>
#include <cmath>        // for std::floor() & co

#include "Platform/Platform.h"
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
    // FlightControlsReceiver
    FlightControlsReceiver flightControlsReceiver;

    // FlightController
    FlightController flightController(1000, 1400);  // Set max pulse width output to 1300us for now...

    // PWMGenerator
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    std::vector<unsigned int> pwmChannelGPIOs;
    pwmChannelGPIOs.push_back( 25 );
    pwmChannelGPIOs.push_back( 24 );
    pwmChannelGPIOs.push_back( 23 );
    pwmChannelGPIOs.push_back( 22 );
    PWMGeneratorGPIO pwmGenerator(1000, 2000, pwmChannelGPIOs);
#else 
    PWMGenerator pwmGenerator(1000, 2000, FlightParameters::numMotors);
#endif

    // TelemetrySender
    TelemetrySender telemetrySender;

    // Main loop
    printf("Aircraft started\n");
    int framePeriod = 20;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    startTime = ((startTime / 1000) + 1) * 1000;   // Round the start time to the next second and wait for it. This is to produce cleaner graph data. But this causes startup of the process to be a bit longer!
    printf("startTime: %d\n", startTime );
    while ( Loco::Time::getTimeAsMilliseconds()<startTime ) {
        Loco::Thread::sleep( 1 );
    } 

    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );
    std::uint32_t timestamp = 0;
    std::uint32_t lastFlightControlsTimestamp = 0;    
    std::uint32_t flightControlsTimeout = 200;

    Sensors sensors;
    FlightControls flightControls;
    FlightParameters flightParameters;

    while (true)
    {   
        timestamp = Loco::Time::getTimeAsMilliseconds();
            
        // Read sensors
        SensorsSample sensorsSample;
        sensorsSample = sensors.getSensorsSample();
        
        // Check and read new received flight controls
        if ( flightControlsReceiver.receive(flightControls) )
        {
            lastFlightControlsTimestamp = timestamp;
            //printf("throttle:%f rudder:%f elevators:%f ailerons:%f\n", flightControls.throttle, flightControls.rudder, flightControls.elevators, flightControls.ailerons);
        } else {
            // flightControls = FlightControls();
            //printf("!");
        }

        if ( timestamp<=lastFlightControlsTimestamp+flightControlsTimeout )
        {
            // The aircraft is currently being controlled, so we update the flight controller
            flightParameters = flightController.update( flightControls, sensorsSample );
        } 
        else 
        {
            // The aircraft is not being controlled, set motor pulse width to minimum
            for ( int i=0; i<FlightParameters::numMotors; i++ ) 
            {
                flightParameters.motorPulseWidths[i] = pwmGenerator.getMinPulseWidth();
            }
            // + maintain flight controller in a reset mode... 
        }

        // Update PWM channels
        for ( int i=0; i<FlightParameters::numMotors; i++ ) 
        {
            pwmGenerator.setPulseWidth( i, flightParameters.motorPulseWidths[i] );
        }

        // Send telemetry
        telemetrySender.send( timestamp, flightControls, sensorsSample, flightParameters );

        // Wait for next loop to start
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