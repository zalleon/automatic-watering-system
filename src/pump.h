#pragma once

#include <Arduino.h>

class Pump
{
private:
  const byte _powerPin; // DigitalPin for power management
  bool _on;
  unsigned long _lastRunTime;

public:
  Pump(byte powerPin)
      : _powerPin(powerPin)
  {

    pinMode(powerPin, OUTPUT);

    _on = false;
    _lastRunTime = 0;
  }
  void turnOn()
  {
    if (!_on)
    {
      _on = true;
      _lastRunTime = millis();

      Serial.println("Turn on pomp");
      digitalWrite(_powerPin, HIGH);
    }
  }

  void turnOff()
  {
    if (_on)
    {
      _on = false;
      _lastRunTime = 0;

      Serial.println("Turn off pomp");
      digitalWrite(_powerPin, LOW);
    }
  }

  bool isOn()
  {
    return _on;
  }

  long getRunTimeDuration()
  {
    if (_lastRunTime > 0)
    {
      return millis() - _lastRunTime;
    }

    return 0;
  }
};