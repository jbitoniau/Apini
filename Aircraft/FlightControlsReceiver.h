#pragma once

#include "FlightControls.h"
#include "UDPSocket.h"

class FlightControlsReceiver 
{
public:
    FlightControlsReceiver();
    virtual ~FlightControlsReceiver();

    bool receive( FlightControls& flightControls );

private:
    static bool deserializeFlightControls( char* messageBuffer, unsigned int messageSize, FlightControls& flightControls );

    Loco::UDPSocket* socket;
    const int bufferSize = 512;
    char* buffer;
};  