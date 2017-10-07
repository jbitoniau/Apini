#pragma once

#include "Platform/UDPSocket.h"

#include "FlightControls.h"
#include "SensorsSample.h"
#include "FlightParameters.h"

class TelemetrySender
{
public:
    TelemetrySender();
    virtual ~TelemetrySender();

    bool send( std::uint32_t timestamp, const FlightControls& flightControls, const SensorsSample& sensorsSample, const FlightParameters& flightParameters );
    
private:
    static int serializeTelemetry( std::uint32_t timestamp, const FlightControls& flightControls, const SensorsSample& sensorsSample, const FlightParameters& flightParameters, char* buffer );
    
    Loco::UDPSocket* socket;
    const int bufferSize = 120;
    char* buffer;

    const unsigned int localPort = 8082;
    const unsigned int remotePort = 8092;
};