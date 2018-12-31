#ifndef _VOLVOCAN_H_
#define _VOLVOCAN_H_

#include "mcp2515.h"
#include "CarState.h"

class CVolvoCan
{
public:
    CVolvoCan(uint8_t pinSpiCs, uint8_t pinInterrupt) : _mcp2515( pinSpiCs ), _pinInterrupt(pinInterrupt)
    {
    }

    static void OnInterrupt()
    {
        _onInterrupt = true;
    }

    bool Setup()
    {
        pinMode(_pinInterrupt, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(2), OnInterrupt, FALLING );

        if ( MCP2515::ERROR_OK != _mcp2515.reset() )
            return false;
        if ( MCP2515::ERROR_OK != _mcp2515.setBitrate( CAN_125KBPS, MCP_8MHZ ) )
            return false;
        if ( MCP2515::ERROR_OK != _mcp2515.setListenOnlyMode() )
            return false;

        return true;
    }

    void Process(const unsigned long &time, CCarState &carState)
    {
        if(!_onInterrupt)
           return;
        _onInterrupt = false;
        //Serial.println("Interrupt");

        while(true)
        {
            uint8_t irq = _mcp2515.getInterrupts();
            if(irq == 0)
                break;
            // Serial.print("irq ");
            // Serial.println(irq, HEX);

            if(irq & MCP2515::CANINTF_ERRIF)
            {
                uint8_t error = _mcp2515.getErrorFlags();
                _mcp2515.clearRXnOVRFlags();
                _mcp2515.clearERRIF();
                if(error & MCP2515::EFLG_RX0OVR) // ignore EFLG_RX0OVR
                {
                    Serial.print("Can ERRIF ");
                    Serial.println(error, HEX);
                }
            }
            if(irq & MCP2515::CANINTF_MERRF)
            {
                _mcp2515.clearMERR();
                Serial.println("Can MERRF");
            }

            if (irq & MCP2515::CANINTF_RX0IF)
            {
                if (_mcp2515.readMessage(MCP2515::RXB0, &_canMsg) == MCP2515::ERROR_OK) 
                    ProcessMessage(_canMsg, carState);
            }
                
            if (irq & MCP2515::CANINTF_RX1IF) 
            {
                if (_mcp2515.readMessage(MCP2515::RXB1, &_canMsg) == MCP2515::ERROR_OK) 
                    ProcessMessage(_canMsg, carState);
            }
        }
    }

private:
    static const unsigned long GearBoxId = 0x3003028;
    static const unsigned long CcmId = 0xE00442;
    static const unsigned long RemId = 0x1E0522E;
    static const unsigned long CemId = 0x617FF8;
    static const unsigned long SwmId = 0x404066;

    void ProcessMessage(can_frame &msg, CCarState &carState)
    {
        msg.can_id &= CAN_EFF_MASK;

        //PrintMessage(msg);

        switch(msg.can_id)
        {
            case GearBoxId:
            {
                GearBox::Enum gear = (GearBox::Enum)(msg.data[2] & 0x7);
                if( carState._gear != gear )
                {
                    carState._gear = gear;
                    Serial.print("Gear ");
                    Serial.println(carState._gear);
                }
                gear = (GearBox::Enum)((msg.data[2] & 0x70) >> 5);
                if( carState._drivingGear != gear )
                {
                    carState._drivingGear = gear;
                    Serial.print("Gear2 ");
                    Serial.println(carState._drivingGear);
                }
                break;
            }
            case CcmId:
            {
                bool parkingButtonPressed = (msg.data[2] & 0x40) > 0;

                if(parkingButtonPressed != carState._parkingButtonPressed)
                {
                    carState._parkingButtonPressed = parkingButtonPressed;
                    if(parkingButtonPressed)
                        carState._parkingAssistance = ! carState._parkingAssistance;

                    Serial.print("ParkButton ");
                    Serial.println(parkingButtonPressed);
                    Serial.print(" enabled ");
                    Serial.println(carState._parkingAssistance);
                }

                uint8_t light = msg.data[3] & 0xF;
                if( carState._lightMeter != light )
                {
                    carState._lightMeter = light;
                    Serial.print("LightMeter ");
                    Serial.println(carState._lightMeter);
                }
                break;
            }
            case RemId:
            {
                uint8_t distance = CCarState::MaxParkingDistance - (msg.data[3] >> 3);
                //uint8_t distance = msg.data[3] >> 3;
                if( carState._parkingDistance != distance )
                {
                    carState._parkingDistance = distance;
                    Serial.print("Distance ");
                    Serial.println(carState._parkingDistance);
                }
                break;
            }
            case CemId:
            {
                uint8_t keyPosition = msg.data[6] >> 5;
                bool key = (keyPosition & 0x4) > 0;
                if( carState._keyInLock != key )
                {
                    carState._keyInLock = key;
                    Serial.print("Key ");
                    Serial.println(carState._keyInLock);
                }
                keyPosition = keyPosition & 0x3;
                if( carState._keyPosition != keyPosition )
                {
                    carState._keyPosition = keyPosition;
                    Serial.print("Key pos ");
                    Serial.println(carState._keyPosition);
                }
                break;
            }
            case SwmId:
            {
                uint16_t buttons = msg.data[6] & 0x3F;
                buttons = (buttons<<8) | ((~msg.data[7]) & 0x3F);
                if( carState._wheelButtons != buttons )
                {
                    carState._wheelButtons = buttons;
                    Serial.print("Buttons ");
                    Serial.println(carState._wheelButtons, HEX);
                }
                break;
            }
            default:
                //PrintMessage(msg);
                break;
        }
    }

    void PrintMessage(can_frame &msg)
    {
        Serial.print(msg.can_id & CAN_EFF_MASK, HEX); // print ID
        Serial.print(" "); 
        Serial.print(msg.can_dlc, HEX); // print DLC
        Serial.print(" ");
        
        for (int i = 0; i<msg.can_dlc; i++)  {  // print the data
            
        Serial.print(msg.data[i],HEX);
        Serial.print(" ");

        }

        Serial.println();      
    }

private:
    static volatile bool _onInterrupt;
    MCP2515 _mcp2515;
    uint8_t _pinInterrupt;
    struct can_frame _canMsg;

};

volatile bool CVolvoCan::_onInterrupt = true;

#endif