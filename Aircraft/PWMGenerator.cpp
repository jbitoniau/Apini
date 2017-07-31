#include "PWMGenerator.h"

#include <stdio.h>
#include <cstdlib>      // for EXIT_SUCCESS 

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX
#include <pigpio.h>
#include <signal.h>     // for SIGINT
 // GPIO#   Pin#
// 4       P1-7
// 17      P1-11
// 18      P1-12
// 21      P1-13
// 22      P1-15
// 23      P1-16
// 24      P1-18
// 25      P1-22

void PWMGenerator_stop(int signum)
{
    // !!!!!!!!!
    printf("PWMGenerator::PWMGenerator_stop!\n");
    gpioTerminate();
    exit(0);
}

PWMGenerator::PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs ):
    mMinPulseWidthInUs(minPulseWidthInUs),
    mMaxPulseWidthInUs(maxPulseWidthInUs),
    mChannelGPIOs(),
    mChannelWidths()
    // mPulseWidthIncrementInUs(10), 
    // mRPIOPWMChannelIndex(0)   // This is the RPIOPWM channel index, nothing to do with this PWMGenerator's notion of channel
{
    mChannelGPIOs = channelGPIOs;
    for ( std::size_t i=0; i<mChannelGPIOs.size(); i++ )
    {
        mChannelWidths.push_back(mMinPulseWidthInUs);
    }

    if (gpioInitialise()<0 )
    {
        printf("PWMGenerator::PWMGenerator: gpioInitialise failed\n");
        return;
    }
    gpioSetSignalFunc(SIGINT, PWMGenerator_stop);

    for ( std::size_t i=0; i<mChannelGPIOs.size(); i++ )
    {
        unsigned int pulseWidth = mChannelWidths[i];
        setPulseWidthInUs( i, pulseWidth );
    }

    // int hardware = DELAY_VIA_PWM;
    // int channelSubcycleTimeInUs = 20000;            // TODO: pass that as an argument
   
    // mChannelGPIOs = channelGPIOs;
    // for ( std::size_t i=0; i<mChannelGPIOs.size(); i++ )
    // {
    //     mChannelWidths.push_back(mMinPulseWidthInUs);
    // }
 
    // // Fatal errors in the rpio-pwm lib will not cause it to exit the program
    // // The function will simply return EXIT_FAILURE and a description of the
    // // problem can be obtained using get_error_message()
    // set_softfatal(1);
    
    // // Only report errors (not simple debug messages)
    // set_loglevel(LOG_LEVEL_ERRORS);

    // int result = setup(mPulseWidthIncrementInUs, hardware);
    // if ( result!=EXIT_SUCCESS )
    // {
    //     printf("PWMGenerator::PWMGenerator: setup failed. %s\n", get_error_message());
    //     return;
    // }

    // result = init_channel(mRPIOPWMChannelIndex, channelSubcycleTimeInUs);  
    // if ( result!=EXIT_SUCCESS )
    // {
    //     printf("PWMGenerator::PWMGenerator: init_channel failed. %s\n", get_error_message());
    // }    
}

unsigned int PWMGenerator::getPulseWidthInUs( std::size_t channelIndex ) const
{
    if ( channelIndex>=mChannelWidths.size() )
    {
        printf("PWMGenerator::setPulseWidthInUs: index %d out of range %d\n", channelIndex, mChannelWidths.size());
        return mMinPulseWidthInUs;         
    }
    return mChannelWidths[channelIndex];
}   

bool PWMGenerator::setPulseWidthInUs( std::size_t channelIndex, unsigned int widthInUs )
{
    if ( channelIndex>=mChannelWidths.size() )
    {
        printf("PWMGenerator::setPulseWidthInUs: index %d out of range %d\n", channelIndex, mChannelWidths.size());
        return mMinPulseWidthInUs;         
    }
   
    if ( widthInUs<mMinPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %d is less than minimum %d\n", widthInUs, channelIndex, mMinPulseWidthInUs );
        widthInUs = mMinPulseWidthInUs;
    }
    else if ( widthInUs>mMaxPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %d is greater than minimum %d\n", widthInUs, channelIndex, mMaxPulseWidthInUs );
        widthInUs = mMaxPulseWidthInUs;
    }
    
    // int gpio = mChannelGPIOs[channelIndex];
    // int widthInIncrements = widthInUs / mPulseWidthIncrementInUs;
    // //printf("PWMGenerator::setPulseWidthInUs: add_channel_pulse(%d, %d, 0, %d)\n", mRPIOPWMChannelIndex, gpio, widthInIncrements);
    // int results = add_channel_pulse( mRPIOPWMChannelIndex, gpio, 0, widthInIncrements);
    // if ( results!=EXIT_SUCCESS )
    // {
    //     printf("PWMGenerator::setPulseWidthInUs: add_channel_pulse failed. %s\n", get_error_message());
    //     return false;
    // }
    
    int gpio = mChannelGPIOs[channelIndex];
    int result = gpioServo( gpio, widthInUs );
    if ( result<0 )
    {
        printf("PWMGenerator::setPulseWidthInUs: gpioServo failed\n");
        return false;    
    }

    mChannelWidths[channelIndex] = widthInUs;
    return true;
}
      
#else

/*
    NOP PWMGenerator implementation
*/
PWMGenerator::PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs ):
    mMinPulseWidthInUs(minPulseWidthInUs),
    mMaxPulseWidthInUs(maxPulseWidthInUs),
    mChannelGPIOs(),
    mChannelWidths()
{
    if ( mMinPulseWidthInUs<=mMaxPulseWidthInUs )
    {
        mMinPulseWidthInUs = minPulseWidthInUs;
        mMaxPulseWidthInUs = maxPulseWidthInUs;
    }
    else
    {
        mMinPulseWidthInUs = maxPulseWidthInUs;
        mMaxPulseWidthInUs = minPulseWidthInUs;     
        printf("PWMGenerator::PWMGenerator: min and max pulse widths are the wrong way round. Values swapped\n");
    }

    mChannelGPIOs = channelGPIOs;
    for ( std::size_t i=0; i<mChannelGPIOs.size(); i++ )
    {
        mChannelWidths.push_back(mMinPulseWidthInUs);  
    }
}

unsigned int PWMGenerator::getPulseWidthInUs( std::size_t channelIndex ) const
{
    if ( channelIndex>=mChannelWidths.size() )
        return mMinPulseWidthInUs;
    return mChannelWidths[channelIndex];
}

bool PWMGenerator::setPulseWidthInUs( std::size_t channelIndex, unsigned int widthInUs )
{
    if ( channelIndex>=mChannelWidths.size() )
    {
        printf("PWMGenerator::setPulseWidthInUs: index out of range\n");
        return mMinPulseWidthInUs;         
    }
   
    if ( widthInUs<mMinPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %lu is less than minimum %d\n", widthInUs, channelIndex, mMinPulseWidthInUs );
        widthInUs = mMinPulseWidthInUs;
    }
    else if ( widthInUs>mMaxPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %lu is greater than minimum %d\n", widthInUs, channelIndex, mMaxPulseWidthInUs );
        widthInUs = mMaxPulseWidthInUs;
    }

    mChannelWidths[channelIndex] = widthInUs;
    return true;
}
#endif 
        
   