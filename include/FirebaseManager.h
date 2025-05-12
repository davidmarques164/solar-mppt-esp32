#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "INA219Manager.h"
#include "MeteoManager.h"
#include "TimeManager.h"

class FirebaseManager {
  private:
    String apiKey;
    String projectId;
    String firestoreUrl;

  public:
    FirebaseManager(String apiKey, String projectId);
    void begin();
    bool isReady();
    bool sendData(String sensorId, INA219Manager& ina, MeteoManager& meteo, TimeManager& time, float dutycycle);
};

#endif
