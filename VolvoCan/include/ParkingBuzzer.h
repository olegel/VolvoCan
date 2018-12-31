#ifndef _PARKINGBUZZER_H_
#define _PARKINGBUZZER_H_

#include "CarState.h"
#include "Tone.h"

class CParkingBuzzer
{
public:
  void Setup(uint8_t pin)
  {
    _tone.begin(pin);
  }

  void Process( const unsigned long &time, bool enabled, uint8_t distance )
  {
    bool wasEnabled = _isEnabled;
    _isEnabled = enabled && distance < CCarState::MaxParkingDistance;
    bool isDistanceUpdated = _lastDistance != distance;
    _lastDistance = distance;

    if(wasEnabled != _isEnabled)
    {
      if(!_isEnabled)
        Stop();
    }

    if(!_isEnabled)
      return;

    if(_mode == modeOff)
    {
       Beep(time);
       return;
    }

    unsigned long elapsed = time - _modeStartTime;
    if(_mode == modeBeep)
    {
      if(elapsed > BeepInterval)
        Silence(time);
    }
    else // silence
    {
      if(isDistanceUpdated)
        UpdateSilenceInterval();
      if(elapsed > _silenceInterval)
        Beep(time);
    }
  }

private:
  enum Mode { modeOff, modeBeep, modeSilence };

  static const unsigned long MaxSilenceInterval = 2000;
  static const uint32_t BeepInterval = 200;

  Tone _tone;
  Mode _mode = modeOff;
  bool _isEnabled = false;
  unsigned long _modeStartTime;
  unsigned long _silenceInterval;
  uint8_t _lastDistance;


  void Beep( const unsigned long &time )
  {
    _mode = modeBeep;
    _modeStartTime = time;
    _tone.play(NOTE_B5, BeepInterval + 5);
  }

  void Silence( const unsigned long &time )
  {
    UpdateSilenceInterval();
    _mode = modeSilence;
    _modeStartTime = time;
  }

  void UpdateSilenceInterval()
  {
    Serial.print("dist =");
    Serial.print(_lastDistance);
    _silenceInterval = MaxSilenceInterval * _lastDistance / CCarState::MaxParkingDistance;
    Serial.print(" sil=");
    Serial.println(_silenceInterval);
  }

  void Stop()
  {
    _mode = modeOff;
    _tone.stop();
  }

};


#endif