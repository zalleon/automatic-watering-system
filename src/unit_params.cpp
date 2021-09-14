#include "unit_params.h"

UnitParams::UnitParams(uint8_t minHumidity, uint8_t maxHumidity, uint8_t runDuration, unsigned long runFrequency)
{
    _minHumidity = minHumidity;
    _maxHumidity = maxHumidity;
    _runDuration = runDuration;
    _runFrequency = runFrequency;

    _lastRunTime = 0;
    _currentHumidity = 0;
}

uint8_t UnitParams::getMinHumidity()
{
    return _minHumidity;
}

void UnitParams::setMinHumidity(uint8_t v)
{
    if (v > 100)
    {
        v = 100;
    }

    if (v < 0)
    {
        v = 0;
    }

    if (_minHumidity != v)
    {
        _minHumidity = v;
    }
}

void UnitParams::incMinHumidity()
{
    setMinHumidity(_minHumidity + 1);
}

void UnitParams::decMinHumidity()
{
    setMinHumidity(_minHumidity - 1);
}

uint8_t UnitParams::getMaxHumidity()
{
    return _maxHumidity;
}

void UnitParams::setMaxHumidity(uint8_t v)
{
    if (v > 100)
    {
        v = 100;
    }

    if (v < 0)
    {
        v = 0;
    }

    if (_maxHumidity != v)
    {
        _maxHumidity = v;
    }
}

void UnitParams::incMaxHumidity()
{
    setMaxHumidity(_maxHumidity + 1);
}

void UnitParams::decMaxHumidity()
{
    setMaxHumidity(_maxHumidity - 1);
}

uint8_t UnitParams::getRunDuration()
{
    return _runDuration;
}

void UnitParams::setRunDuration(uint8_t v)
{
    if (v > 60) // Max 60s
    {
        v = 60;
    }

    if (v < 1) // Min 1s
    {
        v = 1;
    }

    if (_runDuration != v)
    {
        _runDuration = v;
    }
}

void UnitParams::incRunDuration()
{
    setRunDuration(_runDuration + 1);
}

void UnitParams::decRunDuration()
{
    setRunDuration(_runDuration - 1);
}

unsigned long UnitParams::getRunFrequency()
{
    return _runFrequency;
}

void UnitParams::setRunFrequency(unsigned long v)
{
    if (v > 86400000) // Max 24h
    {
        v = 86400000;
    }

    if (v < 30000) // Min 30s
    {
        v = 30000;
    }

    if (_runFrequency != v)
    {
        _runFrequency = v;
    }
}

void UnitParams::incRunFrequency()
{
    if (_runFrequency < 900000)
    { // < 15min
        setRunFrequency(_runFrequency + 30000);
    }
    else if (_runFrequency < 3600000)
    {
        setRunFrequency(_runFrequency + 600000);
    }
    else
    {
        setRunFrequency(_runFrequency + 900000);
    }
}

void UnitParams::decRunFrequency()
{
    if (_runFrequency < 900000)
    { // < 15min
        setRunFrequency(_runFrequency - 30000);
    }
    else if (_runFrequency < 3600000)
    {
        setRunFrequency(_runFrequency - 600000);
    }
    else
    {
        setRunFrequency(_runFrequency - 900000);
    }
}

unsigned long UnitParams::getLastRunTime()
{
    return _lastRunTime;
}

void UnitParams::setLastRunTime(unsigned long v)
{
    if (_lastRunTime != v)
    {
        _lastRunTime = v;
    }
}

uint8_t UnitParams::getCurrentHumidity()
{
    return _currentHumidity;
}

void UnitParams::setCurrentHumidity(uint8_t v)
{
    if (_currentHumidity != v)
    {
        _currentHumidity = v;
    }
}
