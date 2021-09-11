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

    // Serial.println("Run unit");
    // Serial.print("minHumidity: ");
    // Serial.print(_params->getMinHumidity());
    // Serial.print(" maxHumidity: ");
    // Serial.print(_params->getMaxHumidity());
    // Serial.print(" runFrequency: ");
    // Serial.print(_params->getRunFrequency());
    // Serial.print(" runDuration: ");
    // Serial.print(_params->getRunDuration());
    // Serial.print(" lastRunTime: ");
    // Serial.print(_params->getLastRunTime());
    // Serial.print(" timeSinceLastRun: ");
    // Serial.println(timeSinceLastRun);

    if (_params->getLastRunTime() == 0 || timeSinceLastRun >= _params->getRunFrequency())
    {
        _isRun = true;
        _params->setLastRunTime(millis());

        return;
    }

    return;
}

void Unit::execute()
{
    if (!_isRun)
    {
        // Serial.println("Skip execute unit doesn't run");
        return;
    }

    long pompRunTimeDuration = _pump->getRunTimeDuration();

    // Serial.print("Pomp run time duration: ");
    // Serial.println(pompRunTimeDuration);

    if (pompRunTimeDuration != 0 && pompRunTimeDuration > _params->getRunDuration())
    {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        // Serial.println("Achive max run time duration, turn off sensor and pump");

        return;
    }

    _sensor->turnOn();

    int humidity = _sensor->get();

    // Serial.print("Current humidity: ");
    // Serial.println(humidity);

    _params->setCurrentHumidity(humidity);

    if (humidity < 0)
    {
        return; // Sensor doesn't ready
    }

    if (humidity <= _params->getMinHumidity())
    {
        _pump->turnOn();

        return;
    }

    if (humidity >= _params->getMaxHumidity())
    {
        _sensor->turnOff();
        _pump->turnOff();
        _isRun = false;

        // Serial.println("Achive max humidity, turn off sensor and pump");
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
