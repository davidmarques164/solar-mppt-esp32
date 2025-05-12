#include "MeteoManager.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

MeteoManager::MeteoManager(float latitude, float longitude, const char* timezone)
    : _latitude(latitude), _longitude(longitude), _timezone(timezone),
      _updateInterval(10000), _lastUpdateTime(0), _temperature(NAN),
      _radiation(NAN), _lastError("") {}

void MeteoManager::begin(unsigned long updateInterval) {
    if(updateInterval >= 1000) {
        _updateInterval = updateInterval;
    }
    Serial.println("[MeteoManager] Iniciado com intervalo de " + String(_updateInterval / 1000) + " segundos");
}

void MeteoManager::handle() {
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    if (now - lastUpdate >= _updateInterval) {
        if(updateData()) {
            Serial.println("[MeteoManager] Dados atualizados com sucesso");
            Serial.println("  Temperatura: " + String(_temperature, 1) + "°C");
            Serial.println("  Radiação: " + String(_radiation, 1) + "W/m²");
        } else {
            Serial.println("[MeteoManager] Falha: " + _lastError);
        }
        lastUpdate = now;
    }
}

bool MeteoManager::updateData() {
    if(WiFi.status() != WL_CONNECTED) {
        _lastError = "WiFi desconectado";
        return false;
    }

    HTTPClient http;
    http.setConnectTimeout(10000);
    http.setTimeout(10000);

    String url = buildUrl();
    if(!http.begin(url)) {
        _lastError = "Falha ao iniciar conexão HTTP";
        return false;
    }

    int httpCode = http.GET();
    if(httpCode != HTTP_CODE_OK) {
        _lastError = "HTTP Error " + String(httpCode) + ": " + http.getString();
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

    if(!doc["current"].isNull()) {
        _temperature = doc["current"]["temperature_2m"].as<float>();
        _radiation = doc["current"]["shortwave_radiation"].as<float>();
        _lastUpdateTime = doc["current"]["time"].as<time_t>();
        return true;
    }

    _lastError = "Dados ausentes na resposta";
    return false;
}

String MeteoManager::buildUrl() {
    String url = "https://api.open-meteo.com/v1/forecast?";
    url += "latitude=" + String(_latitude, 6);
    url += "&longitude=" + String(_longitude, 6);
    url += "&current=temperature_2m,shortwave_radiation";
    url += "&timezone=" + String(_timezone);
    url += "&timeformat=unixtime&windspeed_unit=ms&forecast_days=1";
    return url;
}

float MeteoManager::getTemperature() {
    return isnan(_temperature) ? 0.0f : _temperature;
}

float MeteoManager::getRadiation() {
    return isnan(_radiation) ? 0.0f : _radiation;
}

String MeteoManager::getLastError() {
    return _lastError;
}

time_t MeteoManager::getLastUpdateTime() {
    return _lastUpdateTime;
}
