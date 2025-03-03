#include "ADSManager.h"

ADSManager::ADSManager() {
  // Construtor vazio
}

bool ADSManager::begin() {
  if (!ads.begin()) {
    Serial.println("❌ Falha ao inicializar o ADS1115!");
    return false;
  }
  Serial.println("✅ ADS1115 inicializado!");
  return true;
}

float ADSManager::readVoltage() {
  int16_t adc0 = ads.readADC_SingleEnded(0); // Ler tensão no A0
  return (adc0 * 0.0017633675) ; // Converter para volts
}

float ADSManager::readCurrent() {
  int16_t adc1 = ads.readADC_SingleEnded(1); // Ler corrente no A1
  return (adc1 * 0.0005142857) ; // Converter para amperes
}