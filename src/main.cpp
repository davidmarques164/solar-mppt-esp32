#include <Arduino.h>
#include <WiFi.h>

#include "WiFiHelper.h"
#include "FirebaseManager.h"
#include "TimeManager.h"
#include "INA219Manager.h"
#include "MeteoManager.h"
#include "MPPTManager.h"
#include "SupabaseManager.h"
#include "OLEDManager.h"

#define API_KEY "AIzaSyD0Xugrn8xexb3eMF3AplTWljXdYmRgW0E"
#define PROJECT_ID "dados-42624"

#define SUPABASE_URL "https://njssjklcybofkiplfxyc.supabase.co/rest/v1/leituras"
#define SUPABASE_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im5qc3Nqa2xjeWJvZmtpcGxmeHljIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDcwMDU4MjcsImV4cCI6MjA2MjU4MTgyN30.fNSdJPbYQdn3dYNkfyt3c2juWd2neV7XLYa7aitP7oU"

#define BUTTON_PIN 32

int pagina = 1;

WiFiHelper wifiHelper;
FirebaseManager firebaseManager(API_KEY, PROJECT_ID);
TimeManager timeManager;
INA219Manager ina219Manager;
MeteoManager meteoManager(-3.876, -38.625, "America/Fortaleza");
MPPTManager mpptManager;
SupabaseManager supabaseManager(SUPABASE_URL, SUPABASE_KEY);
OLEDManager oledManager(BUTTON_PIN);

unsigned long sendDataPrevMillis = 0;
const unsigned long sendDataInterval = 300000;  // 5 minutos

unsigned long mpptPrevMillis = 0;
const unsigned long mpptInterval = 2000;  // 2 segundos

void setup() {
  Serial.begin(115200);

  wifiHelper.begin();
  ina219Manager.begin();
  timeManager.begin();
  firebaseManager.begin();
  mpptManager.begin();
  meteoManager.begin(sendDataInterval);
  supabaseManager.begin();
  oledManager.begin();

  oledManager.showPageByNumber(pagina, ina219Manager, meteoManager);
}

void loop() {
  meteoManager.handle();

  if (digitalRead(BUTTON_PIN) == LOW) {  // Botão pressionado
    delay(100);  // debounce simples
    pagina++;
    if (pagina > 4) pagina = 1;
    Serial.print("Página: ");
    Serial.println(pagina);
    while (!digitalRead(BUTTON_PIN));  // espera soltar botão
    delay(100);

    oledManager.showPageByNumber(pagina, ina219Manager, meteoManager);
  }

  unsigned long currentMillis = millis();

  if (currentMillis - mpptPrevMillis >= mpptInterval) {
    mpptPrevMillis = currentMillis;
    float pwm = mpptManager.update(ina219Manager);
  }

  if (firebaseManager.isReady() && (currentMillis - sendDataPrevMillis >= sendDataInterval)) {
    sendDataPrevMillis = currentMillis;
    float pwm = mpptManager.update(ina219Manager);
    firebaseManager.sendData("esp32_01", ina219Manager, meteoManager, timeManager, pwm);
    supabaseManager.sendData("esp32_01", ina219Manager, meteoManager, timeManager, pwm);
  }
}
