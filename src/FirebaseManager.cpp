#include "FirebaseManager.h"

FirebaseManager::FirebaseManager(const char* apiKey, const char* projectId) {
    this->apiKey = apiKey;
    this->projectId = projectId;
    this->firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/leituras?key=" + String(apiKey);
}

void FirebaseManager::begin() {
    Serial.println("🔥 Firebase Manager iniciado!");
}

bool FirebaseManager::isReady() {
    return WiFi.status() == WL_CONNECTED;
}

bool FirebaseManager::sendData(const String &sensorId, const String &timestamp, float potencia, float tensao, float corrente) {
  if (!isReady()) {
      Serial.println("❌ WiFi desconectado! Dados não enviados.");
      return false;
  }

  HTTPClient http;
  
  // Agora usamos a data como identificador do documento
  String docPath = "leituras/" + timestamp;  
  String firestoreDocUrl = "https://firestore.googleapis.com/v1/projects/" + String(projectId) + "/databases/(default)/documents/" + docPath + "?key=" + String(apiKey);
  
  http.begin(firestoreDocUrl);
  http.addHeader("Content-Type", "application/json");

  // Criando o JSON estruturado corretamente
  JsonDocument doc;
  doc["fields"]["sensorId"]["stringValue"] = sensorId;
  doc["fields"]["potencia"]["doubleValue"] = potencia;
  doc["fields"]["tensao"]["doubleValue"] = tensao;
  doc["fields"]["corrente"]["doubleValue"] = corrente;

  String jsonStr;
  serializeJson(doc, jsonStr);

  // Enviar os dados via PATCH para criar um documento com a data como ID
  int httpResponseCode = http.PATCH(jsonStr);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode == 200 || httpResponseCode == 201) {
      Serial.println("✅ Dados enviados com sucesso para o Firestore!");
      http.end();
      return true;
  } else {
      Serial.println("❌ Erro ao enviar os dados para Firestore.");
      Serial.println(http.getString());
      http.end();
      return false;
  }
}
