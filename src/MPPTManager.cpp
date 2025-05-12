#include "MPPTManager.h"

void MPPTManager::begin() {
  prevPower = 0;
  prevVoltage = 0;
  dutyCycle = 0.5;
  
  // Configura o PWM no pino 25 com frequência de 20 kHz e resolução de 8 bits
  ledcSetup(0, 20000, 8);      // Canal 0, frequência 20 kHz, resolução 8 bits
  ledcAttachPin(25, 0);        // Atribuindo o pino 25 ao canal 0
}

float MPPTManager::update(INA219Manager& ina) {
  // Leitura dos dados de entrada
  float voltageIn = ina.readVoltageIn();
  float currentIn = ina.readCurrentIn();
  float powerIn = voltageIn * currentIn;

  // Leitura dos dados de saída
  float voltageOut = ina.readVoltageOut();
  float currentOut = ina.readCurrentOut();
  float powerOut = voltageOut * currentOut;

  // Filtro de segurança para a saída
  if (voltageOut > maxVoltageOut) {
   // Se a tensão de saída ultrapassar o limite, ajustamos o PWM para reduzir a tensão
   float safePWM = dutyCycle * (maxVoltageOut / voltageOut);
   setPWM(safePWM);
   Serial.print("Tensão de saída alta, ajustando PWM para ");
   Serial.println(safePWM);
  }

  // Lógica P&O (Perturba e Observa)
  float deltaP = powerIn - prevPower;
  float deltaV = voltageIn - prevVoltage;

  if (deltaP == 0) {
    // Mantém o duty
  } else if (deltaP > 0) {
    dutyCycle += (deltaV > 0) ? step : -step;
  } else { // deltaP < 0
    dutyCycle += (deltaV > 0) ? -step : step;
  }

  if (dutyCycle > 1.0) dutyCycle = 1.0;
  if (dutyCycle < 0.0) dutyCycle = 0.0;

  prevPower = powerIn;
  prevVoltage = voltageIn;

  // Envia o PWM para o pino 25
  setPWM(dutyCycle);

  // Impressão no monitor serial
  Serial.print("Tensao In: ");
  Serial.print(voltageIn, 2);
  Serial.print(" V | Corrente In: ");
  Serial.print(currentIn, 3);
  Serial.print(" A | Potencia In: ");
  Serial.print(powerIn, 2);
  Serial.print(" W\n");

  Serial.print("Tensao Out: ");
  Serial.print(voltageOut, 2);
  Serial.print(" V | Corrente Out: ");
  Serial.print(currentOut, 3);
  Serial.print(" A | Potencia Out: ");
  Serial.print(powerOut, 2);
  Serial.print(" W\n");

  Serial.print("PWM Duty Cycle: ");
  Serial.println(dutyCycle, 3);

  return dutyCycle;
}

void MPPTManager::setPWM(float pwm) {
  // Envia o PWM no pino 25, com escala de 0 a 255
  ledcWrite(0, pwm * 255);
}
