#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseManager.h"
#include "TimeManager.h"
#include "INA219Manager.h"
#include "WiFiHelper.h"
#include "MeteoManager.h"

WiFiHelper wifiHelper;

// Configurações
#define API_KEY "AIzaSyD0Xugrn8xexb3eMF3AplTWljXdYmRgW0E"
#define PROJECT_ID "dados-42624"

// Objetos managers
FirebaseManager firebaseManager(API_KEY, PROJECT_ID);
TimeManager timeManager;
INA219Manager ina219Manager;
MeteoManager meteoManager(-3.876, -38.625, "America/Fortaleza");

// Timers
unsigned long sendDataPrevMillis = 0;
unsigned long lastWiFiCheck = 0;
unsigned long lastMeteoUpdate = 0;

void setup() {
    Serial.begin(115200);

    // Inicializações
    wifiHelper.begin();
    ina219Manager.begin();
    timeManager.begin();
    firebaseManager.begin();

    Serial.println("Sistema iniciado. Aguardando dados...");
}

void loop() {
    unsigned long currentMillis = millis();

    // Verificação periódica do WiFi (5 minutos)
    if (currentMillis - lastWiFiCheck > 300000) {
        wifiHelper.verificaConexao();
        lastWiFiCheck = currentMillis;
    }

    // Atualização dos dados meteorológicos (5 minutos)
    if (currentMillis - lastMeteoUpdate > 300000) {
        if(meteoManager.updateData()) {
            Serial.print("[Meteo] ");
            Serial.print(meteoManager.getTemperature(), 1);
            Serial.print("°C | ");
            Serial.print(meteoManager.getRadiation(), 1);
            Serial.println("W/m²");
        }
        lastMeteoUpdate = currentMillis;
    }

    // Envio de dados principal (2 segundos)
    if (firebaseManager.isReady() && (currentMillis - sendDataPrevMillis > 2000)) {
        sendDataPrevMillis = currentMillis;

        // Coleta de dados
        String timestamp = timeManager.getFormattedTime();
        float tensao = ina219Manager.readVoltage();
        float corrente = ina219Manager.readCurrent();
        float potencia = tensao * corrente;
        float temperatura = meteoManager.getTemperature();
        float radiacao = meteoManager.getRadiation();

        // Envio unificado para o Firebase
        bool envioSucesso = firebaseManager.sendData(
            "esp32_01",         // ID do dispositivo
            timestamp,          // Data/Hora formatada
            potencia,           // Potência calculada (W)
            tensao,             // Tensão medida (V)
            corrente,           // Corrente medida (A)
            temperatura,        // Temperatura (°C)
            radiacao            // Irradiância (W/m²)
        );

        // Log de diagnóstico
        if(envioSucesso) {
            Serial.print("[Firebase] ");
            Serial.print(timestamp);
            Serial.print(" | ");
            Serial.print(potencia, 2);
            Serial.print("W (");
            Serial.print(tensao, 2);
            Serial.print("V, ");
            Serial.print(corrente, 3);
            Serial.print("A)");
            
            if(temperatura != 0 || radiacao != 0) {
                Serial.print(" | Meteo: ");
                Serial.print(temperatura, 1);
                Serial.print("°C, ");
                Serial.print(radiacao, 1);
                Serial.print("W/m²");
            }
            Serial.println();
        }
    }
}