#ifndef MPPT_MANAGER_H
#define MPPT_MANAGER_H

#include <Arduino.h>
#include "INA219Manager.h"

class MPPTManager {
  private:
    float prevPower = 0;
    float prevVoltage = 0;
    float dutyCycle = 0.5;
    float step = 0.01;
    float maxVoltageOut = 14.0;  // Limite de tensão de saída (para a bateria)
    float maxCurrentOut = 10.0;  // Limite de corrente de saída (exemplo)

  public:
    void begin();
    float update(INA219Manager& ina219Manager);
    void setPWM(float pwm);
};

#endif
