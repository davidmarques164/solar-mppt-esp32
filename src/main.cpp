#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "WiFiHelper.h"
#include "FirebaseManager.h"
#include "TimeManager.h"
#include "INA219Manager.h"
#include "MeteoManager.h"
#include "MPPTManager.h"
#include "SupabaseManager.h"

#define API_KEY "AIzaSyD0Xugrn8xexb3eMF3AplTWljXdYmRgW0E"
#define PROJECT_ID "dados-42624"

#define SUPABASE_URL "https://njssjklcybofkiplfxyc.supabase.co/rest/v1/leituras"
#define SUPABASE_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im5qc3Nqa2xjeWJvZmtpcGxmeHljIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDcwMDU4MjcsImV4cCI6MjA2MjU4MTgyN30.fNSdJPbYQdn3dYNkfyt3c2juWd2neV7XLYa7aitP7oU"

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Gerenciadores
WiFiHelper wifiHelper;
FirebaseManager firebaseManager(API_KEY, PROJECT_ID);
TimeManager timeManager;
INA219Manager ina219Manager;
MeteoManager meteoManager(-3.876, -38.625, "America/Fortaleza");
MPPTManager mpptManager;
SupabaseManager supabaseManager(SUPABASE_URL, SUPABASE_KEY);

// Controle de envio
unsigned long sendDataPrevMillis = 0;

// Controle do OLED
unsigned long lastOledUpdate = 0;
uint8_t oledPage = 0;

void setup() {
  Serial.begin(115200);

  // Inicialização de módulos
  wifiHelper.begin();
  ina219Manager.begin();
  timeManager.begin();
  firebaseManager.begin();
  mpptManager.begin();
  meteoManager.begin();
  supabaseManager.begin();

  // Inicializa OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED nao encontrado");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema iniciado...");
  display.display();
  delay(1000);
}

void loop() {
  meteoManager.handle();  // Atualiza dados meteo a cada 10s

  // Atualiza OLED a cada 2s
  if (millis() - lastOledUpdate > 2000) {
    lastOledUpdate = millis();
    display.clearDisplay();

    float temp = meteoManager.getTemperature();
    float rad = meteoManager.getRadiation();
    float vin = ina219Manager.readVoltageIn();
    float iin = ina219Manager.readCurrentIn();
    float pin = vin * iin;
    float vout = ina219Manager.readVoltageOut();
    float iout = ina219Manager.readCurrentOut();
    float pout = vout * iout;

    display.setCursor(0, 0);
    switch (oledPage) {
      case 0:
        //display.println("Temp & Irradiancia");
        display.print("Temp: ");
        display.print(temp, 1);
        display.println("C");
        display.print("Rad: ");
        display.print(rad, 1);
        display.println("W/m2");
        break;
      case 1:
        //display.println("Pot In / Out");
        display.print("PIn: ");
        display.print(pin, 1);
        display.println("W");
        display.print("POut: ");
        display.print(pout, 1);
        display.println("W");
        break;
      case 2:
        //display.println("Entrada V / A");
        display.print("VIn: ");
        display.print(vin, 2);
        display.println("V");
        display.print("IIn: ");
        display.print(iin, 2);
        display.println("A");
        break;
      case 3:
        //display.println("Saida V / A");
        display.print("VOut: ");
        display.print(vout, 2);
        display.println("V");
        display.print("IOut: ");
        display.print(iout, 2);
        display.println("A");
        break;
    }

    display.display();
    oledPage = (oledPage + 1) % 4;
  }

  // Envio de dados a cada 5s
  unsigned long currentMillis = millis();
  if (firebaseManager.isReady() && (currentMillis - sendDataPrevMillis > 5000)) {
    sendDataPrevMillis = currentMillis;

    float pwm = mpptManager.update(ina219Manager);

    firebaseManager.sendData("esp32_01", ina219Manager, meteoManager, timeManager, pwm);
    supabaseManager.sendData("esp32_01", ina219Manager, meteoManager, timeManager, pwm);
  }
}
