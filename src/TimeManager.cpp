#include "TimeManager.h"

TimeManager::TimeManager(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec) {
  _ntpServer = ntpServer;
  _gmtOffset_sec = gmtOffset_sec;
  _daylightOffset_sec = daylightOffset_sec;
}

void TimeManager::begin() {
  configTime(_gmtOffset_sec, _daylightOffset_sec, _ntpServer);
  Serial.println("⏳ Configurando horário...");
  delay(2000);  // Esperar para sincronizar
}

String TimeManager::getFormattedTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("⏰ Erro ao obter horário");
    return "0000-00-00_00-00-00";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &timeinfo);
  return String(buffer);
}