#include "unit.h"
#include "unit_params.h"

Unit::Unit(MoistureSensor *sensor, Pump *pump, UnitParams *params)
{
    _sensor = sensor;
    _pump = pump;
    _params = params;

    _isRun = false;
}

bool Unit::isRunning()
{
    return _isRun;
}

void Unit::run()
{
    long timeSinceLastRun = millis() - _params->getLastRunTime();

    Serial.print(F("TimeSinceLastRun: "));
    Serial.print(timeSinceLastRun);
    Serial.print(F(" runFrequency: "));
    Serial.println(_params->getRunFrequency());

    if (_params->getLastRunTime() == 0 || timeSinceLastRun >= _params->getRunFrequency())
    {
        _isRun = true;
        return;
    }

    return;
}

void Unit::execute()
{
    if (!_isRun)
    {
        return;
    }

    long pompRunTimeDuration = _pump->getRunTimeDuration();

    Serial.print(F("Pomp run time duration: "));
    Serial.println(pompRunTimeDuration);

    if (pompRunTimeDuration != 0 && pompRunTimeDuration > (_params->getRunDuration() * 1000))
    {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        Serial.println(F("Achive max run time duration, turn off sensor and pump"));

        return;
    }

    _sensor->turnOn();

    int humidity = _sensor->get();

    Serial.print(F("Current humidity: "));
    Serial.println(humidity);

    _params->setCurrentHumidity(humidity);

    if (humidity < 0)
    {
        return; // Sensor doesn't ready
    }

    if (humidity <= _params->getMinHumidity())
    {
        _pump->turnOn();
        _params->setLastRunTime(millis());

        return;
    }

    if (humidity >= _params->getMaxHumidity())
    {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        Serial.println(F("Achive max humidity, turn off sensor and pump"));
        return;
    }

    if (_pump->isOn())
    {
        // Serial.println("Pomp steel work, return");
        return;
    }

    // Serial.println("Humidity is good do nothing");

    _sensor->turnOff();
    _isRun = false;
}
