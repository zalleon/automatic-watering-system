#pragma once

#include <Arduino.h>

class UnitParams
{
private:
  uint8_t _minHumidity;
  uint8_t _maxHumidity;
  uint8_t _runDuration; // How long pump may on. This restriction uses for prevent overflow pot
  unsigned long _runFrequency; // How often turn on waterring unit
  unsigned long _lastRunTime;
  uint8_t _currentHumidity;
  bool _isChanged;

public:
  UnitParams(uint8_t minHumidity, uint8_t maxHumidity, uint8_t runDuration, unsigned long runFrequency);

  uint8_t getMinHumidity();

  void setMinHumidity(uint8_t v);

  void incMinHumidity();

  void decMinHumidity();

  uint8_t getMaxHumidity();

  void setMaxHumidity(uint8_t v);

  void incMaxHumidity();

  void decMaxHumidity();

  uint8_t getRunDuration();

  void setRunDuration(uint8_t v);

  void incRunDuration();

  void decRunDuration();

  unsigned long getRunFrequency();

  void setRunFrequency(unsigned long v);

  void incRunFrequency();

  void decRunFrequency();

  unsigned long getLastRunTime();

  void setLastRunTime(unsigned long v);

  uint8_t getCurrentHumidity();

  void setCurrentHumidity(uint8_t v);
};