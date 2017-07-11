#include <cstdint>
#include <stdio.h>
#include <string>
#include <cmath>        // for std::floor() & co

#include "LocoTime.h"
#include "LocoThread.h"
#include "UDPSocket.h"

#include "TelemetryDataProvider.h"

int serializeTelemetryData( const TelemetryData& telemetryData, char* buffer )
{
    int int32Size = sizeof(std::uint32_t);
    int floatSize = sizeof(float);
    int doubleSize = sizeof(double);

    int offset = 0;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.accelerationX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.accelerationY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.accelerationZ), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.angularSpeedX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.angularSpeedY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.angularSpeedZ), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.temperature), floatSize ); offset+=floatSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.magneticHeadingX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.magneticHeadingY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.magneticHeadingZ), doubleSize ); offset+=doubleSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.temperature2), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.pressure), floatSize ); offset+=floatSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.timestamp), int32Size ); offset+=int32Size;
    return offset;
}

int main( int argc, char* argv[] )
{
    Loco::UDPSocket socket( 8282 );
    char buffer[512];
    int framePeriod = 20;
    int startTime = Loco::Time::getTimeAsMilliseconds();
    int lastLoopIndex = static_cast<int>( std::floor( startTime/framePeriod) );

    TelemetryDataProvider telemetryDataProvider;

    printf("Aircraft started\n");
    while (true)
    {   
        // Get telemetry data
        TelemetryData telemetryData = telemetryDataProvider.getTelemetryData();

        // Serialize it
        int numBytesToSend = serializeTelemetryData( telemetryData, buffer );   

        // Send it over the socket
        int numBytesSent = socket.send( buffer, numBytesToSend, "127.0.0.1", 8181 );

        // Sleep until next loop cycle
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