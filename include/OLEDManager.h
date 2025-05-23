#ifndef OLEDMANAGER_H
#define OLEDMANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "INA219Manager.h"
#include "MeteoManager.h"

class OLEDManager {
public:
  explicit OLEDManager(uint8_t buttonPin);
  void begin();
  void showPageByNumber(int pagina, INA219Manager& ina, MeteoManager& meteo);

private:
  Adafruit_SSD1306 display;
  uint8_t btnPin;
};

#endif
