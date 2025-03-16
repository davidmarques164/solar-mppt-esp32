#ifndef INA219MANAGER_H
#define INA219MANAGER_H

#include <Wire.h>
#include <Adafruit_INA219.h>

class INA219Manager {
public:
  INA219Manager();
  bool begin();
  float readVoltage();
  float readCurrent();

private:
  Adafruit_INA219 ina219;
};

#endif
