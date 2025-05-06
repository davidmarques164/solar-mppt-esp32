#include "MeteoManager.h"

MeteoManager::MeteoManager(float latitude, float longitude, const char* timezone)
  : _latitude(latitude), _longitude(longitude), _timezone(timezone), 
    _temperature(0), _radiation(0) {}

bool MeteoManager::begin() {
  return WiFi.status() == WL_CONNECTED;
}

String MeteoManager::_buildUrl() {
  String url = "https://api.open-meteo.com/v1/forecast?";
  url += "latitude=" + String(_latitude, 6);
  url += "&longitude=" + String(_longitude, 6);
  url += "&current=temperature_2m,shortwave_radiation";
  url += "&timezone=" + String(_timezone);
  return url;
}

bool MeteoManager::updateData() {
  if(WiFi.status() != WL_CONNECTED) {
    _lastError = "WiFi not connected";
    return false;
  }

  HTTPClient http;
  String url = _buildUrl();
  
  http.begin(url);
  int httpCode = http.GET();
  
  if(httpCode != HTTP_CODE_OK) {
    _lastError = "HTTP Error: " + String(httpCode);
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, payload);
  
  if(error) {
    _lastError = "JSON Error: " + String(error.c_str());
    return false;
  }

  _temperature = doc["current"]["temperature_2m"];
  _radiation = doc["current"]["shortwave_radiation"];
  return true;
}

float MeteoManager::getTemperature() { return _temperature; }
float MeteoManager::getRadiation() { return _radiation; }
String MeteoManager::getLastError() { return _lastError; }