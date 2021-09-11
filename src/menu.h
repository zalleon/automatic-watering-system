#pragma once

#include <Arduino.h>
#include <GyverOLED.h>
#include <EncButton.h>
#include "unit_params.h"

#define INIT_ADDR 1023  // номер резервной ячейки
#define INIT_KEY 2     // ключ первого запуска. 0-254, на выбор

#define MAIN_MENU_MAX_ITEMS 4
#define MAIN_MENU_LINE_OFFSET 1

const char mainMenuItem0[] PROGMEM = "  Unit1 ";
const char mainMenuItem1[] PROGMEM = "  Unit2 ";
const char mainMenuItem2[] PROGMEM = "  Unit3 ";
const char mainMenuItem3[] PROGMEM = "  Unit4 ";

const char *const mainMenu[] PROGMEM =
    {
        mainMenuItem0,
        mainMenuItem1,
        mainMenuItem2,
        mainMenuItem3,
};

#define SUB_MENU_ITEMS 6
#define SUB_MENU_EXIT_ITEM_INDEX 5
#define SUB_MENU_LINE_OFFSET 1

const char subMenuItem0[] PROGMEM = "Unit: ";
const char subMenuItem1[] PROGMEM = "  Min humidity: ";
const char subMenuItem2[] PROGMEM = "  Max humidity: ";
const char subMenuItem3[] PROGMEM = "  Max duration: ";
const char subMenuItem4[] PROGMEM = "  Friquency:    ";
const char subMenuItem5[] PROGMEM = "  Exit";

const char *const subMenu[] PROGMEM =
    {
        subMenuItem0,
        subMenuItem1,
        subMenuItem2,
        subMenuItem3,
        subMenuItem4,
        subMenuItem5,
};

const uint32_t MENU_OFF_TIMEOUT = 15000;

struct UnitParamsContainer
{
  uint8_t minHumidity;
  uint8_t maxHumidity;
  uint8_t runDuration;
  unsigned long runFrequency;
};

class Menu
{
private:
  EncButton<EB_TICK, PD3, PD4, PD2> *_encoder;
  GyverOLED<SSD1306_128x64, OLED_BUFFER> *_display;
  UnitParams *_unitParamList[4];
  uint8_t _unitCount;
  uint8_t _pointer;
  uint8_t _subMenuPointer;
  bool _isSubmenuActive;
  bool _isSubMenuEdit;
  bool _isOn;
  bool _isFirstClickAfterTurnOn;
  unsigned long _lasActivity;

  void printMainMenuItem(uint8_t);

  void printCursorMainMenu(uint8_t);

  void printSubMenuItem(uint8_t);

  void printCursorSubMenu(uint8_t);

  void loadParams(uint8_t, UnitParams *);

  void saveParams(uint8_t, UnitParams *);

  void printTimeFromMillis(unsigned long);

  void printTimeFromSeconds(unsigned long);

  void printBattery(byte);

  byte getBatteryChargeLevel();


  uint8_t getCurrentUnitIdx();

public:
  Menu(EncButton<EB_TICK, PD3, PD4, PD2> *, GyverOLED<SSD1306_128x64, OLED_BUFFER> *);

  void printMenu();

  void printSubMenu();

  void addUnit(UnitParams *);

  bool isActive();

  void turnOn();

  void wakeUp();

  void tick();
};