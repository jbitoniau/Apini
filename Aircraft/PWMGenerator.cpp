#include "PWMGenerator.h"

#include <stdio.h>
#include <cstdlib>      // for EXIT_SUCCESS 

#if LOCO_PLATFORM == LOCO_PLATFORM_LINUX

PWMGenerator::PWMGenerator()
    : pulseWidthIncrementInUs(10), 
      RPIOPWMChannelIndex(0)   // This is the RPIOPWM channel index, nothing to do with this PWMGenerator's notion of channel
{
    int hardware = DELAY_VIA_PWM;
    int channelSubcycleTimeInUs = 20000;
  
    // GPIO#   Pin#
    // 4       P1-7
    // 17      P1-11
    // 18      P1-12
    // 21      P1-13
    // 22      P1-15
    // 23      P1-16
    // 24      P1-18
    // 25      P1-22
    channelGPIOs.push_back( 25 );
    channelGPIOs.push_back( -1 );
    channelGPIOs.push_back( -1 );
    channelGPIOs.push_back( -1 );
    for ( std::size_t i=0; i<channelGPIOs.size(); i++ )
    {
        channelWidths[i] = minWidth;       // there should be a zero-position!
    }

    // Fatal errors in the rpio-pwm lib will not cause it to exit the program
    // The function will simply return EXIT_FAILURE and a description of the
    // problem can be obtained using get_error_message()
    set_softfatal(1);
    
    // Only report errors (not simple debug messages)
    set_loglevel(LOG_LEVEL_ERRORS);

    int result = setup(pulseWidthIncrementInUs, hardware);
    if ( result!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::PWMGenerator: setup failed. %s\n", get_error_message());
        return;
    }

    result = init_channel(RPIOPWMChannelIndex, channelSubcycleTimeInUs);  
    if ( result!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::PWMGenerator: init_channel failed. %s\n", get_error_message());
    }    
}

int PWMGenerator::getPulseWidthInUs( int channelIndex ) const
{
    // TODO: check index
    return channelWidths[channelIndex];
}   

bool PWMGenerator::setPulseWidthInUs( int channelIndex, int widthInUs )
{
    // TODO: check index
    // TODO: validate widthInUs
    int gpio = channelGPIOs[channelIndex];
    int widthInIncrements = widthInUs / pulseWidthIncrementInUs;
    int results = add_channel_pulse( RPIOPWMChannelIndex, gpio, 0, widthInIncrements);
    if ( results!=EXIT_SUCCESS )
    {
        printf("PWMGenerator::setPulseWidthInUs: add_channel_pulse failed. %s\n", get_error_message());
        return false;
    }
    channelWidths[channelIndex] = widthInUs;
    return true;
}
      
#else

PWMGenerator::PWMGenerator()
{
    int numChannels = 4;
    for ( std::size_t i=0; i<numChannels; i++ )
    {
        channelWidths.push_back(0);
    }
}
int PWMGenerator::getPulseWidthInUs( int channelIndex ) const
{
    return channelWidths[channelIndex];
}

bool PWMGenerator::setPulseWidthInUs( int channelIndex, int widthInUs )
{
    channelWidths[channelIndex] = widthInUs;
    return true;
}
#endif 
        
   