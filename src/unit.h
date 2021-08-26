#pragma once

#include <Arduino.h>
#include "moisture_sensor.h"
#include "pump.h"

const uint8_t MIN_HUMIDITY = 70;
const uint8_t MAX_HUMIDITY = 85;

class Unit {
  private:
    MoistureSensor *_sensor;
    Pump *_pump;
    unsigned long _runFrequency;
    unsigned long _runDuration;
    unsigned long _lastRunTime;
    uint8_t _minHumidity;
    uint8_t _maxHumidity;
    bool _isRun;

  public:
    // Constructor
    Unit(MoistureSensor *sensor, Pump *pump, unsigned long runFrequency, unsigned long runDuration, uint8_t minHumidity = MIN_HUMIDITY, uint8_t maxHumidity = MAX_HUMIDITY)
      :  _sensor(sensor),
         _pump(pump),
         _runFrequency(runFrequency),
         _runDuration(runDuration),
         _minHumidity(minHumidity),
         _maxHumidity(maxHumidity)
    {
      _isRun = false;
      _lastRunTime = 0;
    }

    bool isRunning() {
      return _isRun;
    }

    void Run() {
      long timeSinceLastRun = millis() - _lastRunTime;

      Serial.println("Run unit");

      Serial.print("minHumidity: ");
      Serial.print(_minHumidity);
      Serial.print(" maxHumidity: ");
      Serial.print(_maxHumidity);
      Serial.print(" runFrequency: ");
      Serial.print(_runFrequency);
      Serial.print(" runDuration: ");
      Serial.print(_runDuration);
      Serial.print(" lastRunTime: ");
      Serial.print(_lastRunTime);
      Serial.print(" timeSinceLastRun: ");
      Serial.println(timeSinceLastRun);

      if (_lastRunTime == 0 || timeSinceLastRun >= _runFrequency) {
        _isRun = true;
        _lastRunTime = millis();

        return;
      }

      return;
    }

    void Execute() {
      if (!_isRun) {
        Serial.println("Skip execute unit don't run");

        return;
      }

      long pompRunTimeDuration = _pump->getRunTimeDuration();

      Serial.print("Pomp run time duration: ");
      Serial.println(pompRunTimeDuration);

      if (pompRunTimeDuration != 0 && pompRunTimeDuration > _runDuration) {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        Serial.println("Achive max run time duration, turn off sensor and pump");

        return;
      }

      _sensor->turnOn();

      int humidity = _sensor->get();

      Serial.print("Current humidity: ");
      Serial.println(humidity);

      if (humidity < 0) {
        return; // Sensor doesn't ready
      }

      if (humidity <= _minHumidity) {
        _pump->turnOn();

        return;
      }

      if (humidity >= _maxHumidity) {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        Serial.println("Achive max humidity, turn off sensor and pump");

        return;
      }

      if (_pump->isOn()) {
        Serial.println("Pomp steel work, return");

        return;
      }

      Serial.println("Humidity is good do nothing");

      _sensor->turnOff();
      _isRun = false;
    }
};