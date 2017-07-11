#pragma once

#include "TelemetryData.h"
#include "Platform.h"
 
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    #include "MPU6050/MPU6050.h"
    #include "HMC5883L/HMC5883L.h"
    #include "MS561101BA/MS561101BA.h"
    extern "C" 
    {
        #include "pwm.h"
    }
#endif

class TelemetryDataProvider
{
public:
    TelemetryDataProvider();

    TelemetryData getTelemetryData() const;

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    MPU6050     mpu6050(0x69);
    HMC5883L    hmc5883l(0x1E);
    MS561101BA  ms561101ba(0x77);
#endif
};