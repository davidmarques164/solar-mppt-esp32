#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class FirebaseManager {
public:
    FirebaseManager(const char* apiKey, const char* projectId);
    void begin();
    bool isReady();
    bool sendData(const String &sensorId, const String &timestamp, float potencia, float tensao, float corrente);

private:
    String apiKey;
    String projectId;
    String firestoreUrl;
};

#endif
