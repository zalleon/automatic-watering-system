#include <Arduino.h>
#include <GyverOS.h>
#include <GyverPower.h>
#include <GyverOLED.h>
#include <EncButton.h>
#include "menu.h"
#include "unit.h"
#include "unit_params.h"
#include "moisture_sensor.h"
#include "pump.h"

const uint32_t SLEEP_DELAY = 20000;

// https://github.com/GyverLibs/GyverOS
GyverOS<1> OS;

// https://github.com/GyverLibs/EncButton
EncButton<EB_TICK, PD3, PD4, PD2> enc;

// дефайн перед подключением либы - использовать microWire (лёгкая либа для I2C)
#define USE_MICRO_WIRE

// https://github.com/GyverLibs/GyverOLED
GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

bool wakeUpMenu;

UnitParams unitParams1(60, 85, 7, 43200);

Menu menu(&enc, &oled);

MoistureSensor sensor1(PD6, A1);

Pump pump1(PD5);

Unit unit1(&sensor1, &pump1, &unitParams1);

void process1()
{
  unit1.execute();
}

bool isAllUnitsDone()
{
  return !unit1.isRunning() && !menu.isActive();
}

void isr()
{
  wakeUpMenu = true;

  power.wakeUp();
  Serial.println("Power wake up");
}

void setup()
{
  Serial.begin(9600);

  // калибровка таймаутов для максимальной точности sleepDelay
  // вывести реальный макс. период
  // вывести реальный макс. период
  // Serial.println(power.getMaxTimeout());

  power.calibrate(9191);
  power.setSleepMode(POWERDOWN_SLEEP);

  unit1.run();

  menu.addUnit(&unitParams1);
  menu.turnOn();
  menu.printMenu();

  OS.attach(0, process1, 1000); // Check each second
}

void loop()
{
  OS.tick();
  menu.tick();

  // Check all process done
  // Deep sleep
  if (isAllUnitsDone())
  {
    Serial.println("Everything done, go to sleep");
    attachInterrupt(0, isr, FALLING);
    Serial.println("WakeUp interrupt attached");

    // Wait for print to serial
    delay(100);

    power.sleepDelay(SLEEP_DELAY);

    detachInterrupt(0);
    Serial.println("WakeUp interrupt deattached, start cycle");

    // Renew process tikens
    unit1.run();

    if (wakeUpMenu) {
      menu.wakeUp();
      
      wakeUpMenu = false;
    }
  }
}
