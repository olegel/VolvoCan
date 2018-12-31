#ifndef _CARSTATE_H_
#define _CARSTATE_H_

#include <Arduino.h>

namespace GearBox
{
enum Enum
{
    Unknown = 0,
    Parking = 1,
    Rear = 2,
    Neutral = 3,
    Drive = 4,
    DrivingRear = 7,
    DrivingNeutral = 0
};
}

struct CCarState
{
    static const uint8_t MaxParkingDistance = 0x1F;

    GearBox::Enum _gear = GearBox::Unknown;
    GearBox::Enum _drivingGear = GearBox::Unknown;
    bool _parkingAssistance = true;
    bool _parkingButtonPressed = false;
    uint8_t _lightMeter = 0;
    bool _keyInLock = false;
    uint8_t _keyPosition = 0;

    uint8_t _parkingDistance = MaxParkingDistance;
    uint16_t _wheelButtons = 0;
};

#endif
