#include "PWMGenerator.h"

#include <stdio.h>
#include <cstdlib>      // for EXIT_SUCCESS 


#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
#include <pigpio.h>
#include <signal.h>     // for SIGINT
#endif

PWMGenerator::PWMGenerator( unsigned int minPulseWidth, unsigned int maxPulseWidth, std::size_t numChannels ):
    mMinPulseWidth(0),
    mMaxPulseWidth(0),
    mPulseWidths()
{
    if ( mMinPulseWidth<=mMaxPulseWidth )
    {
        mMinPulseWidth = minPulseWidth;
        mMaxPulseWidth = maxPulseWidth;
    }
    else
    {
        printf("PWMGenerator::PWMGenerator: min and max pulse widths are the wrong way round. Values swapped\n");
        mMinPulseWidth = maxPulseWidth;
        mMaxPulseWidth = minPulseWidth;     
    }

    mPulseWidths.resize(numChannels);
    for ( std::size_t i=0; i<mPulseWidths.size(); i++ )
    {
        mPulseWidths[i] = mMinPulseWidth;
    }
}

PWMGenerator::~PWMGenerator() 
{
    for ( std::size_t i=0; i<mPulseWidths.size(); i++ )
    {
        setPulseWidth( i, mMinPulseWidth );
    }
}

// Return the current pulse width for the given channel.
// Return the minimum pulse width to indicate invalid channel index
unsigned int PWMGenerator::getPulseWidth( std::size_t channelIndex ) const
{
    if ( channelIndex>=getNumChannels() )
    {
        printf("PWMGenerator::getPulseWidth: channel index %lu out of range\n", channelIndex);
        return mMinPulseWidth;            
    }
    return mPulseWidths[channelIndex];
}   

// Set the current pulse with for the given channel. Return true on success, false otherwise
bool PWMGenerator::setPulseWidth( std::size_t channelIndex, unsigned int pulseWidth )
{
    if ( channelIndex>=getNumChannels() )
    {
        printf("PWMGenerator::setPulseWidth: index %lu out of range\n", channelIndex);
        return false;
    }

    if ( pulseWidth<mMinPulseWidth || pulseWidth>mMaxPulseWidth ) 
    {
        printf("PWMGenerator::setPulseWidth: pulse width %d for channel %lu is outside the valid [%d..%d] range\n", pulseWidth, channelIndex, mMinPulseWidth, mMaxPulseWidth );
        return false;
    }

    bool result = internalSetPulseWidth( channelIndex, pulseWidth );
    if ( !result ) 
    {
        return false;
    }

    mPulseWidths[channelIndex] = pulseWidth;
    return true;
}

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX

/*
    PWMGeneratorGPIO

    GPIO#   Pin#
    4       P1-7
    17      P1-11
    18      P1-12
    21      P1-13
    22      P1-15
    23      P1-16
    24      P1-18
    25      P1-22
*/
PWMGeneratorGPIO::PWMGeneratorGPIO( unsigned int minPulseWidth, unsigned int maxPulseWidth, const std::vector<unsigned int>& channelGPIOs ):
    PWMGenerator(minPulseWidth, maxPulseWidth, channelGPIOs.size()),
    mChannelGPIOs(),
    mInitializedOK(false)
{
    mChannelGPIOs = channelGPIOs;

    if (gpioInitialise()>=0 )
    {
        printf("PWMGeneratorGPIO: failed to initialize GPIO\n");
        mInitializedOK = true;

        gpioSetSignalFunc(SIGINT, PWMGeneratorGPIO_stop);   // This is global and nasty!
    }
}

PWMGeneratorGPIO::~PWMGeneratorGPIO() 
{
    if ( mInitializedOK ) 
    {
        gpioTerminate();
    }
}

bool PWMGeneratorGPIO::internalSetPulseWidth( std::size_t channelIndex, unsigned int pulseWidth )
{
    if ( !mInitializedOK ) 
    {
        return false;
    }

    int gpio = mChannelGPIOs[channelIndex];
    int result = gpioServo( gpio, pulseWidth );
    if ( result<0 )
    {
        return false;    
    }
    return true;
}

// !!!!!!!!!
void PWMGeneratorGPIO_stop(int signum)  
{
    printf("PWMGenerator::PWMGenerator_stop!\n");
    for ( std::size_t i=0; i<mPulseWidths.size(); i++ )
    {
        setPulseWidth( i, mMinPulseWidth );
    }

    gpioTerminate();
    exit(0);
}

#endif

        
   