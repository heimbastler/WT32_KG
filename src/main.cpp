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
#include <PCF8574.h> // wieder hinzufügen für die Switches
#include <Adafruit_PCA9535.h>
#include "Adafruit_MPR121.h"

// ---------- I2C Setup ----------
#define SDA_PIN 33
#define SCL_PIN 32

// ---------- PCF8574 Adressen für Switches ----------
PCF8574 pcfIn1(0x20);    // A2 = GND, A1 = GND, A0 = GND
PCF8574 pcfIn2(0x21);    // A2 = GND, A1 = GND, A0 = VCC

// ---------- PCA9535 Adressen für Relais ----------
Adafruit_PCA9535 pcaRel1 = Adafruit_PCA9535();
Adafruit_PCA9535 pcaRel2 = Adafruit_PCA9535();
Adafruit_PCA9535 pcaRel3 = Adafruit_PCA9535();

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

// ---------- Relaisnamen, Index entspricht R00...R23 ----------
// ACHTUNG: Index 0 = R00, Index 1 = R01, ... Index 22 = R22, Index 23 = R23
// TouchBoard-Positionen als Kommentar aus old.ccp ergänzt
const char* relayNames[24] = {
  "Fensterrollo hoch",      // R00, idx 0, TouchBoard1: case 9: Taster für Fensterrollo up
  "Fensterrollo runter",    // R01, idx 1, TouchBoard1: case 10: Taster für Fensterrollo down
  "Tuerrollo hoch",         // R02, idx 2, TouchBoard1: case 6: push button for Türrollo up
  "Tuerrollo runter",       // R03, idx 3, TouchBoard1: case 11: push button for Türrollo down
  "Aussenlampe Garten",     // R04, idx 4, TouchBoard1: case 0: unten 2te von links
  "Steinlampe",             // R05, idx 5, TouchBoard2: case 6: 2te links (auskommentiert in old)
  "KG Flurlampe",           // R06, idx 6, TouchBoard1: kein direkter Touch, Schalter/EG/KG
  "Kuechenarbeitslampe",    // R07, idx 7, TouchBoard2: case 3: unten links
  "Kuechenlampe",           // R08, idx 8, TouchBoard2: case 2: 3te links
  "EG Flurlampe",           // R09, idx 9, TouchBoard2: case 0: 3te rechts
  "Traegerlampen",          // R10, idx 10, TouchBoard2: case 1: unten rechts
  "Wohnzimmerlampe 1",      // R11, idx 11, TouchBoard1: case 2: oben 1te von links
  "Wohnzimmerlampe 2",      // R12, idx 12, TouchBoard1: kein direkter Touch, nur Gruppe
  "Relais 13",              // R13, idx 13
  "Relais 14",              // R14, idx 14
  "Relais 15",              // R15, idx 15
  "Relais 16",              // R16, idx 16
  "Relais 17",              // R17, idx 17
  "Relais 18",              // R18, idx 18
  "Relais 19",              // R19, idx 19
  "Relais 20",              // R20, idx 20
  "Relais 21",              // R21, idx 21
  "Relais 22",              // R22, idx 22
  "Relais 23"               // R23, idx 23
};

// --- Funktionsprototypen für Relaisaktionen ---
void toggleFensterrolloUp();
void toggleFensterrolloDown();
void toggleTuerrolloUp();
void toggleTuerrolloDown();
void toggleAussenlampeGarten();
void toggleSteinlampe();
void toggleKGFlurlampe();
void toggleKuechenarbeitslampe();
void toggleKuechenlampe();
void toggleEGFlurlampe();
void toggleTraegerlampen();
void toggleWohnzimmerlampe1();
void toggleWohnzimmerlampe2();
void toggleAussenlampeStrasse();
void toggleKlingeltrafo();
void toggleLamps();

