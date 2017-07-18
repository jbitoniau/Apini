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
    PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs );
    unsigned int getPulseWidthInUs( std::size_t channelIndex ) const;                  
    bool setPulseWidthInUs( std::size_t channelIndex, unsigned int widthInUs );

private:
    unsigned int mMinPulseWidthInUs;
    unsigned int mMaxPulseWidthInUs;

    std::vector<unsigned int> mChannelGPIOs;
    std::vector<unsigned int> mChannelWidths;     // TODO: maybe store a zero-position for each channel as well
  
    int mPulseWidthIncrementInUs;
    int mRPIOPWMChannelIndex;
};