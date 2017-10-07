#include "FlightControlsReceiver.h"

#include <cstdint>
#include <cstring>

FlightControlsReceiver::FlightControlsReceiver()
{
    socket = new Loco::UDPSocket( localPort );
    socket->setReceiveBufferSizeInBytes( bufferSize );
    socket->setSendBufferSizeInBytes( bufferSize );
    
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
    unsigned int numBytesReceived = 0;      
    // TODO: add a max number of loops to go through (to avoid blocking here forever)
    bool receivedData = false;
    int numPacketReceived = 0;
    do 
    {
        numBytesReceived = socket->receive2( buffer, bufferSize, sourceAddress, sourcePort, errorCode );
        if ( numBytesReceived!=0 )
        {
            numPacketReceived++;
            bool deserializedOK = FlightControlsReceiver::deserializeFlightControls( buffer, numBytesReceived, flightControls );
            if ( deserializedOK ) 
            {
                receivedData = true;
            }
            memset( buffer, 0, bufferSize );
        }
        /*if ( errorCode!=0 )
        {
            printf("Error receiving data %d", errorCode);
            return false;
        }*/
    }
    while ( numBytesReceived!=0 );
        
   // printf("FlightControlsReceiver::receive: from:%s:%d (%d bytes): ", sourceAddress.c_str(), sourcePort, numBytesReceived);
    // for ( int i=0; i<numBytesReceived; i++ )
    // {
    //     unsigned char c = buffer[i];
    //     printf( "%x", c );
    // } 
    // printf("\n");

        if ( numPacketReceived>1)
        {
            printf("Received %d flight controls packets\n", numPacketReceived );
        }

    return receivedData;
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
