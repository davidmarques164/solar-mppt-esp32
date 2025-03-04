#include "WiFiHelper.h"

#define LED 2 // LED indicador
#define TEMPO_VERIFICACAO 10000 // Tempo de verificação da conexão WiFi (10s)

WiFiHelper::WiFiHelper() {}

void WiFiHelper::configModeCallback(WiFiManager *myWiFiManager) {
    Serial.println("🔧 Entrando no modo de configuração...");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
}

void WiFiHelper::funConecta() {
    // Tenta conectar à última rede salva, se falhar, inicia o modo AP
    if (!wm.autoConnect("ESP32-Config", "12345678")) {
        Serial.println("⚠️ Falha na conexão WiFi, mantendo AP ativo.");
        return;
    }

    // Conexão bem-sucedida
    Serial.println("✅ Conectado ao WiFi!");
    Serial.print("📡 IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED, HIGH);
}

void WiFiHelper::begin() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);

    // Configuração do AP e tempo limite do portal de configuração
    wm.setAPCallback([this](WiFiManager *wm) { this->configModeCallback(wm); });
    wm.setConfigPortalTimeout(60); // Mantém o portal ativo por 60s
    
    funConecta(); // Tenta conectar ao WiFi
}

void WiFiHelper::verificaConexao() {
    static unsigned long ultimoCheck = 0; // Armazena o tempo da última verificação

    if (millis() - ultimoCheck >= TEMPO_VERIFICACAO) {
        ultimoCheck = millis(); // Atualiza o tempo da última verificação

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("⚠️ Conexão WiFi perdida. Tentando reconectar...");
            digitalWrite(LED, LOW);
            funConecta();
        }
    }
}

bool WiFiHelper::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

