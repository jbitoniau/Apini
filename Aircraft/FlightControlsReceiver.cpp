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

// Check the socket for any packet received, read as many as there are ready to flush the buffer, 
// but stop after a certain number of them. Update the flight controls corresponding to the last 
// packet received (i.e the most recent) that got fully deserialized. Return true if it did, and
// false if there no packet was received or none was deserialized properly.
bool FlightControlsReceiver::receive( FlightControls& flightControls )
{
    std::string sourceAddress;
    unsigned short int sourcePort;
    int errorCode;
    unsigned int numBytesReceived = 0;      
    bool flightControlsReceived = false;
    int maxNumPacketsReceived = 10;  // This method will stop tring to receive/read packets after this number of packets received. This is to avoid blocking forever when socket is saturated
    int numPacketsReceived = 0;
    FlightControls tempFlightControls;
    do 
    {
        numBytesReceived = socket->receive2( buffer, bufferSize, sourceAddress, sourcePort, errorCode );
        if ( numBytesReceived!=0 )
        {
            numPacketsReceived++;
            bool deserializedOK = FlightControlsReceiver::deserializeFlightControls( buffer, numBytesReceived, tempFlightControls );
            if ( deserializedOK ) 
            {
                flightControlsReceived = true;
            }
            memset( buffer, 0, bufferSize );
        }
        /*if ( errorCode!=0 )
        {
            printf("Error receiving data %d", errorCode);
            return false;
        }*/
    }
    while ( numBytesReceived!=0 && numPacketsReceived<maxNumPacketsReceived );
        
    // printf("FlightControlsReceiver::receive: from:%s:%d (%d bytes): ", sourceAddress.c_str(), sourcePort, numBytesReceived);
    // for ( int i=0; i<numBytesReceived; i++ )
    // {
    //     unsigned char c = buffer[i];
    //     printf( "%x", c );
    // } 
    // printf("\n");

    // if ( numPacketsReceived>1)
    // {
    //     if (numBytesReceived!=0) {
    //         printf("Received %d flight controls packets in one loop (more in queue)\n", numPacketsReceived );
    //     } else {
    //         printf("Received %d flight controls packets in one loop (no more in queue)\n", numPacketsReceived );
    //     }
    // }
    
    // if ( numPacketsReceived==0 ) {
    //     printf("0");
    // }
    if ( flightControlsReceived ) {
        flightControls = tempFlightControls;
    }
    return flightControlsReceived;
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
