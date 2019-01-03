#include <Arduino.h>
#include <SPI.h>

#include "Setup.h"
#include "CarState.h"
#include "VolvoCan.h"
#include "WheelButtons.h"
#include "ParkingBuzzer.h"
#include "Radio.h"

CCarState _carState;
CParkingBuzzer _parkingBuzzer;
CWheelButtons _wheelButtons( PIN::PotSpiCs );
CVolvoCan _volvoCan( PIN::CanSpiCs, PIN::CanInterrupt );
CRadio _radio;

void setup() 
{
  Serial.begin(115200);
  Serial.println("setup");

  SPI.begin();

  if(!_volvoCan.Setup())
    Serial.println("VolvoCan failed");

  if( !_wheelButtons.Setup() )
    Serial.println("WheelButtons failed");

  _parkingBuzzer.Setup(PIN::Buzzer);

  _radio.Setup();

  delay(250);
  Serial.println("end");
}

void ProcessSerialData(const unsigned long &time);

void loop() 
{
  auto time = millis();

  _volvoCan.Process(time, _carState);
  _parkingBuzzer.Process( time, _carState._gear == GearBox::Rear && _carState._parkingAssistance, _carState._parkingDistance );
  _wheelButtons.Process( time, _carState._wheelButtons );
  _radio.Process(_carState._gear == GearBox::Rear, _carState._lightMeter, _carState._keyPosition);

  ProcessSerialData(time);
} 

void ProcessSerialData(const unsigned long &time)
{
  static bool wiperConnected = false;
  static unsigned long buttonTime = 0;
  static bool pod1 = false;


  if(wiperConnected && (time - buttonTime) > 200)
  {
    _carState._wheelButtons = 0;
    wiperConnected = false;
  }

  while(Serial.available())
  {
    int byte = Serial.read();
    continue;

    if(byte >= '1' && byte <= '6')
    {
        uint8_t b = byte - '1';
        _carState._wheelButtons = 1 << b;
        if(!pod1)
          _carState._wheelButtons <<= 8;

        wiperConnected = true;
        buttonTime = time;

        Serial.print("button ");
        Serial.println(_carState._wheelButtons, HEX);
        continue;
    }

    switch(byte)
    {
      case 'p':
        Serial.println("parking");
        _carState._gear = GearBox::Parking;
        break;
      case 'P':
        Serial.println("parking button");
        _carState._parkingAssistance = !_carState._parkingAssistance;
        break;
      case 'r':
        Serial.println("rear");
        _carState._gear = GearBox::Rear;
        break;
      case '[':
        Serial.print("p- ");
        if(_carState._parkingDistance > 0)
          _carState._parkingDistance--;
        Serial.println(_carState._parkingDistance);
        break;
      case ']':
        Serial.print("p+ ");
        if(_carState._parkingDistance < CCarState::MaxParkingDistance)
          _carState._parkingDistance++;
        Serial.println(_carState._parkingDistance);
        break;
      case '0':
        pod1 = !pod1;
        Serial.println("pod ");
        Serial.println(pod1);
        break;
      case 'l':
        if(_carState._lightMeter > 0xA )
          _carState._lightMeter = 1;
        else
          _carState._lightMeter = 0xF;
        Serial.println("light ");
        Serial.println(_carState._lightMeter);
        break;
      case 'k':
        if(_carState._keyPosition == 2 )
          _carState._keyPosition = 0;
        else
          _carState._keyPosition = 2;
        Serial.println("key ");
        Serial.println(_carState._keyPosition);
        break;
      default:
        Serial.println(byte);
        break;
    }
  }
}
