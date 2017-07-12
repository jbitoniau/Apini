#include <cstdint>
#include <stdio.h>
#include <string>
#include <cmath>        // for std::floor() & co

#include "LocoTime.h"
#include "LocoThread.h"

#include "TelemetryProvider.h"
#include "TelemetrySender.h"

int main( int argc, char* argv[] )
{
    int framePeriod = 20;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );

    TelemetryProvider telemetryProvider;
    TelemetrySender telemetrySender;

    printf("Aircraft started\n");
    while (true)
    {   
        TelemetryData telemetryData = telemetryProvider.getTelemetryData();
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
            printf(".");
            fflush(stdout);
        }
    }

    return 0;
}