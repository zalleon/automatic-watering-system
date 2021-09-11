#pragma once

#include <Arduino.h>
#include "moisture_sensor.h"
#include "pump.h"
#include "unit_params.h"

class Unit {
  private:
    MoistureSensor *_sensor;
    Pump *_pump;
    UnitParams *_params;
    bool _isRun;

  public:
    // Constructor
    Unit(MoistureSensor *sensor, Pump *pump, UnitParams *params);

    bool isRunning();

    void run();

    void execute();
};