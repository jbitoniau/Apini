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
};