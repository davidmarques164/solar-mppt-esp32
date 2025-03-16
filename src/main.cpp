#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseManager.h"
#include "TimeManager.h"
#include "INA219Manager.h"
#include "WiFiHelper.h"

WiFiHelper wifiHelper;

// Configuração do Firebase (Firestore)
#define API_KEY "AIzaSyD0Xugrn8xexb3eMF3AplTWljXdYmRgW0E"
#define PROJECT_ID "dados-42624"

// Objetos FirebaseManager, TimeManager e INA219Manager
FirebaseManager firebaseManager(API_KEY, PROJECT_ID);
TimeManager timeManager;
INA219Manager ina219Manager;

unsigned long sendDataPrevMillis = 0;
unsigned long lastWiFiCheck = 0;

void setup() {
    Serial.begin(115200);

    wifiHelper.begin(); // Inicializa o WiFi
    ina219Manager.begin(); // Inicializa o INA219
    timeManager.begin(); // Configurar NTP para obter a hora correta
    firebaseManager.begin(); // Inicializar Firestore
}

void loop() {
    unsigned long currentMillis = millis();

    // Verifica a conexão WiFi a cada 5 minutos
    if (currentMillis - lastWiFiCheck > 300000 || lastWiFiCheck == 0) {
        wifiHelper.verificaConexao();
        lastWiFiCheck = currentMillis;
    }

    // Envia dados ao Firestore a cada 2 segundos
    if (firebaseManager.isReady() && (currentMillis - sendDataPrevMillis > 2000 || sendDataPrevMillis == 0)) {
        sendDataPrevMillis = currentMillis;

        // Obter timestamp formatado
        String timeStamp = timeManager.getFormattedTime();

        // Ler valores do INA219
        float tensaoEntrada = ina219Manager.readVoltage();
        float correnteEntrada = ina219Manager.readCurrent();
        float potenciaEntrada = tensaoEntrada*correnteEntrada;

        // Formato de envio: Leituras > dataHora > valores de entrada > potência;tensão;corrente
        firebaseManager.sendData("esp32_01", timeStamp, potenciaEntrada, tensaoEntrada, correnteEntrada);

        // Exibir no Serial Monitor
        Serial.print("Leituras > ");
        Serial.print(timeStamp);
        Serial.print(" > ");
        Serial.print(potenciaEntrada);
        Serial.print("W; ");
        Serial.print(tensaoEntrada);
        Serial.print("V; ");
        Serial.print(correnteEntrada);
        Serial.println("A");
    }
}
