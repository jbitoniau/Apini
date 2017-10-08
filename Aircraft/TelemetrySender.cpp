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
    
    // memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.pulseWidthMotor0), int32Size ); offset+=int32Size;
    // memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.pulseWidthMotor1), int32Size ); offset+=int32Size;
    // memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.pulseWidthMotor2), int32Size ); offset+=int32Size;
    // memcpy( buffer+offset, reinterpret_cast<const char*>(&flightParameters.pulseWidthMotor3), int32Size ); offset+=int32Size;

    int m0 = flightParameters.powerMotor0 * 1000 + 1000;
    int m1 = flightParameters.powerMotor1 * 1000 + 1000;
    int m2 = flightParameters.powerMotor2 * 1000 + 1000;
    int m3 = flightParameters.powerMotor3 * 1000 + 1000;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&m0), int32Size ); offset+=int32Size;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&m1), int32Size ); offset+=int32Size;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&m2), int32Size ); offset+=int32Size;
    memcpy( buffer+offset, reinterpret_cast<const char*>(&m3), int32Size ); offset+=int32Size;

    return offset;
}
