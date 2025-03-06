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
unsigned long lastWiFiCheck = 0;

void setup() {
  Serial.begin(115200);

  wifiHelper.begin(); // Inicializa o WiFi

  // Inicializar o ADS1115
  adsManager.begin();

  // Configurar NTP para obter a hora correta
  timeManager.begin();

  // Inicializar Firebase
  firebaseManager.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // Verifica a conexão WiFi a cada 5 minutos (300000 ms)
  if (currentMillis - lastWiFiCheck > 300000 || lastWiFiCheck == 0) {
    wifiHelper.verificaConexao();
    lastWiFiCheck = currentMillis;
  }

  // Envia dados ao Firebase a cada 2 segundos
  if (firebaseManager.isReady() && (currentMillis - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = currentMillis;

    // Obter timestamp formatado
    String timeStamp = timeManager.getFormattedTime();

    // Ler valores do ADS1115
    float tensaoEntrada = adsManager.readVoltage(); // Ler tensão no A0
    float correnteEntrada = adsManager.readCurrent(); // Ler corrente no A1

    // Enviar os dados para o Firebase
    firebaseManager.sendData("/sensores/esp32_01/", timeStamp, tensaoEntrada, correnteEntrada);

    // Exibir os valores lidos no Serial Monitor
    Serial.print("Tensão Entrada: ");
    Serial.print(tensaoEntrada);
    Serial.print(" V | Corrente Entrada: ");
    Serial.print(correnteEntrada);
    Serial.println(" A");
  }
}
