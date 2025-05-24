#include "MPPTManager.h"


void MPPTManager::begin() {
  prevPower = 0.0000;
  prevVoltage = 0.0000;
  dutyCycle = 0.2000;

  // Configura o PWM no pino 25 com frequência de 20 kHz e resolução de 8 bits
  ledcSetup(0, 25000, 10);      // Canal 0, frequência 25 kHz, resolução 8 bits
  ledcAttachPin(25, 0);        // Atribuindo o pino 25 ao canal 0
  
  // Inicializa filtros
  filteredVoltageIn = 0.00;
  filteredCurrentIn = 0.00;
  filteredPowerIn = 0.00;
}

float MPPTManager::update(INA219Manager& ina) {
  // Leitura e filtragem dos dados de entrada
  float rawVoltageIn = ina.readVoltageIn();
  float rawCurrentIn = ina.readCurrentIn();
  
  // Aplica filtro passa-baixa
  filteredVoltageIn = voltageFilterAlpha * rawVoltageIn + (1 - voltageFilterAlpha) * filteredVoltageIn;
  filteredCurrentIn = currentFilterAlpha * rawCurrentIn + (1 - currentFilterAlpha) * filteredCurrentIn;
  filteredPowerIn = filteredVoltageIn * filteredCurrentIn;

  // Leitura dos dados de saída (sem filtro para resposta rápida a transientes)
  float voltageOut = ina.readVoltageOut();
  float currentOut = ina.readCurrentOut();
  float powerOut = voltageOut * currentOut;

  // Filtro de segurança para a saída - resposta imediata se ultrapassar o limite
  if (voltageOut > maxVoltageOut) {
    // Reduz o duty cycle proporcionalmente ao excesso de tensão
    float safePWM = dutyCycle * (maxVoltageOut / voltageOut);
    setPWM(safePWM);
    dutyCycle = safePWM;
    
    Serial.print("Tensão de saída alta (");
    Serial.print(voltageOut, 2);
    Serial.print("V), ajustando PWM para ");
    Serial.println(safePWM, 3);
    
    // Atualiza valores anteriores para evitar oscilações
    prevPower = filteredPowerIn;
    prevVoltage = filteredVoltageIn;
    
    return dutyCycle;
  }

  // Algoritmo de Condutância Incremental
  // Calcula a variação de potência e a variação de tensão
  float deltaP = filteredPowerIn - prevPower;
  float deltaV = filteredVoltageIn - prevVoltage;
  float deltaI = filteredCurrentIn - prevCurrent;

  if (deltaV == 0.00) {
      if (deltaI == 0.00) {
          dutyCycle = dutyCycle;
      } else if (deltaI > 0.00) {
          dutyCycle += step;
      } else {
          dutyCycle -= step;
      }
  } else {
      if ((deltaI / deltaV) == (-filteredCurrentIn / filteredVoltageIn)) {
          dutyCycle = dutyCycle;
      } else if ((deltaI / deltaV) > (-filteredCurrentIn / filteredVoltageIn)) {
          dutyCycle += step;
      } else {
          dutyCycle -= step;
      }
  }

  // Limites do duty cycle
  dutyCycle = constrain(dutyCycle, 0.05f, 0.95f);  // Mantém entre 5% e 95%

  // Atualiza valores anteriores
  prevPower = filteredPowerIn;
  prevVoltage = filteredVoltageIn;

  // Envia o PWM para o pino 25
  setPWM(dutyCycle);

  // Impressão no monitor serial (opcional para depuração)
  Serial.print("Vin: ");
  Serial.print(filteredVoltageIn, 2);
  Serial.print("V | Iin: ");
  Serial.print(filteredCurrentIn, 3);
  Serial.print("A | Pin: ");
  Serial.print(filteredPowerIn, 2);
  Serial.print("W | Vout: ");
  Serial.print(voltageOut, 2);
  Serial.print("V | Iout: ");
  Serial.print(currentOut, 3);
  Serial.print("A | Piout: ");
  Serial.print(powerOut, 2);
  Serial.print("W | deltaP: ");
  Serial.print(deltaP, 2);
  Serial.print("W | deltaV: ");
  Serial.print(deltaV, 2);
  Serial.print("W | step: ");
  Serial.print(step, 2);
  Serial.print("V | PWM: ");
  Serial.print(dutyCycle * 100, 1);
  Serial.println("%");    
  
  return dutyCycle;
}

void MPPTManager::setPWM(float pwm) {
  // Envia o PWM no pino 25, com escala de 0 a 255
  ledcWrite(0, pwm * 1023);
}
