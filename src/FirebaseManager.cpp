#include "FirebaseManager.h"

FirebaseManager::FirebaseManager(String apiKey, String projectId) {
  this->apiKey = apiKey;
  this->projectId = projectId;
  this->firestoreUrl = "https://firestore.googleapis.com/v1/projects/" + projectId + "/databases/(default)/documents/leituras/";  // Corrigido: caminho correto para documentos
}

void FirebaseManager::begin() {
  Serial.println("🔥 Firebase Manager iniciado!");
}

bool FirebaseManager::isReady() {
  return WiFi.status() == WL_CONNECTED;
}

bool FirebaseManager::sendData(String sensorId, INA219Manager& ina, MeteoManager& meteo, TimeManager& time, float dutycycle) {
  if (!isReady()) return false;

  String timestamp = time.getFormattedTime();
  float temperatura = meteo.getTemperature();
  float irradiancia = meteo.getRadiation();
  float tensao_in = ina.readVoltageIn();
  float corrente_in = ina.readCurrentIn();
  float potencia_in = tensao_in * corrente_in;
  float tensao_out = ina.readVoltageOut();
  float corrente_out = ina.readCurrentOut();
  float potencia_out = tensao_out * corrente_out;

  HTTPClient http;
  String docPath = timestamp;  // Usando timestamp como nome do documento
  String fullUrl = firestoreUrl + docPath + "?key=" + apiKey;  // Corrigido: usando o timestamp diretamente

  http.begin(fullUrl);
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["fields"]["timestamp"]["stringValue"] = timestamp;
  doc["fields"]["sensorId"]["stringValue"] = sensorId;
  doc["fields"]["temperatura"]["doubleValue"] = temperatura;
  doc["fields"]["irradiancia"]["doubleValue"] = irradiancia;
  doc["fields"]["tensao_in"]["doubleValue"] = tensao_in;
  doc["fields"]["corrente_in"]["doubleValue"] = corrente_in;
  doc["fields"]["potencia_in"]["doubleValue"] = potencia_in;
  doc["fields"]["tensao_out"]["doubleValue"] = tensao_out;
  doc["fields"]["corrente_out"]["doubleValue"] = corrente_out;
  doc["fields"]["potencia_out"]["doubleValue"] = potencia_out;
  doc["fields"]["dutycycle"]["doubleValue"] = dutycycle;

  String payload;
  serializeJson(doc, payload);

  int httpCode = http.PATCH(payload);  // Envia para o Firebase
  Serial.print("HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == 200 || httpCode == 201) {
    Serial.println("✅ Dados enviados com sucesso!");
    http.end();
    return true;
  } else {
    Serial.print("❌ Erro no envio: ");
    Serial.println(http.getString());
    http.end();
    return false;
  }
}
