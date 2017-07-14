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
    /*if ( errorCode!=0 )
    {
        printf("Error receiving data %d", errorCode);
        return false;
    }*/
    if ( numBytesReceived==0 )
    {
        memset( buffer, 0, bufferSize );
        return false;
    }

   // printf("FlightControlsReceiver::receive: from:%s:%d (%d bytes): ", sourceAddress.c_str(), sourcePort, numBytesReceived);
    // for ( int i=0; i<numBytesReceived; i++ )
    // {
    //     unsigned char c = buffer[i];
    //     printf( "%x", c );
    // } 
    // printf("\n");

/*    if ( numBytesReceived==0 )
    {
        return false;
    }
*/
    bool deserializedOK = FlightControlsReceiver::deserializeFlightControls( buffer, numBytesReceived, flightControls );
    memset( buffer, 0, bufferSize );
   
    return deserializedOK;
 }

bool FlightControlsReceiver::deserializeFlightControls( char* buffer, unsigned int bufferSize, FlightControls& flightControls )
{
    //int int32Size = sizeof(std::uint32_t);
    int floatSize = sizeof(float);
    //int doubleSize = sizeof(double);

    // TODO: make sure we don't go too far!
    int offset = 0;
    memcpy( reinterpret_cast<char*>(&flightControls.throttle), buffer+offset, floatSize ); 
    offset+=floatSize;
    memcpy( reinterpret_cast<char*>(&flightControls.rudder), buffer+offset, floatSize ); 
    offset+=floatSize;
    memcpy( reinterpret_cast<char*>(&flightControls.elevators), buffer+offset, floatSize ); 
    offset+=floatSize;
    memcpy( reinterpret_cast<char*>(&flightControls.ailerons), buffer+offset, floatSize ); 
  
    return true;
}
