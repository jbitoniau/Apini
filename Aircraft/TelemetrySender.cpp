#include "TelemetrySender.h"

TelemetrySender::TelemetrySender()
{
    socket = new Loco::UDPSocket( localPort );
    printf("TelemetrySender: on port %d for remote port %d \n", localPort, remotePort);
   
    buffer = new char[bufferSize];
    memset( buffer, 0, bufferSize );
}

TelemetrySender::~TelemetrySender()
{
    delete socket;
    delete[] buffer;
}

bool TelemetrySender::send( const TelemetryData& telemetryData ) 
{
    int numBytesToSend = serializeTelemetryData( telemetryData, buffer );   
    int numBytesSent = socket->send( buffer, numBytesToSend, "127.0.0.1", remotePort );
    bool result = (numBytesToSend==numBytesSent);
    return result;
}

int TelemetrySender::serializeTelemetryData( const TelemetryData& telemetryData, char* buffer )
{
    int int32Size = sizeof(std::uint32_t);
    int floatSize = sizeof(float);
    int doubleSize = sizeof(double);

    int offset = 0;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.timestamp), int32Size ); offset+=int32Size;
    
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
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.throttle), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.rudder), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.elevators), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.ailerons), floatSize ); offset+=floatSize;
    
    return offset;
}
