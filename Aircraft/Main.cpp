#include <cstdint>
#include <stdio.h>
#include <string>
#include <cmath>        // for std::floor() & co

#include "Platform/LocoTime.h"
#include "Platform/LocoThread.h"

#include "TelemetryProvider.h"
#include "TelemetrySender.h"
#include "FlightControlsReceiver.h"

int main( int argc, char* argv[] )
{
    int framePeriod = 20;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );

    TelemetryProvider telemetryProvider;
    TelemetrySender telemetrySender;

    FlightControls flightControls;
    FlightControlsReceiver flightControlsReceiver;

    printf("Aircraft started\n");
    while (true)
    {   
        if ( flightControlsReceiver.receive(flightControls) )
        {
            printf("throttle:%f rudder:%f elevators:%f ailerons:%f\n", flightControls.throttle, flightControls.rudder, flightControls.elevators, flightControls.ailerons);
        }

        TelemetryData telemetryData = telemetryProvider.getTelemetryData();
        telemetryData.throttle = flightControls.throttle;
        telemetryData.rudder = flightControls.rudder;
        telemetryData.elevators = flightControls.elevators;
        telemetryData.ailerons = flightControls.ailerons;
        telemetrySender.send( telemetryData );

        int loopIndex = static_cast<int>( std::floor( Loco::Time::getTimeAsMilliseconds()/framePeriod) );
        while ( loopIndex==lastLoopIndex )
        {
            Loco::Thread::sleep( 1 );
            loopIndex = static_cast<int>( std::floor( Loco::Time::getTimeAsMilliseconds()/framePeriod) );
        }
        lastLoopIndex = loopIndex;

        if ( loopIndex % 50 == 0 )
        {
            //printf(".");
            fflush(stdout);
        }
    }

    return 0;
}