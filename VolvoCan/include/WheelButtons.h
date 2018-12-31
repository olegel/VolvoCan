#ifndef _WHEELBUTTONS_H_
#define _WHEELBUTTONS_H_

#include "mcp4xxx.h"

class CWheelButtons
{

public:
    CWheelButtons(uint8_t potPinCs) : 
        _pot1( potPinCs, icecave::arduino::MCP4XXX::pot_0,  icecave::arduino::MCP4XXX::Resolution::res_7bit ),
        _pot2( potPinCs, icecave::arduino::MCP4XXX::pot_1,  icecave::arduino::MCP4XXX::Resolution::res_7bit )
    {
    }

    bool Setup()
    {
        bool ok = true;
        ok = ok && _pot1.set_terminal_a_status(true);
        ok = ok && _pot2.set_terminal_a_status(true);

        ok = ok && _pot1.set_wiper_status(false);
        ok = ok && _pot2.set_wiper_status(false);
        return ok;
    }

    void Process( const unsigned long &time, const uint16_t &wheelButtons )
    {
        if( _lastWheelButtons == wheelButtons )
            return;

        _lastWheelButtons = wheelButtons;

        static const uint8_t POTVALUES[] = { 10, 40, 60, 80, 100, 127 };

        if(wheelButtons)
        {
            uint16_t buttons = wheelButtons;
            for(uint8_t b = 0; b < 16; b++)            
            {
                uint8_t valueId = b % 8;
                if(buttons & 1)
                {
                    if(valueId < 6)
                    {
                        uint8_t potId = b / 8;
                        icecave::arduino::MCP4XXX &pot = potId == 0 ? _pot1 : _pot2;
                        pot.set( POTVALUES[valueId] );
                        pot.set_wiper_status(true);
                        Serial.print("wiper ");
                        Serial.print(potId);
                        Serial.print(" value ");
                        Serial.print(POTVALUES[valueId]);
                        Serial.print(" button ");
                        Serial.println(b);
                    }
                }
                buttons >>= 1;                 
            }
        }
        else
        {
            _pot1.set_wiper_status(false);
            _pot2.set_wiper_status(false);
            Serial.println("wipers off");
        }
    }

private:
    icecave::arduino::MCP4XXX _pot1;
    icecave::arduino::MCP4XXX _pot2;

    uint16_t _lastWheelButtons = 0;
};

#endif