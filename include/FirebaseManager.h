#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

class FirebaseManager {
public:
  FirebaseManager(const char* apiKey, const char* databaseUrl);
  void begin();
  bool isReady();
  bool sendData(const String &path, const String &timestamp, float tensaoEntrada, float correnteEntrada);
  //bool sendData(const String &path, const String &timestamp, float tensaoEntrada, float correnteEntrada, float potenciaEntrada, float dutyCycle, float tensaoSaida, float correnteSaida, float potenciaMaxima);
  bool sendJson(const String& path, const String& timestamp, FirebaseJson& json); // Novo método

private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;
  bool signupOK;
};

#endif