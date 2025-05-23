#include "OLEDManager.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

OLEDManager::OLEDManager(uint8_t buttonPin)
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), btnPin(buttonPin) {}

void OLEDManager::begin() {
  pinMode(btnPin, INPUT_PULLUP);
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

void OLEDManager::showPageByNumber(int pagina, INA219Manager& ina, MeteoManager& meteo) {
  float temperature = meteo.getTemperature();
  float radiation = meteo.getRadiation();

  float vin = ina.readVoltageIn();
  float iin = ina.readCurrentIn();
  float pin = vin * iin;

  float vout = ina.readVoltageOut();
  float iout = ina.readCurrentOut();
  float pout = vout * iout;

  display.clearDisplay();
  display.setCursor(0, 0);

  switch (pagina) {
    case 1:
      display.print("Temp: ");
      display.print(temperature, 1);
      display.println("C");
      display.print("Rad: ");
      display.print(radiation, 1);
      display.println("W/m2");
      break;
    case 2:
      display.print("PIn: ");
      display.print(pin, 1);
      display.println("W");
      display.print("POut: ");
      display.print(pout, 1);
      display.println("W");
      break;
    case 3:
      display.print("VIn: ");
      display.print(vin, 2);
      display.println("V");
      display.print("IIn: ");
      display.print(iin, 2);
      display.println("A");
      break;
    case 4:
      display.print("VOut: ");
      display.print(vout, 2);
      display.println("V");
      display.print("IOut: ");
      display.print(iout, 2);
      display.println("A");
      break;
    default:
      display.print("Pagina invalida");
  }
  display.display();
}
