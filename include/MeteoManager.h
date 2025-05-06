#ifndef METEOMANAGER_H
#define METEOMANAGER_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

class MeteoManager {
  public:
    MeteoManager(float latitude, float longitude, const char* timezone);
    bool begin();
    bool updateData();
    float getTemperature();
    float getRadiation();
    String getLastError();
    
  private:
    float _latitude;
    float _longitude;
    const char* _timezone;
    float _temperature;
    float _radiation;
    String _lastError;
    String _buildUrl();
};

#endif