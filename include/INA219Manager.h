#ifndef INA219_MANAGER_H
#define INA219_MANAGER_H

#include <Wire.h>
#include <Adafruit_INA219.h>

class INA219Manager {
  private:
    Adafruit_INA219 ina219In;          // endereço padrão 0x40
    Adafruit_INA219 ina219Out{0x44};   // endereço alternativo 0x41

  public:
    INA219Manager();
    bool begin();
    float readVoltageIn();
    float readCurrentIn();
    float readPowerIn();
    float readVoltageOut();
    float readCurrentOut();
    float readPowerOut();
};

#endif
