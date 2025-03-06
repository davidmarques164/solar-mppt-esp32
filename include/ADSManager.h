#ifndef ADSMANAGER_H
#define ADSMANAGER_H

#include <Adafruit_ADS1X15.h>

class ADSManager {
public:
  ADSManager();
  bool begin();
  float readVoltage();
  float readCurrent();
  float readA2();
  float readA3();


private:
  Adafruit_ADS1115 ads;
};

#endif