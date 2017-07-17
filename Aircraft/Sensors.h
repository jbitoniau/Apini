#pragma once

#include "Platform/Platform.h"
#include "SensorsSample.h"
 
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    #include "MPU6050/MPU6050.h"
    #include "HMC5883L/HMC5883L.h"
    #include "MS561101BA/MS561101BA.h"
#endif

class Sensors
{
public:
    Sensors();
    virtual ~Sensors();
    
    SensorsSample getSensorsSample() const;

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    MPU6050*     mpu6050;
    HMC5883L*    hmc5883l;
    MS561101BA*  ms561101ba;
#endif
};