// --- Zeitsteuerung für Rollos ---
unsigned long fensterrolloTimer = 0;
unsigned long tuerrolloTimer = 0;
const unsigned long rolloActiveTime = 60000; // ms, wie lange das Relais anzieht (1 Minute)

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

  // PCF8574 Switches starten
  pcfIn1.begin();
  pcfIn2.begin();

  // PCA9535 Relais starten
  pcaRel1.begin(0x22);
  pcaRel2.begin(0x23);
  pcaRel3.begin(0x24);

  // Switches als INPUT
  for (int i = 0; i < 8; i++) {
    pcfIn1.pinMode(i, INPUT);
    pcfIn2.pinMode(i, INPUT);
  }

  // Relais als OUTPUT und alle AUS (HIGH)
  for (int i = 0; i < 8; i++) {
    pcaRel1.pinMode(i, OUTPUT);
    pcaRel1.digitalWrite(i, HIGH);
    pcaRel2.pinMode(i, OUTPUT);
    pcaRel2.digitalWrite(i, HIGH);
    pcaRel3.pinMode(i, OUTPUT);
    pcaRel3.digitalWrite(i, HIGH);
  }
  for (int i = 0; i < 24; i++) {
    relayState[i] = 0;
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

  // Eingänge lesen (weiterhin über PCF8574)
  for (int i = 0; i < 8; i++) {
    inputState[i] = pcfIn1.digitalRead(i);
    inputState[i + 8] = pcfIn2.digitalRead(i);
  }

  // IRQ über PCF erkennen
  bool mprIRQ = (pcfIn1.digitalRead(MPR_IRQ_PCF_PIN) == LOW);
  if (mprIRQ) {
    checkMPR(cap1, "MPR121 #1", 0);
    checkMPR(cap2, "MPR121 #2", 1);
    checkMPR(cap3, "MPR121 #3", 2);
  }

  // --- Fensterrollo nach Zeit abschalten ---
  if (fensterrolloTimer > 0 && millis() - fensterrolloTimer > rolloActiveTime) {
    relayState[0] = 0; relayState[1] = 0;
    pcaRel1.digitalWrite(0, HIGH);
    pcaRel1.digitalWrite(1, HIGH);
    fensterrolloTimer = 0;
  }
  // --- Türrollo nach Zeit abschalten ---
  if (tuerrolloTimer > 0 && millis() - tuerrolloTimer > rolloActiveTime) {
    relayState[2] = 0; relayState[3] = 0;
    pcaRel1.digitalWrite(2, HIGH);
    pcaRel1.digitalWrite(3, HIGH);
    tuerrolloTimer = 0;
  }

  delay(50);
}

// ======================================================
// Webserver Handler
// ======================================================
void handleRoot() {
  String html = "<html><body><h2>WT32-ETH01 Steuerung</h2>";

  html += "<h3>Relais</h3>";
  for (int i = 0; i < 24; i++) {
    html += "<button onclick=\"location.href='/toggle?r=" + String(i) + "'\">";
    html += String(relayNames[i]) + ": ";
    html += (relayState[i]) ? "EIN" : "AUS";
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
    if (chip == 0) pcaRel1.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
    if (chip == 1) pcaRel2.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
    if (chip == 2) pcaRel3.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}



// --- Relaisaktionsfunktionen mit LOW-aktiv Logik ---
// Index und Funktion immer klar kommentiert!

void toggleFensterrolloUp() {
  // R00 (idx 0): Fensterrollo hoch EIN, Fensterrollo runter AUS
  // TouchBoard1: case 9: Taster für Fensterrollo up
  relayState[0] = 1;
  relayState[1] = 0;
  pcaRel1.digitalWrite(0, LOW);   // EIN
  pcaRel1.digitalWrite(1, HIGH);  // AUS
  fensterrolloTimer = millis();
}
void toggleFensterrolloDown() {
  // R01 (idx 1): Fensterrollo runter EIN, Fensterrollo hoch AUS
  // TouchBoard1: case 10: Taster für Fensterrollo down
  relayState[0] = 0;
  relayState[1] = 1;
  pcaRel1.digitalWrite(0, HIGH);  // AUS
  pcaRel1.digitalWrite(1, LOW);   // EIN
  fensterrolloTimer = millis();
}
void toggleTuerrolloUp() {
  // R02 (idx 2): Türrollo hoch EIN, Türrollo runter AUS
  // TouchBoard1: case 6: push button for Türrollo up
  relayState[2] = 1;
  relayState[3] = 0;
  pcaRel1.digitalWrite(2, LOW);
  pcaRel1.digitalWrite(3, HIGH);
  tuerrolloTimer = millis();
}
void toggleTuerrolloDown() {
  // R03 (idx 3): Türrollo runter EIN, Türrollo hoch AUS
  // TouchBoard1: case 11: push button for Türrollo down
  relayState[2] = 0;
  relayState[3] = 1;
  pcaRel1.digitalWrite(2, HIGH);
  pcaRel1.digitalWrite(3, LOW);
  tuerrolloTimer = millis();
}
void toggleAussenlampeGarten() {
  // R04 (idx 4)
  // TouchBoard1: case 0: unten 2te von links
  int idx = 4;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? LOW : HIGH);
}
void toggleSteinlampe() {
  // R05 (idx 5)
  // TouchBoard2: case 6: 2te links (auskommentiert in old)
  int idx = 5;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? LOW : HIGH);
}
void toggleKGFlurlampe() {
  // R06 (idx 6)
  // Kein direkter Touch, Schalter/EG/KG
  int idx = 6;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? LOW : HIGH);
}
void toggleKuechenarbeitslampe() {
  // R07 (idx 7)
  // TouchBoard2: case 3: unten links
  int idx = 7;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(0, relayState[idx] ? LOW : HIGH);
}
void toggleKuechenlampe() {
  // R08 (idx 8)
  // TouchBoard2: case 2: 3te links
  int idx = 8;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(1, relayState[idx] ? LOW : HIGH);
}
void toggleEGFlurlampe() {
  // R09 (idx 9)
  // TouchBoard2: case 0: 3te rechts
  int idx = 9;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(2, relayState[idx] ? LOW : HIGH);
}
void toggleTraegerlampen() {
  // R10 (idx 10)
  // TouchBoard2: case 1: unten rechts
  int idx = 10;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(3, relayState[idx] ? LOW : HIGH);
}
void toggleWohnzimmerlampe1() {
  // R11 (idx 11)
  // TouchBoard1: case 2: oben 1te von links
  int idx = 11;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(4, relayState[idx] ? LOW : HIGH);
}
void toggleWohnzimmerlampe2() {
  // R12 (idx 12)
  // Kein direkter Touch, nur Gruppe
  int idx = 12;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(5, relayState[idx] ? LOW : HIGH);
}
void toggleAussenlampeStrasse() {
  // R13 (idx 13)
  int idx = 13;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(6, relayState[idx] ? LOW : HIGH);
}
void toggleKlingeltrafo() {
  // R14 (idx 14)
  int idx = 14;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(7, relayState[idx] ? LOW : HIGH);
}
void toggleLamps() {
  // Gruppe: R05, R07, R08, R09, R10, R11, R12 (idx 5,7,8,9,10,11,12)
  // Touch Zuordnung siehe readTouchInputs2/readTouchInputs3
  for (int idx : {5,7,8,9,10,11,12}) {
    relayState[idx] = !relayState[idx];
    int chip = idx / 8;
    int pin = idx % 8;
    if (chip == 0) pcaRel1.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
    if (chip == 1) pcaRel2.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
    if (chip == 2) pcaRel3.digitalWrite(pin, relayState[idx] ? LOW : HIGH);
  }
}

