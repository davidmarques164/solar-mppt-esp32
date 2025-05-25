#include "MPPTManager.h"

void MPPTManager::begin() {
  prevPower = 0.0000;
  prevVoltage = 0.0000;
  dutyCycle = 0.2000;

  // Configura o PWM no pino 25 com frequência de 25 kHz e resolução de 10 bits
  ledcSetup(0, 25000, 10);      
  ledcAttachPin(25, 0);        
  
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
    float safePWM = dutyCycle * (maxVoltageOut / voltageOut);
    setPWM(safePWM);
    dutyCycle = safePWM;

    Serial.print("Tensão de saída alta (");
    Serial.print(voltageOut, 2);
    Serial.print("V), ajustando PWM para ");
    Serial.println(safePWM, 3);

    prevPower = filteredPowerIn;
    prevVoltage = filteredVoltageIn;
    return dutyCycle;
  }

  // --- Método Perturb and Observe (P&O) --- 
  if (filteredVoltageIn < 25.5 && filteredVoltageIn > 0.0) {
    float vInPerturb = filteredVoltageIn + step;
    float vOutPerturb = filteredVoltageIn + step * (vInPerturb - filteredVoltageIn) / (vInPerturb + filteredVoltageIn);
    float pOutPerturb = vOutPerturb * filteredCurrentIn;
    float pOut = filteredVoltageIn * filteredCurrentIn;

    if (pOutPerturb > pOut) {
      vMPPT = vInPerturb;
    } else {
      vInPerturb = filteredVoltageIn - step;
      vOutPerturb = filteredVoltageIn + step * (vInPerturb - filteredVoltageIn) / (vInPerturb + filteredVoltageIn);
      pOutPerturb = vOutPerturb * filteredCurrentIn;

      if (pOutPerturb > pOut) {
        vMPPT = vInPerturb;
      } else {
        vMPPT = filteredVoltageIn;
      }
    }
  } else {
    vMPPT = filteredVoltageIn;
  }

  // Cálculo do duty cycle baseado na tensão MPPT
  dutyCycle = (vMPPT - 0.0) / (25.5 - 0.0);
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
  Serial.print("W | step: ");
  Serial.print(step, 2);
  Serial.print("V | PWM: ");
  Serial.print(dutyCycle * 100, 1);
  Serial.println("%");    

  return dutyCycle;
}

void MPPTManager::setPWM(float pwm) {
  // Envia o PWM no pino 25, com escala de 0 a 1023 (10 bits)
  ledcWrite(0, pwm * 1023);
}
