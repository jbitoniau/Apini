#pragma once

#include "UDPSocket.h"
#include "TelemetryData.h"

class TelemetrySender
{
public:
    TelemetrySender();
    virtual ~TelemetrySender();

    bool send( const TelemetryData& telemetryData );
    static int serializeTelemetryData( const TelemetryData& telemetryData, char* buffer );

private:
    Loco::UDPSocket* socket;
    const int bufferSize = 512;
    char* buffer;
};