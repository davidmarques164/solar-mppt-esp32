#include "INA219Manager.h"

INA219Manager::INA219Manager() {
  // Construtor vazio
}

bool INA219Manager::begin() {
  if (!ina219.begin()) {
    Serial.println("❌ Falha ao inicializar o INA219!");
    return false;
  }
  Serial.println("✅ INA219 inicializado!");
  return true;
}

float INA219Manager::readVoltage() {
  return ina219.getBusVoltage_V(); // Retorna a tensão medida pelo INA219
}

float INA219Manager::readCurrent() {
  return ina219.getCurrent_mA() / 1000.0; // Converte de mA para A
}
