#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <WiFi.h>
#include <WiFiManager.h>

class WiFiHelper {
public:
    WiFiHelper();
    void begin(); // Inicializa o WiFi e o AP caso necessário
    void verificaConexao(); // Verifica e reconecta caso a conexão caia
    bool isConnected(); // Retorna o estado da conexão
private:
    WiFiManager wm;
    void configModeCallback(WiFiManager *myWiFiManager);
    void funConecta();
};

#endif
