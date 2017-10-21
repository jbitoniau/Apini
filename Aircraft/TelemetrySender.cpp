#include "TelemetrySender.h"
#include <cstring>

TelemetrySender::TelemetrySender()
{
    socket = new Loco::UDPSocket( localPort );
    socket->setReceiveBufferSizeInBytes( bufferSize );
    socket->setSendBufferSizeInBytes( bufferSize );
    printf("TelemetrySender: on port %d for remote port %d \n", localPort, remotePort);
    
    buffer = new char[bufferSize];
    memset( buffer, 0, bufferSize );
}

TelemetrySender::~TelemetrySender()
{
    delete socket;
    delete[] buffer;
}

bool TelemetrySender::send( std::uint32_t timestamp, const FlightControls& flightControls, const SensorsSample& sensorsSample, const FlightParameters& flightParameters ) 
{
    int numBytesToSend = serializeTelemetry( timestamp, flightControls, sensorsSample, flightParameters, buffer );   
    if ( numBytesToSend!=bufferSize ) 
    {
        printf("TelemetrySender buffer %d!=%d\n", numBytesToSend, bufferSize );
    }
    int numBytesSent = socket->send( buffer, numBytesToSend, "127.0.0.1", remotePort );
    bool result = (numBytesToSend==numBytesSent);
    return result;
}

int TelemetrySender::serializeTelemetry( std::uint32_t timestamp, const FlightControls& flightControls, const SensorsSample& sensorsSample, const FlightParameters& flightParameters, char* buffer )
{
    int int32Size = sizeof(std::uint32_t);
    int floatSize = sizeof(float);
    int doubleSize = sizeof(double);

    int offset = 0;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&timestamp), int32Size ); offset+=int32Size;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.accelerationX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.accelerationY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.accelerationZ), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.angularSpeedX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.angularSpeedY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.angularSpeedZ), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.temperature), floatSize ); offset+=floatSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.magneticHeadingX), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.magneticHeadingY), doubleSize ); offset+=doubleSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.magneticHeadingZ), doubleSize ); offset+=doubleSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.temperature2), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&sensorsSample.pressure), floatSize ); offset+=floatSize;
    
    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightControls.throttle), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightControls.rudder), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightControls.elevators), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightControls.ailerons), floatSize ); offset+=floatSize;
    
    for ( int i=0; i<FlightParameters::numMotors; i++ ) 
    {
        memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.motorPowerLevels[i]), floatSize ); offset+=floatSize;
    }
    
    for ( int i=0; i<FlightParameters::numMotors; i++ ) 
    {
        memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.motorPulseWidths[i]), int32Size ); offset+=int32Size;
    }

    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.measuredRollSpeed), floatSize ); offset+=floatSize;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.targetRollSpeed), floatSize ); offset+=floatSize;

    return offset;
}
