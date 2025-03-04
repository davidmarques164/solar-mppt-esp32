#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseManager.h"
#include "TimeManager.h"
#include "ADSManager.h"
#include "WiFiHelper.h"

WiFiHelper wifiHelper;

// Configuração do Firebase
#define API_KEY "AIzaSyD0Xugrn8xexb3eMF3AplTWljXdYmRgW0E"
#define DATABASE_URL "https://dados-42624-default-rtdb.firebaseio.com/"

// Objetos FirebaseManager, TimeManager e ADSManager
FirebaseManager firebaseManager(API_KEY, DATABASE_URL);
TimeManager timeManager; // Usando valores padrão: "pool.ntp.org", GMT-3, sem horário de verão
ADSManager adsManager;

unsigned long sendDataPrevMillis = 0;

void setup() {
  Serial.begin(115200);

  wifiHelper.begin(); // Inicializa o WiFi

  // Inicializar o ADS1115
  if (!adsManager.begin()) {
    Serial.println("❌ Falha ao inicializar o ADS1115!");
    while (1);
  }

   // Configurar NTP para obter a hora correta
  timeManager.begin();

  // Inicializar Firebase
  firebaseManager.begin();
}

void loop() {

  wifiHelper.verificaConexao(); // Verifica a conexão sem bloquear o ESP32
  
  if (firebaseManager.isReady() && (millis() - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Obter timestamp formatado
    String timeStamp = timeManager.getFormattedTime();

    // Ler valores do ADS1115
    float tensaoEntrada = adsManager.readVoltage(); // Ler tensão no A0
    float correnteEntrada = adsManager.readCurrent(); // Ler corrente no A1

    // Criar um FirebaseJson manualmente
    FirebaseJson json;
    json.set("data_hora", timeStamp);
    json.set("tensao_entrada", tensaoEntrada);
    json.set("corrente_entrada", correnteEntrada);

    // Enviar o JSON para o Firebase
    String path = "/sensores/esp32_01/";
    firebaseManager.sendJson(path, timeStamp, json);

    Serial.println(tensaoEntrada);
    Serial.println(correnteEntrada);
  }
}