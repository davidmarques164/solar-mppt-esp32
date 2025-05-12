#include "INA219Manager.h"

INA219Manager::INA219Manager() : ina219In(0x40), ina219Out(0x44) {
  // Inicializa com os endereços corretos
}

bool INA219Manager::begin() {
  if (!ina219In.begin()) {
    Serial.println("❌ Falha ao inicializar o INA219 de entrada (0x40)!");
    return false;
  }
  if (!ina219Out.begin()) {
    Serial.println("❌ Falha ao inicializar o INA219 de saída (0x41)!");
    return false;
  }
  Serial.println("✅ INA219 de entrada e saída inicializados!");
  return true;
}

float INA219Manager::readVoltageIn() {
  return ina219In.getBusVoltage_V();
}

float INA219Manager::readCurrentIn() {
  return ina219In.getCurrent_mA() / 1000.0;
}

float INA219Manager::readPowerIn() {
  return readVoltageIn() * readCurrentIn();
}

float INA219Manager::readVoltageOut() {
  return ina219Out.getBusVoltage_V();
}

float INA219Manager::readCurrentOut() {
  return ina219Out.getCurrent_mA() / 1000.0;
}

float INA219Manager::readPowerOut() {
  return readVoltageOut() * readCurrentOut();
}
