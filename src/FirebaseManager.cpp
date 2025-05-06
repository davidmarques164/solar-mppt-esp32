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

bool FirebaseManager::sendData(const String &sensorId, const String &timestamp, 
                             float potencia, float tensao, float corrente,
                             float temperatura, float radiacao) {
    if (!isReady()) {
        Serial.println("❌ WiFi desconectado! Dados não enviados.");
        return false;
    }

    HTTPClient http;
    String docPath = "leituras/" + timestamp;  
    String fullUrl = firestoreUrl + docPath + "?key=" + String(apiKey);
    
    http.begin(fullUrl);
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["fields"]["sensorId"]["stringValue"] = sensorId;
    doc["fields"]["potencia"]["doubleValue"] = potencia;
    doc["fields"]["tensao"]["doubleValue"] = tensao;
    doc["fields"]["corrente"]["doubleValue"] = corrente;
    
    if (temperatura != 0) doc["fields"]["temperatura"]["doubleValue"] = temperatura;
    if (radiacao != 0) doc["fields"]["radiacao"]["doubleValue"] = radiacao;

    String jsonStr;
    serializeJson(doc, jsonStr);

    int httpCode = http.PATCH(jsonStr);
    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if (httpCode == 200 || httpCode == 201) {
        Serial.println("✅ Dados enviados!");
        http.end();
        return true;
    } else {
        Serial.print("❌ Falha no envio: ");
        Serial.println(http.getString());
        http.end();
        return false;
    }
}