#pragma once

#include "Platform/Platform.h"
#include <vector>

class PWMGenerator 
{
public:
    PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs );

    std::size_t getNumChannels() const { return mChannelWidths.size(); }
    unsigned int getMinPulseWidth() const { return mMinPulseWidthInUs; } 
    unsigned int getMaxPulseWidth() const { return mMaxPulseWidthInUs; } 

    // float getPower( std::size_t channelIndex ) const;                  
    bool setPower( std::size_t channelIndex, float value );

    // unsigned int getPulseWidthInUs( std::size_t channelIndex ) const;                  
    // bool setPulseWidthInUs( std::size_t channelIndex, unsigned int widthInUs );

    // void clearAll(); 

private:
    bool setPulseWidthInUs( std::size_t channelIndex, unsigned int widthInUs );
    
    unsigned int mMinPulseWidthInUs;
    unsigned int mMaxPulseWidthInUs;

    std::vector<unsigned int> mChannelGPIOs;
    std::vector<unsigned int> mChannelWidths;     // TODO: maybe store a zero-position for each channel as well
  
    // int mPulseWidthIncrementInUs;
    // int mRPIOPWMChannelIndex;
};