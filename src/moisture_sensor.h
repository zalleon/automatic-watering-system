#pragma once

#include <Arduino.h>

#define _MOISTURE_SENSOR_INIT_TIMEOUT 500   // initialization timeout
#define _MOISTURE_SENSOR_MIN_VALUE 943      // Sensor value on air
#define _MOISTURE_SENSOR_MAX_VALUE 431      // Sensor value in water

class MoistureSensor {
  public:
    MoistureSensor(byte powerPin, byte dataPin)
      : _powerPin(powerPin), _dataPin(dataPin) {
      pinMode(powerPin, OUTPUT);

      _on = false;
    }

    void turnOn() {
      if (!_on) {
        _on = true;
        _tmr = millis();

        Serial.println("Turn on sensor");
        digitalWrite(_powerPin, HIGH);
      }
    }

    void turnOff() {
      if (_on) {
        _on = false;

        Serial.println("Turn off sensor");
        digitalWrite(_powerPin, LOW);
      }
    }

    bool isReady() {
      return _on && millis() - _tmr >= _MOISTURE_SENSOR_INIT_TIMEOUT;
    }

    bool isOn() {
      return _on;
    }

    int8_t get() {
      if (!isReady()) {
        return -1;
      }

      // Read sensor data
      int rawValue = analogRead(_dataPin);

      //Serial.print("Raw humidity: ");
      //Serial.println(rawValue);

      // Convert raw data to percents
      int8_t value = map(rawValue, _MOISTURE_SENSOR_MIN_VALUE, _MOISTURE_SENSOR_MAX_VALUE, 0, 100);

      return value;
    }

  private:
    const byte _powerPin;  // DigitalPin for power management
    const byte _dataPin;   // AnalogPin for read data
    bool _on;
    uint32_t _tmr;
};