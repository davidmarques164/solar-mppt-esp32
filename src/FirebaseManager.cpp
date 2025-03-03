#include "FirebaseManager.h"

FirebaseManager::FirebaseManager(const char* apiKey, const char* databaseUrl) {
  config.api_key = apiKey;
  config.database_url = databaseUrl;
  signupOK = false;
}

void FirebaseManager::begin() {
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("🔥 Autenticação Anônima bem-sucedida!");
    signupOK = true;
  } else {
    Serial.printf("❌ Erro na autenticação: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

bool FirebaseManager::isReady() {
  return Firebase.ready() && signupOK;
}

bool FirebaseManager::sendData(const String& path, const String& timestamp, float tensaoEntrada, float correnteEntrada) {
  FirebaseJson json;
  json.set("data_hora", timestamp);
  json.set("tensao_entrada", tensaoEntrada);
  json.set("corrente_entrada", correnteEntrada);


  return sendJson(path, timestamp, json); // Reutiliza o novo método
}

// Novo método: Envia um FirebaseJson já montado
bool FirebaseManager::sendJson(const String& path, const String& timestamp, FirebaseJson& json) {
    String fullPath = path;
    fullPath.concat(timestamp);

  if (Firebase.RTDB.setJSON(&fbdo, fullPath.c_str(), &json)) {
    Serial.println("✅ Dados enviados com sucesso!");
    return true;
  } else {
    Serial.print("❌ Erro ao enviar: ");
    Serial.println(fbdo.errorReason());
    return false;
  }
}