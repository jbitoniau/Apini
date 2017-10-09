#pragma once

#include "Platform/Platform.h"
#include <vector>

/*
    PWMGenerator

    The PWMGenerator is an abstraction form managing multiple PWM signal generation channels.

    All pulse widths are expressed in microseconds.

    TODO:
    - timeout: if a channel isn't updated for a while, bring it back to zero (either instantly
      or gradually)
    - min value per channel: the 'zero' value might not be the minimum of the range (for eg for a servo
      actuating a camera)
*/
class PWMGenerator 
{
public:
    PWMGenerator( unsigned int minPulseWidth, unsigned int maxPulseWidth, std::size_t numChannels );
    virtual ~PWMGenerator();

    std::size_t getNumChannels() const { return mPulseWidths.size(); }
    unsigned int getPulseWidth( std::size_t channelIndex ) const;                  
    bool setPulseWidth( std::size_t channelIndex, unsigned int pulseWidth );

    unsigned int getMinPulseWidth() const { return mMinPulseWidth; } 
    unsigned int getMaxPulseWidth() const { return mMaxPulseWidth; } 
    
protected:
    virtual bool internalSetPulseWidth(std::size_t channelIndex, unsigned int width ) { return true; }

private:
    unsigned int mMinPulseWidth;
    unsigned int mMaxPulseWidth;
    std::vector<unsigned int> mPulseWidths;
};

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX

class PWMGeneratorGPIO : public PWMGenerator 
{
public: 
    PWMGeneratorGPIO( unsigned int minPulseWidth, unsigned int maxPulseWidth, const std::vector<unsigned int>& channelGPIOs );
    virtual ~PWMGeneratorGPIO();

protected:
    virtual bool internalSetPulseWidth(std::size_t channelIndex, unsigned int width );

private:
    bool mInitializedOK;
    std::vector<unsigned int> mChannelGPIOs;
};

#endif
