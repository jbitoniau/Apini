#include "FlightControlsReceiver.h"

#include <cstdint>

FlightControlsReceiver::FlightControlsReceiver()
{
    socket = new Loco::UDPSocket( localPort );
    printf("FlightControlsReceiver: on port %d\n", localPort);
    buffer = new char[bufferSize];
    memset( buffer, 0, bufferSize );
}

FlightControlsReceiver::~FlightControlsReceiver()
{
    delete socket;
    delete[] buffer;
}

 bool FlightControlsReceiver::receive( FlightControls& flightControls )
 {
    std::string sourceAddress;
    unsigned short int sourcePort;
    int errorCode;
    unsigned int numBytesReceived = socket->receive2( buffer, bufferSize, sourceAddress, sourcePort, errorCode );
    if ( errorCode!=0 )
    {
        printf("Error receiving data %d", errorCode);
        return false;
    }
    else if ( numBytesReceived==0 )
    {
        return false;
    }

    printf("Received %d\n", numBytesReceived);
    return true;
 }

bool FlightControlsReceiver::deserializeFlightControls( char* messageBuffer, unsigned int messageSize, FlightControls& flightControls )
{
    int int32Size = sizeof(std::uint32_t);
    int floatSize = sizeof(float);
    int doubleSize = sizeof(double);

    int offset = 0;
   // memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.accelerationX), doubleSize ); offset+=doubleSize;
   // memcpy( buffer+offset, reinterpret_cast<const char*>(&telemetryData.accelerationY), doubleSize ); offset+=doubleSize;
  
    return false;
}
