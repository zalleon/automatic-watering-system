#include <Arduino.h>
#include <GyverOS.h>
#include <GyverPower.h>
#include <GyverOLED.h>
#include <EncButton.h>
#include "unit.h"
#include "moisture_sensor.h"
#include "pump.h"


const unsigned long RUN_FREQUENCY = 10000; // 1s = 1000 How often turn on waterring unit 
const unsigned long RUN_DURATION = 7000;  // 1s = 1000 How long pump may on. This restriction uses for prevent overflow pot
const uint32_t SLEEP_DELAY = 20000;

// https://github.com/GyverLibs/GyverOS
GyverOS<1> OS;

// https://github.com/GyverLibs/EncButton
EncButton<EB_TICK, PD4, PD5, PD6> encoder;

// https://github.com/GyverLibs/GyverOLED
GyverOLED<SSH1106_128x64> oled;

MoistureSensor sensor1(PD2, A1);

Pump pump1(PD3);

Unit unit1(&sensor1, &pump1, RUN_FREQUENCY, RUN_DURATION); // Use defaul unit humidity

void process1() {
  unit1.Execute();

  // Vaoltage level from analog read port
  int value = 0;
  float voltage;
  float perc;

  value = analogRead(A0);
  voltage = value * 5.0/1023;
  perc = map(voltage, 3.6, 4.2, 0, 100);
  Serial.print("Voltage= ");
  Serial.println(voltage);
  Serial.print("Battery level= ");
  Serial.print(perc);
  Serial.println(" %");

  oled.clear();
  oled.home();
  oled.print("Voltage: ");
  oled.print(voltage);
  oled.update();
}

bool isAllUnitsDone() {
  return !unit1.isRunning();
}

void setup() {
  Serial.begin(9600);

  // калибровка таймаутов для максимальной точности sleepDelay
  // вывести реальный макс. период
  // вывести реальный макс. период
  // Serial.println(power.getMaxTimeout());

  power.calibrate(9191);
  power.setSleepMode(POWERDOWN_SLEEP);

  // Display init
  oled.init();
  oled.clear();

  // Encoder init
  encoder.counter = 100;

  unit1.Run();

  OS.attach(0, process1, 1000);  // Check each second
}

void loop() {
  OS.tick();
  encoder.tick();

  // ------------=Start encoder example=------------
  if (encoder.isTurn()) {               // любой поворот
    Serial.print("turn ");
    Serial.println(encoder.counter);    // вывод счётчика
  }

  if (encoder.isLeft()) {
    if (encoder.isFast()) Serial.println("fast left");
    else Serial.println("left");
  }

  if (encoder.isRight()) {
    if (encoder.isFast()) Serial.println("fast right");
    else Serial.println("right");
  }

  if (encoder.isLeftH()) Serial.println("leftH");
  if (encoder.isRightH()) Serial.println("rightH");
  if (encoder.isClick()) Serial.println("click");
  if (encoder.isHolded()) Serial.println("holded");
  if (encoder.isStep()) Serial.println("step");

  if (encoder.isPress()) Serial.println("press");
  if (encoder.isClick()) Serial.println("click");
  if (encoder.isRelease()) Serial.println("release");

  if (encoder.hasClicks(1)) Serial.println("1 click");
  if (encoder.hasClicks(2)) Serial.println("2 click");
  if (encoder.hasClicks(3)) Serial.println("3 click");
  if (encoder.hasClicks(5)) Serial.println("5 click");

  if (encoder.hasClicks()) Serial.println(encoder.clicks);
  // -----------=End encoder example=-------------
  
  // Check all process done
  // Deep sleep
  // if (isAllUnitsDone()) {
  //   Serial.println("Everything done, go to sleep");
  //   delay(100);

  //   power.sleepDelay(SLEEP_DELAY);

  //   Serial.println("Wake up and start cycle");
  //   delay(100);

  //   // Renew process tikens
  //   unit1.Run();
  // }
}
