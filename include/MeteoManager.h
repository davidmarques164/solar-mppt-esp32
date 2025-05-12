#ifndef METEO_MANAGER_H
#define METEO_MANAGER_H

#include <Arduino.h>

class MeteoManager {
  public:
    MeteoManager(float latitude, float longitude, const char* timezone);
    void begin(unsigned long updateInterval = 10000);  // padrão: 10 segundos
    void handle();
    float getTemperature();
    float getRadiation();
    String getLastError();
    time_t getLastUpdateTime();

  private:
    float _latitude;
    float _longitude;
    const char* _timezone;
    unsigned long _updateInterval;
    unsigned long _lastUpdateTime;
    float _temperature;
    float _radiation;
    String _lastError;

    bool updateData();
    String buildUrl();
};

#endif
