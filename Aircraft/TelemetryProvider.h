#pragma once

#include "Platform/Platform.h"
#include "TelemetryData.h"
 
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    #include "MPU6050/MPU6050.h"
    #include "HMC5883L/HMC5883L.h"
    #include "MS561101BA/MS561101BA.h"
#endif

class TelemetryProvider
{
public:
    TelemetryProvider();
    virtual ~TelemetryProvider();
    
    TelemetryData getTelemetryData() const;

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    MPU6050*     mpu6050;
    HMC5883L*    hmc5883l;
    MS561101BA*  ms561101ba;
#endif
};