// ======================================================
// Hilfsfunktion: MPR121 Status prüfen und Relais schalten
// ======================================================
void checkMPR(Adafruit_MPR121 &sensor, const char *name, int sensorIdx) {
  uint16_t touched = sensor.touched();
  static uint16_t lastTouched[3] = {0, 0, 0};
  if (touched != lastTouched[sensorIdx]) {
    uint16_t changed = touched ^ lastTouched[sensorIdx];
    for (int i = 0; i < 12; i++) {
      if (changed & (1 << i)) {
        bool isTouched = touched & (1 << i);
        // --- Zuordnung wie in old.ccp ---
        if (sensorIdx == 0) { // TouchBoard 1: Tür Garten EG
          if (isTouched) {
            switch (i) {
              case 2: toggleWohnzimmerlampe1(); break;    // oben 1te von links
              case 5: toggleTraegerlampen(); break;       // oben 2te von links
              case 1: toggleEGFlurlampe(); break;         // unten 1te von links
              case 0: toggleAussenlampeGarten(); break;   // unten 2te von links
              case 9: toggleFensterrolloUp(); break;      // Taster für Fensterrollo up
              case 10: toggleFensterrolloDown(); break;   // Taster für Fensterrollo down
              case 6: toggleTuerrolloUp(); break;         // push button for Türrollo up
              case 11: toggleTuerrolloDown(); break;      // push button for Türrollo down
            }
          }
        } else if (sensorIdx == 1) { // TouchBoard 2: Säule Garten EG
          if (isTouched) {
            switch (i) {
              case 9: toggleLamps(); break;               // oben links
              case 11: toggleLamps(); break;              // oben rechts
              case 10: toggleWohnzimmerlampe1(); break;   // 2te rechts
              case 1: toggleTraegerlampen(); break;       // unten rechts
              case 2: toggleKuechenlampe(); break;        // 3te links
              case 3: toggleKuechenarbeitslampe(); break; // unten links
              case 0: toggleEGFlurlampe(); break;         // 3te rechts
              case 6: toggleSteinlampe(); break;          // 2te links
            }
          }
        } else if (sensorIdx == 2) { // TouchBoard 3: Säule Strasse EG
          if (isTouched) {
            switch (i) {
              case 9: toggleLamps(); break;               // oben links
              case 10: toggleLamps(); break;              // oben rechts
              case 11: toggleWohnzimmerlampe1(); break;   // 2te rechts
              case 1: toggleTraegerlampen(); break;       // unten rechts
              case 3: toggleKuechenlampe(); break;        // 3te links
              case 2: toggleKuechenarbeitslampe(); break; // unten links
              case 0: toggleEGFlurlampe(); break;         // 3te rechts
              case 6: toggleSteinlampe(); break;          // 2te links
            }
          }
        }
      }
    }
    lastTouched[sensorIdx] = touched;
  }
}
