#ifndef MPPT_MANAGER_H
#define MPPT_MANAGER_H

#include <Arduino.h>
#include "INA219Manager.h"

class MPPTManager {
private:
    // Variáveis de estado
    float prevPower = 0.0000;
    float prevVoltage = 0.00;
    float prevCurrent = 0.00;
    float dutyCycle = 0.20;
    float step = 0.01;
    const float maxVoltageOut = 25.5f;  // Limite de tensão de saída
    
    // Variáveis para filtros
    float filteredVoltageIn = 0.00;
    float filteredCurrentIn = 0.00;
    float filteredPowerIn = 0.00;
    
    // Parâmetros dos filtros
    const float voltageFilterAlpha = 0.9f;
    const float currentFilterAlpha = 0.9f;
    
    // Parâmetros do passo adaptativo
    const float minStep = 0.005f;
    const float maxStep = 0.03f;
    const float powerThresholdFar = 5.0f;  // Limiar para passo grande
    const float powerThresholdNear = 1.0f; // Limiar para passo médio

public:
    void begin();
    float update(INA219Manager& ina);
    void setPWM(float pwm);
};

#endif