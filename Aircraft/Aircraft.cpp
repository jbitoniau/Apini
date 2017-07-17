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
    FlightController flightController;
    TelemetrySender telemetrySender;
    PWMGenerator pwmGenerator;
    
    printf("Aircraft started\n");
    std::uint32_t timestamp = 0;
    while (true)
    {   
        timestamp = Loco::Time::getTimeAsMilliseconds();

        FlightControls flightControls;
        if ( flightControlsReceiver.receive(flightControls) )
        {
            printf("throttle:%f rudder:%f elevators:%f ailerons:%f\n", flightControls.throttle, flightControls.rudder, flightControls.elevators, flightControls.ailerons);
        }

        SensorsSample sensorsSample;
        sensors.getSensorsSample();
    
        FlightParameters flightParameters;
        flightParameters = flightController.update( flightControls, sensorsSample );

        pwmGenerator.setPulseWidthInUs( 0, flightParameters.pwmMotor1 * 1000 );
        pwmGenerator.setPulseWidthInUs( 1, flightParameters.pwmMotor2 * 1000 );
        pwmGenerator.setPulseWidthInUs( 2, flightParameters.pwmMotor3 * 1000 );
        pwmGenerator.setPulseWidthInUs( 3, flightParameters.pwmMotor4 * 1000 );

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