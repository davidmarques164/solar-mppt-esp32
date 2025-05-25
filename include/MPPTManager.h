#ifndef MPPT_MANAGER_H
#define MPPT_MANAGER_H

#include <Arduino.h>
#include "INA219Manager.h"

class MPPTManager {
public:
    void begin();
    float update(INA219Manager& ina);

private:
    float prevPower = 0.0f;
    float prevVoltage = 0.0f;
    float prevCurrent = 0.0f;
    float dutyCycle = 0.2f;
    float filteredVoltageIn = 0.0f;
    float filteredCurrentIn = 0.0f;
    float filteredPowerIn = 0.0f;
    float vMPPT = 0.0f;

    const float voltageFilterAlpha = 0.1f;
    const float currentFilterAlpha = 0.1f;
    const float step = 0.2f;
    const float maxVoltageOut = 25.5f;

    void setPWM(float pwm);
};

#endif
