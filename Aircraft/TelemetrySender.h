#pragma once

#include "UDPSocket.h"
#include "TelemetryData.h"

class TelemetrySender
{
public:
    TelemetrySender();
    virtual ~TelemetrySender();

    bool send( const TelemetryData& telemetryData );
    
private:
    static int serializeTelemetryData( const TelemetryData& telemetryData, char* buffer );
    
    Loco::UDPSocket* socket;
    const int bufferSize = 512;
    char* buffer;

    const unsigned int localPort = 8082;
    const unsigned int remotePort = 8092;
};