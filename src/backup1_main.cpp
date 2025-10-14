// WT32-ETH01 (ESP32) – Front-Pin Layout (Version 1.4)
// ===================================================

// Left Pin Column                             Right Pin Column
// -------------------------------------------------------------
// EN / RST / BOOT                                3.3V  → Versorgung

// GPIO00   BOOT / Flash strapping             GPIO01   TX0  / UART0 TX
// GPIO02   strapping (low @ boot)             GPIO03   RX0  / UART0 RX
// GPIO04   GPIO / frei                        GPIO05   GPIO / frei
// GPIO12   1-Wire  (open-drain + 4.7k)        GPIO13   GPIO / frei
// GPIO14   PWM / LEDC output                  GPIO15   strapping (must be low)
// GPIO16   UART2 RX                           GPIO17   UART2 TX / **On-board LED**
// GPIO18   ETH_MDIO (LAN8720)                 GPIO19   ETH_TXD0 (LAN8720)
// GPIO20   GPIO / frei                        GPIO21   ETH_CLK_OUT (LAN8720)
// GPIO22   ETH_RXD0 (LAN8720)                 GPIO23   ETH_MDC (LAN8720)
// GPIO24   GPIO / frei                        GPIO25   ETH_TX_EN (LAN8720)
// GPIO26   ETH_RX_ER (LAN8720)                GPIO27   ETH_CRS_DV (LAN8720)
// GPIO32   I2C SCL                            GPIO33   I2C SDA
// GPIO34   Input only / ADC                   GPIO35   Input only / ADC
// GPIO36   Input only / ADC (VP)              GPIO37   GPIO / frei
// GPIO38   GPIO / frei                        GPIO39   Input only / ADC (VN)

// GND        Masse / Ground                   GND       Masse / Ground
// -------------------------------------------------------------

// Hinweise:
// - On-Board LED → **GPIO17** (aktiv HIGH)
// - LAN/PHY reserviert: 18,19,21,22,23,25,26,27 → nicht verwenden
// - Empfohlene User-Pins:
//     • I²C  → GPIO32 (SCL), GPIO33 (SDA)
//     • UART → GPIO16 (RX2), GPIO17 (TX2 / LED)
//     • PWM  → GPIO14
//     • 1-Wire → GPIO12 (mit 4.7k Pull-up auf 3.3V)
// - Input-only: GPIO34–39 (keine Ausgänge)
// - Bootrelevante Pins: GPIO0, GPIO2, GPIO12, GPIO15





#include <Wire.h>
#include <WebServer.h>
#include <ETH.h>
#include "PCF8574.h"
#include "Adafruit_MPR121.h"

// ---------- I2C Setup ----------
#define SDA_PIN 33
#define SCL_PIN 32

// ---------- PCF8574 Adressen ----------
PCF8574 pcfIn1(0x20);    // A2 = GND, A1 = GND, A0 = GND
PCF8574 pcfIn2(0x21);    // A2 = GND, A1 = GND, A0 = VCC
PCF8574 pcfRel1(0x22);    // A2 = GND, A1 = VCC, A0 = GND
PCF8574 pcfRel2(0x23);    // A2 = GND, A1 = VCC, A0 = VCC
PCF8574 pcfRel3(0x24);    // A2 = VCC, A1 = GND, A0 = GND

// ---------- MPR121 Sensoren ----------
Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();
Adafruit_MPR121 cap3 = Adafruit_MPR121();

#define MPR_IRQ_PCF_PIN 0   // z.B. P0 vom pcfIn1 als IRQ Eingang

// ---------- Webserver ----------
WebServer server(80);

// ---------- Zustandsspeicher ----------
uint8_t relayState[24];   // 3 PCFs à 8 Ausgänge
uint8_t inputState[16];   // 2 PCFs à 8 Eingänge

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(500);

  // Ethernet starten
  ETH.begin();
  Serial.println("Ethernet initialisiert");

  // PCFs starten
  pcfIn1.begin();
  pcfIn2.begin();
  pcfRel1.begin();
  pcfRel2.begin();
  pcfRel3.begin();

  // Eingänge konfigurieren
  for (int i = 0; i < 8; i++) {
    pcfIn1.pinMode(i, INPUT);
    pcfIn2.pinMode(i, INPUT);
  }

  // Ausgänge konfigurieren
  for (int i = 0; i < 8; i++) {
    pcfRel1.pinMode(i, OUTPUT);
    pcfRel2.pinMode(i, OUTPUT);
    pcfRel3.pinMode(i, OUTPUT);
  }

  // MPR121 Sensoren starten
  cap1.begin(0x5A);
  cap2.begin(0x5C);
  cap3.begin(0x5D);

  // Webserver konfigurieren
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
  Serial.println("Webserver gestartet");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  server.handleClient();

  // Eingänge lesen
  for (int i = 0; i < 8; i++) {
    inputState[i] = pcfIn1.digitalRead(i);
    inputState[i + 8] = pcfIn2.digitalRead(i);
  }

  // Beispiel: IRQ über PCF erkennen
  bool mprIRQ = (pcfIn1.digitalRead(MPR_IRQ_PCF_PIN) == LOW);
  if (mprIRQ) {
    // Abfragen, welcher MPR121 ein Event meldet
    checkMPR(cap1, "MPR121 #1");
    checkMPR(cap2, "MPR121 #2");
    checkMPR(cap3, "MPR121 #3");
  }

  delay(50);
}

// ======================================================
// Hilfsfunktion: MPR121 Status prüfen
// ======================================================
void checkMPR(Adafruit_MPR121 &sensor, const char *name) {
  uint16_t touched = sensor.touched();
  if (touched) {
    Serial.printf("%s touch mask: 0x%04X\n", name, touched);
  }
}

// ======================================================
// Webserver Handler
// ======================================================
void handleRoot() {
  String html = "<html><body><h2>WT32-ETH01 Steuerung</h2>";

  html += "<h3>Relais</h3>";
  for (int i = 0; i < 24; i++) {
    html += "<button onclick=\"location.href='/toggle?r=" + String(i) + "'\">";
    html += (relayState[i]) ? "Relais " + String(i) + " EIN" : "Relais " + String(i) + " AUS";
    html += "</button><br>";
  }

  html += "<h3>Eingaenge</h3><table border='1'><tr>";
  for (int i = 0; i < 16; i++) {
    html += "<td>" + String(i) + ": " + String(inputState[i]) + "</td>";
    if ((i + 1) % 8 == 0) html += "</tr><tr>";
  }
  html += "</tr></table>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  int idx = server.arg("r").toInt();
  if (idx < 24) {
    relayState[idx] = !relayState[idx];
    int chip = idx / 8;
    int pin = idx % 8;
    if (chip == 0) pcfRel1.digitalWrite(pin, relayState[idx]);
    if (chip == 1) pcfRel2.digitalWrite(pin, relayState[idx]);
    if (chip == 2) pcfRel3.digitalWrite(pin, relayState[idx]);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
