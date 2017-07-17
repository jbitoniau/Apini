#include "Sensors.h"

#include <stdio.h>
#include "Platform/LocoTime.h"
#include "Platform/LocoThread.h"

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
double getAccelerometerHalfScaleRange( MPU6050& mpu6050 )
{
    uint8_t scaleCode = mpu6050.getFullScaleAccelRange();
    switch ( scaleCode )
    {
        case MPU6050_ACCEL_FS_2 : return 2.0;
        case MPU6050_ACCEL_FS_4 : return 4.0;
        case MPU6050_ACCEL_FS_8 : return 8.0;
        case MPU6050_ACCEL_FS_16 : return 16.0;
    }
    printf("getAccelerometerHalfScaleRange unsupported value returned\n");
    return 2.0;
}

double getGyroscopeHalfScaleRange( MPU6050& mpu6050 )
{
    uint8_t scaleCode = mpu6050.getFullScaleGyroRange();
    switch ( scaleCode )
    {
        case MPU6050_GYRO_FS_250 : return 250.0;
        case MPU6050_GYRO_FS_500 : return 500.0;
        case MPU6050_GYRO_FS_1000 : return 1000.0;
        case MPU6050_GYRO_FS_2000 : return 2000.0;
    }
    printf("getGyroscopeHalfScaleRange unsupported value returned\n");
    return 250.0;
}
#endif

Sensors::Sensors()
{
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    // MPU6050
    mpu6050 = new MPU6050(0x69);

    mpu6050->initialize();
    mpu6050->setI2CMasterModeEnabled(0);     // !!!!!!!!!!!!!
    mpu6050->setI2CBypassEnabled(1);         // !!!!!!!!!!!!!
    mpu6050->setFullScaleAccelRange(MPU6050_ACCEL_FS_4);     // in Gs
    mpu6050->setFullScaleGyroRange(MPU6050_GYRO_FS_1000);    // in deg/s
    printf(mpu6050->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // HMC5883L
    hmc5883l = new HMC5883L(0x1E);
    hmc5883l->initialize();
    printf(hmc5883l->testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

    // MS561101BA
    ms561101ba = new MS561101BA(0x77);
    ms561101ba->initialize();      // Note: this is costly on the MS5611-01BA. See if we can decrease the delay
    printf(ms561101ba->testConnection() ? "MS561101BA connection successful" : "MS561101BA connection failed");
#endif
}

Sensors::~Sensors()
{
#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    delete mpu6050;
    delete hmc5883l;
    delete ms561101ba;
#endif
}

SensorsSample Sensors::getSensorsSample() const
{
    SensorsSample sensorsSample;

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
    // MPU6050
    double accelerometerHalfScaleRange = getAccelerometerHalfScaleRange(*mpu6050);
    double gyroscopeHalfScaleRange = getGyroscopeHalfScaleRange(*mpu6050);
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    mpu6050->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);       // This takes 1 or 2 ms
    sensorsSample.accelerationX = static_cast<double>(ax) * accelerometerHalfScaleRange / 32768.0;
    sensorsSample.accelerationY = static_cast<double>(ay) * accelerometerHalfScaleRange / 32768.0;
    sensorsSample.accelerationZ = static_cast<double>(az) * accelerometerHalfScaleRange / 32768.0;
    sensorsSample.angularSpeedX = static_cast<double>(gx) * gyroscopeHalfScaleRange / 32768.0;
    sensorsSample.angularSpeedY = static_cast<double>(gy) * gyroscopeHalfScaleRange / 32768.0;
    sensorsSample.angularSpeedZ = static_cast<double>(gz) * gyroscopeHalfScaleRange / 32768.0;

    int16_t t = mpu6050->getTemperature();
    sensorsSample.temperature = static_cast<float>(t)/340.f + 36.53f;
        
    // HMC5883L
    int16_t mx = 0;
    int16_t my = 0;
    int16_t mz = 0;
    hmc5883l->getHeading(&mx, &my, &mz);                     // This takes 1 or 2 ms
    sensorsSample.magneticHeadingX = static_cast<double>(mx);
    sensorsSample.magneticHeadingY = static_cast<double>(my);
    sensorsSample.magneticHeadingZ = static_cast<double>(mz);

    // MS561101BA
    float temperature = 0.f;            
    float pressure = 0.f;
    ms561101ba->readValues( &pressure, &temperature );       // This takes 4 or 5 ms
    sensorsSample.temperature2 = temperature;
    sensorsSample.pressure = pressure;
#endif

    return sensorsSample;
}
