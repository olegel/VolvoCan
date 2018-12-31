#ifndef _RADIO_H_
#define _RADIO_H_

#include "Setup.h"

class CRadio
{
public:
    void Setup()
    {
          pinMode(PIN::RadioRearGear, OUTPUT);
          pinMode(PIN::RadioIllum, OUTPUT);
          pinMode(PIN::KeyPos2, OUTPUT);
          digitalWrite(PIN::RadioRearGear, HIGH);
          digitalWrite(PIN::RadioIllum, LOW);
          digitalWrite(PIN::KeyPos2, LOW);
    }

    void Process( bool rearGear, uint8_t lightMeter, uint8_t keyPos )
    {
        if(_rearGear != rearGear)
        {
            _rearGear = rearGear;
            if(_rearGear)
                digitalWrite(PIN::RadioRearGear, LOW);
            else
                digitalWrite(PIN::RadioRearGear, HIGH);
        }

        bool illum = lightMeter < 0xA;

        if(illum != _illum)
        {
            _illum = illum;
            if(_illum)            
                digitalWrite(PIN::RadioIllum, LOW);
            else
                digitalWrite(PIN::RadioIllum, HIGH);
        }

        bool keyOn = keyPos == 1 || keyPos == 2;
        if(keyOn != _keyOn)
        {
            _keyOn = keyOn;
            if(_keyOn)            
                digitalWrite(PIN::KeyPos2, HIGH);
            else
                digitalWrite(PIN::KeyPos2, LOW);
        }
    }

private:
    bool _rearGear = false;
    bool _illum = false;
    bool _keyOn = false;
};

#endif