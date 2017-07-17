#pragma once

#include "Platform/Platform.h"
#include <vector>

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
extern "C" 
{
    #include "pwm.h"
}
#endif

class PWMGenerator 
{
public:
    PWMGenerator();
    int getPulseWidthInUs( int channelIndex ) const;                  
    bool setPulseWidthInUs( int channelIndex, int widthInUs );

private:
    const int minWidth = 1000;
    const int maxWidth = 2000;

    std::vector<int> channelGPIOs;
    std::vector<int> channelWidths;
    // zero-position for each channel

    int pulseWidthIncrementInUs;
    int RPIOPWMChannelIndex;
};