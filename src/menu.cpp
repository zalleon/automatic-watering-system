#include <EEPROM.h>
#include "menu.h"

Menu::Menu(EncButton<EB_TICK, PD3, PD4, PD2> *encoder, GyverOLED<SSD1306_128x64, OLED_BUFFER> *display)
{
    _encoder = encoder;
    _display = display;

    _unitCount = 0;

    _isSubmenuActive = false;
    _isSubMenuEdit = false;

    _pointer = MAIN_MENU_LINE_OFFSET;
    _subMenuPointer = SUB_MENU_LINE_OFFSET;

    _isOn = false;
    _lasActivity = 0;
}

void Menu::printMenu()
{
    _display->clear();
    _display->home();

    _display->println("Main menu");

    for (uint8_t i = 0; i < _unitCount; i++)
    {
        printMainMenuItem(i);
    }

    // Item params
    for (uint8_t i = 0; i < _unitCount; i++)
    {
        _display->setCursor(45, i + 1);
        _display->print(_unitParamList[i]->getCurrentHumidity());
        _display->print("% ");
        printTimeFromMillis(_unitParamList[i]->getLastRunTime());
    }

    printCursorMainMenu(_pointer);

    _display->setCursorXY(110, 1);
    printBattery(getBatteryChargeLevel());

    _display->update();
}

void Menu::printSubMenu()
{
    _display->clear();
    _display->home();

    for (uint8_t i = 0; i < SUB_MENU_ITEMS; i++)
    {
        printSubMenuItem(i);
    }

    _display->setCursor(32, 0);
    _display->print(_pointer);

    _display->setCursor(95, 1);
    _display->print(_unitParamList[getCurrentUnitIdx()]->getMinHumidity());
    _display->print("%");

    _display->setCursor(95, 2);
    _display->print(_unitParamList[getCurrentUnitIdx()]->getMaxHumidity());
    _display->print("%");

    _display->setCursor(95, 3);
    _display->print(_unitParamList[getCurrentUnitIdx()]->getRunDuration());
    _display->print("s");

    _display->setCursor(75, 4);
    printTimeFromSeconds(_unitParamList[getCurrentUnitIdx()]->getRunFrequency());

    printCursorSubMenu(_subMenuPointer);

    _display->update();
}

void Menu::addUnit(UnitParams *param)
{
    if (_unitCount < MAIN_MENU_MAX_ITEMS)
    {
        loadParams(_unitCount, param);

        _unitParamList[_unitCount] = param;

        _unitCount++;
    }
}

bool Menu::isActive()
{
    return _isOn;
}

void Menu::turnOn()
{
    Serial.println("Turn on menu");

    _isOn = true;
    _lasActivity = millis();

    _display->setPower(true);
    _display->init();
    _display->setContrast(255);
}

void Menu::wakeUp()
{
    Serial.println("Menu waked up");

    _isOn = true;
    _isFirstClickAfterTurnOn = true;
    _lasActivity = millis();

    _display->setPower(true);
}

void Menu::tick()
{
    _encoder->tick();

    if (_isOn && (millis() - _lasActivity > MENU_OFF_TIMEOUT))
    {
        _isOn = false;

        _display->setPower(false);

        Serial.println("Turn off menu");
    }

    if (_encoder->isClick())
    {
        // Use first click just for activation menu
        if (_isFirstClickAfterTurnOn)
        {
            _isFirstClickAfterTurnOn = false;
            return;
        }

        _lasActivity = millis();

        if (_isSubmenuActive)
        {
            if (_subMenuPointer == SUB_MENU_EXIT_ITEM_INDEX)
            {
                saveParams(getCurrentUnitIdx(), _unitParamList[getCurrentUnitIdx()]);

                _isSubmenuActive = false;
                printMenu();
            }
            else if (_isSubMenuEdit && _subMenuPointer > 0 && _subMenuPointer < SUB_MENU_EXIT_ITEM_INDEX)
            {
                _isSubMenuEdit = false;
                printSubMenu();
            }
            else if (!_isSubMenuEdit && _subMenuPointer > 0 && _subMenuPointer < SUB_MENU_EXIT_ITEM_INDEX)
            {
                _isSubMenuEdit = true;
                printSubMenu();
            }
        }
        else
        {
            _isSubmenuActive = true;
            _subMenuPointer = SUB_MENU_LINE_OFFSET;

            printSubMenu();
        }
    }

    if (_encoder->isTurn())
    {
        _lasActivity = millis();

        if (_isSubmenuActive)
        {
            if (_isSubMenuEdit)
            {
                if (_encoder->isRight())
                {
                    switch (_subMenuPointer)
                    {
                    case 1:
                        _unitParamList[getCurrentUnitIdx()]->incMinHumidity();
                        break;
                    case 2:
                        _unitParamList[getCurrentUnitIdx()]->incMaxHumidity();
                        break;
                    case 3:
                        _unitParamList[getCurrentUnitIdx()]->incRunDuration();
                        break;
                    case 4:
                        _unitParamList[getCurrentUnitIdx()]->incRunFrequency();
                        break;
                    }
                }

                if (_encoder->isLeft())
                {
                    switch (_subMenuPointer)
                    {
                    case 1:
                        _unitParamList[getCurrentUnitIdx()]->decMinHumidity();
                        break;
                    case 2:
                        _unitParamList[getCurrentUnitIdx()]->decMaxHumidity();
                        break;
                    case 3:
                        _unitParamList[getCurrentUnitIdx()]->decRunDuration();
                        break;
                    case 4:
                        _unitParamList[getCurrentUnitIdx()]->decRunFrequency();
                        break;
                    }
                }
            }
            else
            {
                if (_encoder->isRight())
                {
                    _subMenuPointer++;
                    if (_subMenuPointer > SUB_MENU_EXIT_ITEM_INDEX)
                        _subMenuPointer = SUB_MENU_LINE_OFFSET;
                }
                if (_encoder->isLeft())
                {
                    _subMenuPointer--;
                    if (_subMenuPointer < SUB_MENU_LINE_OFFSET)
                        _subMenuPointer = SUB_MENU_EXIT_ITEM_INDEX;
                }
            }
            printSubMenu();
        }
        else
        {
            if (_encoder->isRight())
            {
                _pointer++;
                if (_pointer > _unitCount)
                    _pointer = MAIN_MENU_LINE_OFFSET;
            }
            if (_encoder->isLeft())
            {
                _pointer--;
                if (_pointer < MAIN_MENU_LINE_OFFSET)
                    _pointer = _unitCount;
            }
            printMenu();
        }
    }
}

