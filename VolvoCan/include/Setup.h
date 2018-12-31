#ifndef _SETUP_H_
#define _SETUP_H_


#include <Arduino.h>

namespace PIN
{
    enum Enum { Buzzer = 3, Led = LED_BUILTIN, PotSpiCs = 7, CanSpiCs = 10, CanInterrupt = 2, RadioRearGear = 5, RadioIllum = 4, KeyPos2 = 6 };
}

#endif