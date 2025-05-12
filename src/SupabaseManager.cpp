#include "SupabaseManager.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

SupabaseManager::SupabaseManager(const String& url, const String& apiKey)
  : _url(url), _apiKey(apiKey) {}

void SupabaseManager::begin() {
  Serial.println("🟢 SupabaseManager iniciado.");
}

bool SupabaseManager::sendData(const String& deviceId, INA219Manager& ina, MeteoManager& meteo, TimeManager& time, float dutyCycle) {
  if (WiFi.status() != WL_CONNECTED) return false;

  String timestamp = time.getFormattedTime();
  float temperatura = meteo.getTemperature();
  float irradiancia = meteo.getRadiation();
  float tensao_in = ina.readVoltageIn();
  float corrente_in = ina.readCurrentIn();
  float potencia_in = tensao_in * corrente_in;
  float tensao_out = ina.readVoltageOut();
  float corrente_out = ina.readCurrentOut();
  float potencia_out = tensao_out * corrente_out;

  JsonDocument doc;
  doc["timestamp"] = timestamp;
  doc["temperatura"] = temperatura;
  doc["irradiancia"] = irradiancia;
  doc["tensao_in"] = tensao_in;
  doc["corrente_in"] = corrente_in;
  doc["potencia_in"] = potencia_in;
  doc["tensao_out"] = tensao_out;
  doc["corrente_out"] = corrente_out;
  doc["potencia_out"] = potencia_out;
  doc["dutycycle"] = dutyCycle;
  doc["dispositivo"] = deviceId;

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(_url);
  http.addHeader("apikey", _apiKey);
  http.addHeader("Authorization", "Bearer " + _apiKey);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Prefer", "return=minimal");

  int httpCode = http.POST(payload);
  Serial.print("📤 Supabase HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode < 200 || httpCode >= 300) {
    Serial.println("❌ Supabase error: " + http.getString());
    http.end();
    return false;
  }

  http.end();
  return true;
}