void Menu::printMainMenuItem(uint8_t num)
{
    char buffer[21];                                // Буфер на полную строку
    uint16_t ptr = pgm_read_word(&(mainMenu[num])); // Получаем указатель на первый символ строки
    uint8_t i = 0;                                  // Переменная - счетчик

    do
    {                                             // Начало цикла
        buffer[i] = (char)(pgm_read_byte(ptr++)); // Прочитать в буфер один символ из PGM и подвинуть указатель на 1
    } while (buffer[i++] != NULL);                // Если это не конец строки - вернуться в начало цикла

    _display->println(buffer); // Вывод готовой строки с переносом на следующую
}

void Menu::printCursorMainMenu(uint8_t pos)
{
    _display->setCursor(0, pos);
    _display->print(">");
}

void Menu::printSubMenuItem(uint8_t num)
{
    char buffer[21];                               // Буфер на полную строку
    uint16_t ptr = pgm_read_word(&(subMenu[num])); // Получаем указатель на первый символ строки
    uint8_t i = 0;                                 // Переменная - счетчик

    do
    {                                             // Начало цикла
        buffer[i] = (char)(pgm_read_byte(ptr++)); // Прочитать в буфер один символ из PGM и подвинуть указатель на 1
    } while (buffer[i++] != NULL);                // Если это не конец строки - вернуться в начало цикла

    _display->println(buffer); // Вывод готовой строки с переносом на следующую
}

void Menu::printCursorSubMenu(uint8_t pos)
{
    if (_isSubMenuEdit)
    {
        _display->setCursor(120, pos);
        _display->print("<");
    }
    else
    {
        _display->setCursor(0, pos);
        _display->print(">");
    }
}

void Menu::loadParams(uint8_t idx, UnitParams *params)
{
    if (EEPROM.read(INIT_ADDR - idx) != INIT_KEY)
    {
        return; // Use default params
    }

    UnitParamsContainer container;

    unsigned long containerSize = sizeof(container);

    eeprom_read_block((void *)&container, (void *)(containerSize * idx), containerSize);

    params->setMinHumidity(container.minHumidity);
    params->setMaxHumidity(container.maxHumidity);
    params->setRunDuration(container.runDuration);
    params->setRunFrequency(container.runFrequency);
}

void Menu::saveParams(uint8_t idx, UnitParams *params)
{
    UnitParamsContainer container;
    unsigned long containerSize = sizeof(container);

    eeprom_read_block((void *)&container, (void *)(containerSize * idx), containerSize);

    bool isChanged = false;

    if (container.minHumidity != params->getMinHumidity())
    {
        isChanged = true;
        container.minHumidity = params->getMinHumidity();
    }
    if (container.maxHumidity != params->getMaxHumidity())
    {
        isChanged = true;
        container.maxHumidity = params->getMaxHumidity();
    }
    if (container.runDuration != params->getRunDuration())
    {
        isChanged = true;
        container.runDuration = params->getRunDuration();
    }
    if (container.runFrequency != params->getRunFrequency())
    {
        isChanged = true;
        container.runFrequency = params->getRunFrequency();
    }

    if (isChanged)
    {
        eeprom_write_block((void *)&container, (void *)(containerSize * idx), containerSize);

        // Mark as initiated
        if (EEPROM.read(INIT_ADDR - idx) != INIT_KEY)
        {
            EEPROM.write(INIT_ADDR - idx, INIT_KEY);
        }
    }
}

byte Menu::getBatteryChargeLevel()
{
    int raw = analogRead(A0);
    long vin = raw * 5.35 / 1024;

    return map(vin, 4.8, 5.35, 0, 100);
}

void Menu::printBattery(byte percent)
{
    _display->drawByte(0b00111100);
    _display->drawByte(0b00111100);
    _display->drawByte(0b11111111);
    for (byte i = 0; i < 100 / 8; i++)
    {
        if (i < (100 - percent) / 8)
            _display->drawByte(0b10000001);
        else
            _display->drawByte(0b11111111);
    }
    _display->drawByte(0b11111111);
}

void Menu::printTimeFromMillis(unsigned long v)
{
    unsigned long Now = v / 1000;

    _display->print((Now / 3600) % 24);
    _display->print(":");
    _display->print((Now / 60) % 60);
    _display->print(":");
    _display->print(Now % 60);
    
}

void Menu::printTimeFromSeconds(unsigned long v)
{
    _display->print((v / 3600) % 24);
    _display->print(":");
    _display->print((v / 60) % 60);
    _display->print(":");
    _display->print(v % 60);
}

uint8_t Menu::getCurrentUnitIdx()
{
    return _pointer - MAIN_MENU_LINE_OFFSET;
}