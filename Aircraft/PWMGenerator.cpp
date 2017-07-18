#include "PWMGenerator.h"

#include <stdio.h>
#include <cstdlib>      // for EXIT_SUCCESS 

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX

 // GPIO#   Pin#
// 4       P1-7
// 17      P1-11
// 18      P1-12
// 21      P1-13
// 22      P1-15
// 23      P1-16
// 24      P1-18
// 25      P1-22

PWMGenerator::PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs ):
    mChannelGPIOs(),
    mChannelWidths(),
    mPulseWidthIncrementInUs(10), 
    mRPIOPWMChannelIndex(0)   // This is the RPIOPWM channel index, nothing to do with this PWMGenerator's notion of channel
{


    int hardware = DELAY_VIA_PWM;
    int channelSubcycleTimeInUs = 20000;
   
    mChannelGPIOs = channelGPIOs;
    for ( std::size_t i=0; i<mChannelGPIOs.size(); i++ )
    {
        mChannelWidths.push_back(mMinPulseWidthInUs);
    }
 
    // Fatal errors in the rpio-pwm lib will not cause it to exit the program
    // The function will simply return EXIT_FAILURE and a description of the
    // problem can be obtained using get_error_message()
    set_softfatal(1);
    
    // Only report errors (not simple debug messages)
    set_loglevel(LOG_LEVEL_ERRORS);

    int result = setup(mPulseWidthIncrementInUs, hardware);
    if ( result!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::PWMGenerator: setup failed. %s\n", get_error_message());
        return;
    }

    result = init_channel(mRPIOPWMChannelIndex, channelSubcycleTimeInUs);  
    if ( result!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::PWMGenerator: init_channel failed. %s\n", get_error_message());
    }    
}

unsigned int PWMGenerator::getPulseWidthInUs( std::size_t channelIndex ) const
{
    if ( channelIndex>=mChannelWidths.size() )
    {
        printf("PWMGenerator::getPulseWidthInUs: index out of range\n");
        return mMinPulseWidthInUs;         
    }
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
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d is less than minimum\n", widthInUs);
        widthInUs = mMinPulseWidthInUs;
    }
    else if ( widthInUs>mMaxPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d is greater than maximum\n", widthInUs);
        widthInUs = mMaxPulseWidthInUs;
    }
    
    int gpio = mChannelGPIOs[channelIndex];
    int widthInIncrements = widthInUs / mPulseWidthIncrementInUs;
    int results = add_channel_pulse( mRPIOPWMChannelIndex, gpio, 0, widthInIncrements);
    if ( results!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::setPulseWidthInUs: add_channel_pulse failed. %s\n", get_error_message());
        return false;
    }
    mChannelWidths[channelIndex] = widthInUs;
    return true;
}
      
#else

PWMGenerator::PWMGenerator( unsigned int minPulseWidthInUs, unsigned int maxPulseWidthInUs, const std::vector<unsigned int>& channelGPIOs ):
    mMinPulseWidthInUs(0),
    mMaxPulseWidthInUs(0),
    mChannelGPIOs(),
    mChannelWidths(),
    mPulseWidthIncrementInUs(10), 
    mRPIOPWMChannelIndex(0) 
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
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %d is less than minimum %d\n", widthInUs, channelIndex, mMinPulseWidthInUs );
        widthInUs = mMinPulseWidthInUs;
    }
    else if ( widthInUs>mMaxPulseWidthInUs )
    {
        printf("PWMGenerator::setPulseWidthInUs: widthInUs %d for channel %d is greater than minimum %d\n", widthInUs, channelIndex, mMaxPulseWidthInUs );
        widthInUs = mMaxPulseWidthInUs;
    }

    mChannelWidths[channelIndex] = widthInUs;
    return true;
}
#endif 
        
   