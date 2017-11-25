#pragma once

#include "FlightControls.h"
#include "Platform/UDPSocket.h"

class FlightControlsReceiver 
{
public:
    FlightControlsReceiver();
    virtual ~FlightControlsReceiver();

    bool receive( FlightControls& flightControls );

private:
    static bool deserializeFlightControls( char* messageBuffer, unsigned int messageSize, FlightControls& flightControls );

    Loco::UDPSocket* socket;
    const int bufferSize = (4*4 + 3*4) + 16;        // Not sure why +16 needed on OSX 
    char* buffer;

    const unsigned int localPort = 8081;
    //const unsigned int remotePort = 8092;
};  