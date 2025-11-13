// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    WT32-ETH01 (ESP32-WROOM-32) Pin Layout                    ║
// ║                              Version 1.4                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

//  ┌─────────────────┐                               ┌─────────────────┐
//  │   LINKS         │                               │    RECHTS       │
//  ├─────────────────┤                               ├─────────────────┤
//  │ EN/RST/BOOT     │                               │ 3.3V            │ ← Versorgung
//  │                 │                               │                 │
//  │ GPIO00  ⚠️      │ BOOT/Flash strapping          │ GPIO01  ❌      │ TX0/UART0
//  │ GPIO02  ⚠️      │ strapping (low@boot)          │ GPIO03  ❌      │ RX0/UART0
//  │ GPIO04  ✅      │ frei                          │ GPIO05  ✅      │ frei
//  │ GPIO12  🔸      │ 1-Wire Temperatursensoren     │ GPIO13  ✅      │ frei
//  │ GPIO14  🔸      │ LED Dimmer (PWM)              │ GPIO15  ⚠️      │ strapping (low)
//  │ GPIO16  ✅      │ UART2 RX                      │ GPIO17  💡      │ OnBoard LED
//  │ GPIO18  ❌      │ ETH_MDIO (LAN8720)            │ GPIO19  ❌      │ ETH_TXD0
//  │ GPIO20  ✅      │ frei                          │ GPIO21  ❌      │ ETH_CLK_OUT
//  │ GPIO22  ❌      │ ETH_RXD0 (LAN8720)            │ GPIO23  ❌      │ ETH_MDC
//  │ GPIO24  ✅      │ frei                          │ GPIO25  ❌      │ ETH_TX_EN
//  │ GPIO26  ❌      │ ETH_RX_ER (LAN8720)           │ GPIO27  ❌      │ ETH_CRS_DV
//  │ GPIO32  📡      │ I2C SCL                       │ GPIO33  📡      │ I2C SDA
//  │ GPIO34  📥      │ Input only/ADC                │ GPIO35  📥      │ Input only/ADC
//  │ GPIO36  📥      │ Input only/ADC (VP)           │ GPIO37  ✅      │ frei
//  │ GPIO38  ✅      │ frei                          │ GPIO39  📥      │ Input only/ADC
//  │                 │                               │                 │
//  │ GND             │ Masse                         │ GND             │ Masse
//  └─────────────────┘                               └─────────────────┘

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                           AKTUELLE PIN BELEGUNG                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 🔸 GPIO4   → AC Dimmer Kronleuchter (PWM, 220V Dimmer)
// 🔸 GPIO12  → 1-Wire Bus Temperatursensoren (needs 4.7k pullup)
// 🔸 GPIO14  → LED Dimmer Kellertreppe (PWM, MOSFET Gate)
// 📡 GPIO32  → I2C SCL (PCF8574, PCA9535, MPR121)
// 📡 GPIO33  → I2C SDA (PCF8574, PCA9535, MPR121)
// 💡 GPIO17  → OnBoard Status LED (aktiv HIGH)

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                              I2C GERÄTE                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// PCF8574 Input Boards:  0x20, 0x21    (Switch Eingänge)
//   PCF8574 #1 (0x20): Pin 0+1 = IR-Switch Küche EG11, Pin 2-6 = Kreuzschaltungen, Pin 7 = frei
//   PCF8574 #2 (0x21): Pin 0-7 = frei für weitere Eingänge
// PCA9535 Relay Boards:  0x22, 0x23, 0x24 (Relais Ausgänge)
// MPR121 Touch Sensors:  0x5A, 0x5C, 0x5D  (Capacitive Touch)

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                            PIN KATEGORIEN                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// ✅ FREI      → Verfügbar für weitere Funktionen
// 🔸 VERWENDET → Aktuell in diesem Projekt belegt
// ❌ RESERVIERT→ Ethernet LAN8720 oder UART (nicht verwenden)
// ⚠️ VORSICHT  → Boot-relevant oder spezielle Funktion
// 📡 I2C       → Für I2C Bus reserviert
// 📥 INPUT     → Nur Eingänge möglich (GPIO34-39)
// 💡 STATUS    → OnBoard LED




#include <Wire.h>
#include <WebServer.h>
#include <ETH.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <PCF8574.h> // wieder hinzufügen für die Switches
#include <Adafruit_PCA9535.h>
#include "Adafruit_MPR121.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// --- TouchBoards Funktionsprototypen ---
void initTouchBoards();
void handleTouchEvents();

// --- Network Funktionsprototypen ---
void initNetworking();
void handleWiFiEvents();
String getNetworkStatus();

// ---------- I2C Setup ----------
#define SDA_PIN 33
#define SCL_PIN 32

// ---------- PWM Setup für LED Dimmer ----------
#define LED_TREPPE_PIN 14        // GPIO14 für MOSFET Kellertreppe LEDs
#define KRONLEUCHTER_DIMMER_PIN 4  // GPIO4 für AC Dimmer Kronleuchter 220V
#define PWM_CHANNEL_TREPPE 0     // LEDC Kanal 0 für Kellertreppe
#define PWM_CHANNEL_KRONLEUCHTER 1 // LEDC Kanal 1 für Kronleuchter
#define PWM_FREQ 5000            // 5kHz Frequenz
#define PWM_RESOLUTION 8         // 8-bit Resolution (0-255)

// ---------- 1-Wire Setup für Temperatursensoren ----------
#define ONE_WIRE_BUS 12      // GPIO12 für DS18B20 Temperatursensoren
#define TEMPERATURE_PRECISION 10  // 10-bit = 0.25°C Auflösung

// ---------- PCF8574 Adressen für Switches ----------
PCF8574 pcfIn1(0x20);    // A2 = GND, A1 = GND, A0 = GND
PCF8574 pcfIn2(0x21);    // A2 = GND, A1 = GND, A0 = VCC

// ---------- IR-Switch Küche Kabel EG11 ----------
// Kabel EG11: br/ws = +5V, br = GND, grn = links, grn/ws = rechts
#define IR_SWITCH_KITCHEN_LEFT  0   // PCF8574 #1 Pin 0 → grn (linker Taster)
#define IR_SWITCH_KITCHEN_RIGHT 1   // PCF8574 #1 Pin 1 → grn/ws (rechter Taster)
// Beide Taster gegen br = GND, Pull-up über PCF8574

// ---------- Kreuzlichtschaltungen ----------
// EG Kreuzschaltung (2 Schalter für EG Flurlampe R09)
#define KREUZ_EG1 2              // PCF8574 #1 Pin 2 → Kabel EG10 (unterer Schalter Treppe OG)
#define KREUZ_EG2 3              // PCF8574 #1 Pin 3 → Kabel EG1 (oberer Schalter Eingang EG)
// KREUZ_EG3 nicht im Einsatz (war Pin 53 im alten System)

// KG Kreuzschaltung (3 Schalter für KG Flurlampe R06)  
#define KREUZ_KG1 4              // PCF8574 #1 Pin 4 → Kabel KG1 (Schalter Tür Schlafzimmer, gelb)
#define KREUZ_KG2 5              // PCF8574 #1 Pin 5 → Kabel KG1 (oberer Schalter Bad KG, weiss)
#define KREUZ_KG3 6              // PCF8574 #1 Pin 6 → Kabel EG1 (Schalter Treppe zum KG im EG, braun)

// ---------- PCA9535 Adressen für Relais ----------
// Base-Adresse: 0x20, finale Adresse = 0x20 + A2×4 + A1×2 + A0×1
// KONFLIKT VERMIEDEN: PCF8574 nutzt bereits 0x20,0x21 → PCA9535 ab 0x22
Adafruit_PCA9535 pcaRel1 = Adafruit_PCA9535(); // RelaisBoard_A: 0x22 → A2=offen, A1=löten, A0=offen
Adafruit_PCA9535 pcaRel2 = Adafruit_PCA9535(); // RelaisBoard_B: 0x23 → A2=offen, A1=löten, A0=löten
Adafruit_PCA9535 pcaRel3 = Adafruit_PCA9535(); // RelaisBoard_C: 0x24 → A2=löten, A1=offen, A0=offen

// ---------- MPR121 Sensoren ----------
Adafruit_MPR121 cap1 = Adafruit_MPR121(); // TouchBoard 1: 0x5A → Tür Garten EG
Adafruit_MPR121 cap2 = Adafruit_MPR121(); // TouchBoard 2: 0x5C → Säule Garten EG
Adafruit_MPR121 cap3 = Adafruit_MPR121(); // TouchBoard 3: 0x5D → Säule Strasse EG

// ---------- 1-Wire Temperatursensoren ----------
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperaturSensoren(&oneWire);



// ---------- Webserver ----------
WebServer server(80);

// ---------- Zustandsspeicher ----------
uint8_t relayState[24];   // 3 PCFs à 8 Ausgänge
uint8_t inputState[16];   // 2 PCFs à 8 Eingänge
uint8_t ledTreppeBrightness = 0;      // LED Kellertreppe Helligkeit (0-255)
uint8_t kronleuchterBrightness = 0;   // Kronleuchter AC Dimmer Helligkeit (0-255)
bool kronleuchterDimmingUp = true;    // Dimm-Richtung für Touch-Steuerung
unsigned long lastTouchTime = 0;      // Zeitstempel letzter Touch
const unsigned long touchDebounceTime = 200;  // 200ms Entprellung

// ---------- IR-Switch Küche Zustand ----------
uint8_t lastIRSwitchLeft = LOW;   // Letzter Zustand linker Taster
uint8_t lastIRSwitchRight = LOW;  // Letzter Zustand rechter Taster

// ---------- Kreuzschaltung Zustand ----------
uint8_t kreuzstateEG = 1;         // EG Kreuzschaltung Zustand (Bit-kombiniert)
uint8_t kreuzstateKG = 1;         // KG Kreuzschaltung Zustand (Bit-kombiniert)

// ---------- Temperatursensoren Zustand ----------
float sensorTemperaturen[5];      // Max 5 Temperatursensoren
float lastSensorTemperaturen[5];  // Letzte Werte für Änderungserkennung
uint8_t anzahlSensoren = 0;       // Anzahl gefundener Sensoren
unsigned long lastTempUpdate = 0; // Zeitstempel letzte Messung
const unsigned long tempUpdateInterval = 60000; // 1 Minute

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
  "Kronleuchter",           // R11, idx 11, TouchBoard1: case 2: AC Dimmer über GPIO4
  "Reserve Wohnzimmer",     // R12, idx 12, TouchBoard1: kein direkter Touch, Reserve
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
void toggleKronleuchter();
void dimKronleuchter(bool dimUp);
void toggleWohnzimmerlampe2();
void toggleLamps();

// --- PWM Funktionsprototypen ---
void setLEDTreppeBrightness(uint8_t brightness);
void setKronleuchterBrightness(uint8_t brightness);

// --- 1-Wire Temperatursensor Funktionsprototypen ---
void initTemperatureSensors();
void updateTemperatures();
String getTemperatureHTML();

// --- IR-Switch Küche Funktionsprototypen ---
void handleIRSwitchKitchen();

// --- Kreuzschaltung Funktionsprototypen ---
void handleKreuzschaltungEG();
void handleKreuzschaltungKG();

// --- Webserver Funktionsprototypen ---
void handleRoot();
void handleToggle();
void handleLEDDimmer();



// --- Zeitsteuerung für Rollos ---
unsigned long fensterrolloTimer = 0;
unsigned long tuerrolloTimer = 0;
const unsigned long rolloActiveTime = 60000; // ms, wie lange das Relais anzieht (1 Minute)

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  Serial.println("=== WT32-KG Smart Home Controller ===");
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(500);

  // Netzwerk initialisieren (Ethernet + WiFi + AP)
  initNetworking();

  // OTA Setup nach Netzwerk-Initialisierung
  ArduinoOTA.setHostname("WT32-KG-Controller");
  ArduinoOTA.setPassword("WT32_SecureOTA_2024"); // Sicheres Passwort
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Start updating " + type);
    // Sicherheitshalber alle Relais ausschalten während Update
    for (int i = 0; i < 8; i++) {
      pcaRel1.digitalWrite(i, HIGH);
      pcaRel2.digitalWrite(i, HIGH);  
      pcaRel3.digitalWrite(i, HIGH);
    }
    // LEDs ausschalten
    setLEDTreppeBrightness(0);
    setKronleuchterBrightness(0);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update completed successfully!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA Ready - Hostname: WT32-KG-Controller");

  // PCF8574 Switches starten
  pcfIn1.begin();
  pcfIn2.begin();

  // PCA9535 Relais starten
  pcaRel1.begin(0x22); // RelaisBoard_A: A2=offen, A1=löten, A0=offen
  pcaRel2.begin(0x23); // RelaisBoard_B: A2=offen, A1=löten, A0=löten
  pcaRel3.begin(0x24); // RelaisBoard_C: A2=löten, A1=offen, A0=offen

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

  // PWM für LED Dimmer konfigurieren
  ledcAttach(LED_TREPPE_PIN, PWM_FREQ, PWM_RESOLUTION);
  setLEDTreppeBrightness(0); // Starten mit LEDs aus
  
  // PWM für Kronleuchter AC Dimmer konfigurieren
  ledcAttach(KRONLEUCHTER_DIMMER_PIN, PWM_FREQ, PWM_RESOLUTION);
  setKronleuchterBrightness(0); // Starten mit Kronleuchter aus

  // TouchBoards initialisieren
  initTouchBoards();

  // Temperatursensoren initialisieren
  initTemperatureSensors();

  // Webserver konfigurieren
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/led", handleLEDDimmer);
  server.on("/kronleuchter", handleKronleuchterDimmer);
  server.begin();
  Serial.println("Webserver gestartet");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  ArduinoOTA.handle();  // OTA Updates verarbeiten
  server.handleClient();
  
  // Netzwerk Events überwachen
  handleWiFiEvents();

  // Eingänge lesen (weiterhin über PCF8574)
  for (int i = 0; i < 8; i++) {
    inputState[i] = pcfIn1.digitalRead(i);
    inputState[i + 8] = pcfIn2.digitalRead(i);
  }

  // Touch Events verarbeiten
  handleTouchEvents();

  // IR-Switch Küche überwachen
  handleIRSwitchKitchen();

  // Kreuzschaltungen überwachen
  handleKreuzschaltungEG();
  handleKreuzschaltungKG();

  // Temperaturen alle 1 Minute aktualisieren
  if (millis() - lastTempUpdate > tempUpdateInterval) {
    updateTemperatures();
    lastTempUpdate = millis();
  }

  // --- Sicherheits-Timeout für Rollos (5 Minuten) ---
  // Verhindert dass Rollos bei Fehler endlos laufen
  if (fensterrolloTimer > 0 && millis() - fensterrolloTimer > (rolloActiveTime * 5)) {
    Serial.println("SICHERHEITS-STOPP Fensterrollo nach 5 Minuten");
    relayState[0] = 0; relayState[1] = 0;
    pcaRel1.digitalWrite(0, HIGH);
    pcaRel1.digitalWrite(1, HIGH);
    fensterrolloTimer = 0;
  }
  if (tuerrolloTimer > 0 && millis() - tuerrolloTimer > (rolloActiveTime * 5)) {
    Serial.println("SICHERHEITS-STOPP Türrollo nach 5 Minuten");
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
  String html = "<html><head><title>WT32-KG Controller</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;margin:10px;background:#f5f5f5;}";
  html += ".container{max-width:1200px;margin:0 auto;background:white;padding:20px;border-radius:10px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}";
  html += "h2{color:#333;text-align:center;border-bottom:2px solid #4CAF50;padding-bottom:10px;}";
  html += "h3{color:#555;margin-top:30px;border-left:4px solid #2196F3;padding-left:15px;}";
  
  // Button Styles
  html += ".btn{padding:12px 20px;margin:5px;border:none;border-radius:8px;cursor:pointer;font-size:14px;font-weight:bold;transition:all 0.3s;min-width:200px;display:inline-block;text-align:center;text-decoration:none;}";
  html += ".btn:hover{transform:translateY(-2px);box-shadow:0 4px 8px rgba(0,0,0,0.2);}";
  html += ".btn-on{background:#4CAF50;color:white;}";
  html += ".btn-off{background:#f44336;color:white;}";
  html += ".btn-rollo{background:#FF9800;color:white;min-width:100px;margin:2px;}";
  html += ".btn-neutral{background:#607D8B;color:white;}";
  
  // Layout Styles
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:20px;margin:20px 0;}";
  html += ".card{background:#fafafa;padding:15px;border-radius:8px;border:1px solid #ddd;}";
  html += ".rollo-group{display:flex;flex-wrap:wrap;gap:5px;align-items:center;margin:10px 0;}";
  html += ".rollo-label{min-width:180px;font-weight:bold;}";
  
  // Table Styles
  html += "table{border-collapse:collapse;width:100%;margin:15px 0;}";
  html += "td,th{border:1px solid #ddd;padding:8px;text-align:center;}";
  html += "th{background:#f0f0f0;font-weight:bold;}";
  html += "</style></head><body>";
  
  html += "<div class='container'>";
  html += "<h2>🏠 WT32-KG Smart Home Controller</h2>";
  
  // Netzwerk Status
  html += getNetworkStatus();
  
  // LED Dimmer
  html += "<h3>💡 LED Dimmer Kellertreppe</h3>";
  int brightnessPercent = (ledTreppeBrightness * 100) / 255;
  html += "<div style='margin:20px 0;'>";
  html += "<label for='ledSlider'>Helligkeit: <b>" + String(brightnessPercent) + "%</b></label><br>";
  html += "<input type='range' id='ledSlider' min='0' max='100' value='" + String(brightnessPercent) + "' ";
  html += "style='width:300px;' oninput='updateLED(this.value)'><br>";
  html += "<span style='font-size:12px;'>0%</span>";
  html += "<span style='float:right;font-size:12px;'>100%</span>";
  html += "</div>";
  
  html += "<script>";
  html += "function updateLED(value) {";
  html += "  document.querySelector('label b').textContent = value + '%';";
  html += "  fetch('/led?brightness=' + value);";
  html += "}";
  html += "</script>";
  
  // Kronleuchter AC Dimmer
  html += "<h3>💡 AC Dimmer Kronleuchter</h3>";
  int kronleuchterPercent = (kronleuchterBrightness * 100) / 255;
  html += "<div style='margin:20px 0;'>";
  html += "<label for='kronleuchterSlider'>Helligkeit: <b>" + String(kronleuchterPercent) + "%</b></label><br>";
  html += "<input type='range' id='kronleuchterSlider' min='0' max='100' value='" + String(kronleuchterPercent) + "' ";
  html += "style='width:300px;' oninput='updateKronleuchter(this.value)'><br>";
  html += "<span style='font-size:12px;'>0%</span>";
  html += "<span style='float:right;font-size:12px;'>100%</span>";
  html += "</div>";
  
  html += "<script>";
  html += "function updateKronleuchter(value) {";
  html += "  document.querySelectorAll('label b')[1].textContent = value + '%';";
  html += "  fetch('/kronleuchter?brightness=' + value);";
  html += "}";
  html += "</script>";

  // Rollläden
  html += "<h3>🪟 Rollläden</h3>";
  html += "<div class='card'>";
  
  // Fensterrollo
  html += "<div class='rollo-group'>";
  html += "<span class='rollo-label'>Fensterrollo:</span>";
  
  // Dynamische Button-Symbole je nach Status
  String fensterHochSymbol = (relayState[0] == 1) ? "⏹️ Stopp" : "▲ Hoch";
  String fensterRunterSymbol = (relayState[1] == 1) ? "⏹️ Stopp" : "▼ Runter";
  String fensterHochClass = (relayState[0] == 1) ? "btn-neutral" : "btn-rollo";
  String fensterRunterClass = (relayState[1] == 1) ? "btn-neutral" : "btn-rollo";
  
  html += "<a href='/toggle?r=0' class='btn " + fensterHochClass + "'>" + fensterHochSymbol + "</a>";
  html += "<a href='/toggle?r=1' class='btn " + fensterRunterClass + "'>" + fensterRunterSymbol + "</a>";
  
  String fensterStatus = "";
  if (relayState[0] == 1) fensterStatus = " (Fährt hoch)";
  else if (relayState[1] == 1) fensterStatus = " (Fährt runter)";
  else fensterStatus = " (Stopp)";
  html += "<span style='margin-left:10px;color:#666;'>" + fensterStatus + "</span>";
  html += "</div>";
  
  // Türrollo  
  html += "<div class='rollo-group'>";
  html += "<span class='rollo-label'>Türrollo:</span>";
  
  // Dynamische Button-Symbole je nach Status
  String tuerHochSymbol = (relayState[2] == 1) ? "⏹️ Stopp" : "▲ Hoch";
  String tuerRunterSymbol = (relayState[3] == 1) ? "⏹️ Stopp" : "▼ Runter";
  String tuerHochClass = (relayState[2] == 1) ? "btn-neutral" : "btn-rollo";
  String tuerRunterClass = (relayState[3] == 1) ? "btn-neutral" : "btn-rollo";
  
  html += "<a href='/toggle?r=2' class='btn " + tuerHochClass + "'>" + tuerHochSymbol + "</a>";
  html += "<a href='/toggle?r=3' class='btn " + tuerRunterClass + "'>" + tuerRunterSymbol + "</a>";
  
  String tuerStatus = "";
  if (relayState[2] == 1) tuerStatus = " (Fährt hoch)";
  else if (relayState[3] == 1) tuerStatus = " (Fährt runter)";
  else tuerStatus = " (Stopp)";
  html += "<span style='margin-left:10px;color:#666;'>" + tuerStatus + "</span>";
  html += "</div>";
  
  html += "</div>";

  // Lampen - alle zusammengefasst und nach Relaisnummer sortiert
  html += "<h3>💡 Lampen</h3>";
  html += "<div class='card'>";
  
  // R04 - Aussenlampe Garten
  String aussenGartenClass = relayState[4] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=4' class='btn " + aussenGartenClass + "'>💡 Aussenlampe Garten (R04)</a><br>";
  
  // R05 - Steinlampe
  String steinlampeClass = relayState[5] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=5' class='btn " + steinlampeClass + "'>💡 Steinlampe (R05)</a><br>";
  
  // R06 - KG Flurlampe
  String kgFlurClass = relayState[6] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=6' class='btn " + kgFlurClass + "'>💡 KG Flurlampe (R06)</a><br>";
  
  // R07 - Kuechenarbeitslampe
  String kuechenArbeitClass = relayState[7] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=7' class='btn " + kuechenArbeitClass + "'>💡 Kuechenarbeitslampe (R07)</a><br>";
  
  // R08 - Kuechenlampe
  String kuechenClass = relayState[8] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=8' class='btn " + kuechenClass + "'>💡 Kuechenlampe (R08)</a><br>";
  
  // R09 - EG Flurlampe
  String egFlurClass = relayState[9] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=9' class='btn " + egFlurClass + "'>💡 EG Flurlampe (R09)</a><br>";
  
  // R10 - Traegerlampen
  String traegerClass = relayState[10] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=10' class='btn " + traegerClass + "'>💡 Traegerlampen (R10)</a><br>";
  
  // R11 - Kronleuchter (mit Dimmerfunktion)
  String kronleuchterClass = relayState[11] ? "btn-on" : "btn-off";
  int kronPercent = (kronleuchterBrightness * 100) / 255;
  html += "<a href='/toggle?r=11' class='btn " + kronleuchterClass + "'>💡 Kronleuchter (R11)";
  if (relayState[11]) {
    html += " (" + String(kronPercent) + "%)";
  }
  html += "</a><br>";
  
  // R12 - Reserve Wohnzimmer
  String wohnzimmer2Class = relayState[12] ? "btn-on" : "btn-off";
  html += "<a href='/toggle?r=12' class='btn " + wohnzimmer2Class + "'>💡 Reserve Wohnzimmer (R12)</a>";
  
  // R05 - Steinlampe
  String steinlampeClass = relayState[5] ? "btn-on" : "btn-off";
  String steinlampeStatus = relayState[5] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=5' class='btn " + steinlampeClass + "'>� Steinlampe (R05): " + steinlampeStatus + "</a><br>";
  
  // R06 - KG Flurlampe
  String kgFlurClass = relayState[6] ? "btn-on" : "btn-off";
  String kgFlurStatus = relayState[6] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=6' class='btn " + kgFlurClass + "'>� KG Flurlampe (R06): " + kgFlurStatus + "</a><br>";
  
  // R07 - Kuechenarbeitslampe
  String kuechenArbeitClass = relayState[7] ? "btn-on" : "btn-off";
  String kuechenArbeitStatus = relayState[7] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=7' class='btn " + kuechenArbeitClass + "'>� Kuechenarbeitslampe (R07): " + kuechenArbeitStatus + "</a><br>";
  
  // R08 - Kuechenlampe
  String kuechenClass = relayState[8] ? "btn-on" : "btn-off";
  String kuechenStatus = relayState[8] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=8' class='btn " + kuechenClass + "'>💡 Kuechenlampe (R08): " + kuechenStatus + "</a><br>";
  
  // R09 - EG Flurlampe
  String egFlurClass = relayState[9] ? "btn-on" : "btn-off";
  String egFlurStatus = relayState[9] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=9' class='btn " + egFlurClass + "'>💡 EG Flurlampe (R09): " + egFlurStatus + "</a><br>";
  
  // R10 - Traegerlampen
  String traegerClass = relayState[10] ? "btn-on" : "btn-off";
  String traegerStatus = relayState[10] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=10' class='btn " + traegerClass + "'>💡 Traegerlampen (R10): " + traegerStatus + "</a><br>";
  
  // R11 - Kronleuchter (mit Dimmerfunktion)
  String kronleuchterClass = relayState[11] ? "btn-on" : "btn-off";
  String kronleuchterStatus = relayState[11] ? "EIN" : "AUS";
  int kronPercent = (kronleuchterBrightness * 100) / 255;
  html += "<a href='/toggle?r=11' class='btn " + kronleuchterClass + "'>💡 Kronleuchter (R11): " + kronleuchterStatus;
  if (relayState[11]) {
    html += " (" + String(kronPercent) + "%)";
  }
  html += "</a><br>";
  
  // R12 - Reserve Wohnzimmer
  String wohnzimmer2Class = relayState[12] ? "btn-on" : "btn-off";
  String wohnzimmer2Status = relayState[12] ? "EIN" : "AUS";
  html += "<a href='/toggle?r=12' class='btn " + wohnzimmer2Class + "'>💡 Reserve Wohnzimmer (R12): " + wohnzimmer2Status + "</a>";
  html += "</div>";
  
  html += "</div>";

  // Freie Relais
  html += "<h3>⚙️ Freie Relais & Sonstiges</h3>";
  html += "<div class='card'>";
  
  // Freie Relais 13-23
  for (int i = 13; i < 15; i++) {
    String relaisClass = relayState[i] ? "btn-on" : "btn-off";
    String relaisStatus = relayState[i] ? "EIN" : "AUS";
    html += "<a href='/toggle?r=" + String(i) + "' class='btn " + relaisClass + "'>🔌 " + String(relayNames[i]) + ": " + relaisStatus + "</a><br>";
  }
  
  // Freie Relais 15-23
  for (int i = 15; i < 24; i++) {
    String relaisClass = relayState[i] ? "btn-on" : "btn-off";
    String relaisStatus = relayState[i] ? "EIN" : "AUS";
    html += "<a href='/toggle?r=" + String(i) + "' class='btn " + relaisClass + "'>🔌 " + String(relayNames[i]) + ": " + relaisStatus + "</a>";
    if (i % 3 == 2) html += "<br>"; // Umbruch nach 3 Buttons
  }
  
  html += "</div>";

  // Eingänge
  html += "<h3>📊 Digitale Eingänge</h3>";
  html += "<table><tr>";
  html += "<th>Eingang</th><th>Funktion</th><th>Status</th></tr>";
  
  String inputLabels[16] = {
    "IR-Küche Links", "IR-Küche Rechts", "Kreuz EG1", "Kreuz EG2", 
    "Kreuz KG1", "Kreuz KG2", "Kreuz KG3", "Frei",
    "Frei", "Frei", "Frei", "Frei", "Frei", "Frei", "Frei", "Frei"
  };
  
  for (int i = 0; i < 16; i++) {
    String status = inputState[i] ? "HIGH" : "LOW";
    String color = inputState[i] ? "lightgreen" : "lightcoral";
    html += "<tr>";
    html += "<td>IN" + String(i) + "</td>";
    html += "<td>" + inputLabels[i] + "</td>";
    html += "<td style='background-color:" + color + ";font-weight:bold;'>" + status + "</td>";
    html += "</tr>";
  }
  html += "</table>";
  
  // Temperatursensoren nach digitalen Eingängen
  html += getTemperatureHTML();
  
  html += "<br><a href='/wifi' style='color:#2196F3;text-decoration:none;'>📶 WiFi Konfiguration</a>";
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  int idx = server.arg("r").toInt();
  
  // Spezielle Behandlung für Rollos (Start/Stopp Logik)
  if (idx == 0) {
    toggleFensterrolloUp();
  } else if (idx == 1) {
    toggleFensterrolloDown();
  } else if (idx == 2) {
    toggleTuerrolloUp();
  } else if (idx == 3) {
    toggleTuerrolloDown();
  } else if (idx == 11) {
    // Spezielle Behandlung für Kronleuchter (AC Dimmer)
    toggleKronleuchter();
  } else if (idx < 24) {
    // Standard Toggle für alle anderen Relais
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
  // R00 (idx 0): Fensterrollo hoch - START/STOPP Logik
  // TouchBoard1: case 9: Taster für Fensterrollo up
  if (relayState[0] == 1) {
    // Bereits hoch aktiv → STOPP
    relayState[0] = 0;
    pcaRel1.digitalWrite(0, HIGH);  // AUS
    Serial.println("Fensterrollo STOPP (war hoch)");
    fensterrolloTimer = 0;
  } else {
    // Start hoch, runter stoppen
    relayState[0] = 1;
    relayState[1] = 0;
    pcaRel1.digitalWrite(0, LOW);   // Hoch EIN
    pcaRel1.digitalWrite(1, HIGH);  // Runter AUS
    Serial.println("Fensterrollo START hoch");
    fensterrolloTimer = millis();
  }
}

void toggleFensterrolloDown() {
  // R01 (idx 1): Fensterrollo runter - START/STOPP Logik
  // TouchBoard1: case 10: Taster für Fensterrollo down
  if (relayState[1] == 1) {
    // Bereits runter aktiv → STOPP
    relayState[1] = 0;
    pcaRel1.digitalWrite(1, HIGH);  // AUS
    Serial.println("Fensterrollo STOPP (war runter)");
    fensterrolloTimer = 0;
  } else {
    // Start runter, hoch stoppen
    relayState[0] = 0;
    relayState[1] = 1;
    pcaRel1.digitalWrite(0, HIGH);  // Hoch AUS
    pcaRel1.digitalWrite(1, LOW);   // Runter EIN
    Serial.println("Fensterrollo START runter");
    fensterrolloTimer = millis();
  }
}

void toggleTuerrolloUp() {
  // R02 (idx 2): Türrollo hoch - START/STOPP Logik
  // TouchBoard1: case 6: push button for Türrollo up
  if (relayState[2] == 1) {
    // Bereits hoch aktiv → STOPP
    relayState[2] = 0;
    pcaRel1.digitalWrite(2, HIGH);  // AUS
    Serial.println("Türrollo STOPP (war hoch)");
    tuerrolloTimer = 0;
  } else {
    // Start hoch, runter stoppen
    relayState[2] = 1;
    relayState[3] = 0;
    pcaRel1.digitalWrite(2, LOW);   // Hoch EIN
    pcaRel1.digitalWrite(3, HIGH);  // Runter AUS
    Serial.println("Türrollo START hoch");
    tuerrolloTimer = millis();
  }
}

void toggleTuerrolloDown() {
  // R03 (idx 3): Türrollo runter - START/STOPP Logik
  // TouchBoard1: case 11: push button for Türrollo down
  if (relayState[3] == 1) {
    // Bereits runter aktiv → STOPP
    relayState[3] = 0;
    pcaRel1.digitalWrite(3, HIGH);  // AUS
    Serial.println("Türrollo STOPP (war runter)");
    tuerrolloTimer = 0;
  } else {
    // Start runter, hoch stoppen
    relayState[2] = 0;
    relayState[3] = 1;
    pcaRel1.digitalWrite(2, HIGH);  // Hoch AUS
    pcaRel1.digitalWrite(3, LOW);   // Runter EIN
    Serial.println("Türrollo START runter");
    tuerrolloTimer = millis();
  }
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
// PWM LED Dimmer Funktionen
// ======================================================
void setLEDTreppeBrightness(uint8_t brightness) {
  ledTreppeBrightness = brightness;
  ledcWrite(LED_TREPPE_PIN, brightness);
  Serial.println("LED Treppe Helligkeit: " + String(brightness) + " (" + String(brightness * 100 / 255) + "%)");
}

void handleLEDDimmer() {
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    if (brightness >= 0 && brightness <= 100) {
      // Umrechnung von Prozent (0-100) zu PWM-Wert (0-255)
      uint8_t pwmValue = (brightness * 255) / 100;
      setLEDTreppeBrightness(pwmValue);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// ======================================================
// Kronleuchter AC Dimmer Funktionen
// ======================================================
void setKronleuchterBrightness(uint8_t brightness) {
  kronleuchterBrightness = brightness;
  ledcWrite(KRONLEUCHTER_DIMMER_PIN, brightness);
  Serial.println("Kronleuchter Helligkeit: " + String(brightness) + " (" + String(brightness * 100 / 255) + "%)");
}

void handleKronleuchterDimmer() {
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    if (brightness >= 0 && brightness <= 100) {
      // Umrechnung von Prozent (0-100) zu PWM-Wert (0-255)
      uint8_t pwmValue = (brightness * 255) / 100;
      setKronleuchterBrightness(pwmValue);
      
      // Auch das Relais entsprechend schalten
      if (brightness > 0) {
        relayState[11] = 1; // R11 = Kronleuchter EIN
        pcaRel2.digitalWrite(3, LOW); // Board B, Pin 3 (R11 = 8+3)
      } else {
        relayState[11] = 0; // R11 = Kronleuchter AUS
        pcaRel2.digitalWrite(3, HIGH); // Board B, Pin 3 (R11 = 8+3)
      }
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void toggleKronleuchter() {
  // Kronleuchter toggle: AUS → 50% → AUS
  if (kronleuchterBrightness == 0) {
    setKronleuchterBrightness(127); // 50% Helligkeit
    relayState[11] = 1; // R11 EIN
    pcaRel2.digitalWrite(3, LOW); // Board B, Pin 3 (R11 = 8+3)
  } else {
    setKronleuchterBrightness(0); // AUS
    relayState[11] = 0; // R11 AUS
    pcaRel2.digitalWrite(3, HIGH); // Board B, Pin 3 (R11 = 8+3)
  }
}

void dimKronleuchter(bool dimUp) {
  const uint8_t dimStep = 15; // Dimm-Schritte
  
  if (dimUp) {
    // Heller dimmen
    if (kronleuchterBrightness < 240) {
      setKronleuchterBrightness(kronleuchterBrightness + dimStep);
      if (kronleuchterBrightness > 0) {
        relayState[11] = 1; // R11 EIN wenn > 0
        pcaRel2.digitalWrite(3, LOW); // Board B, Pin 3 (R11 = 8+3)
      }
    }
  } else {
    // Dunkler dimmen
    if (kronleuchterBrightness > dimStep) {
      setKronleuchterBrightness(kronleuchterBrightness - dimStep);
    } else {
      setKronleuchterBrightness(0); // Komplett aus
      relayState[11] = 0; // R11 AUS
      pcaRel2.digitalWrite(3, HIGH); // Board B, Pin 3 (R11 = 8+3)
    }
  }
}

// ======================================================
// 1-Wire Temperatursensor Funktionen
// ======================================================
void initTemperatureSensors() {
  temperaturSensoren.begin();
  anzahlSensoren = temperaturSensoren.getDeviceCount();
  Serial.println("1-Wire Temperatursensoren initialisiert");
  Serial.println("Anzahl gefundener Sensoren: " + String(anzahlSensoren));
  
  // Auflösung für alle Sensoren setzen
  for (uint8_t i = 0; i < anzahlSensoren && i < 5; i++) {
    DeviceAddress tempDeviceAddress;
    if (temperaturSensoren.getAddress(tempDeviceAddress, i)) {
      temperaturSensoren.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    }
    sensorTemperaturen[i] = -999.0;
    lastSensorTemperaturen[i] = -999.0;
  }
}

void updateTemperatures() {
  if (anzahlSensoren == 0) return;
  
  temperaturSensoren.requestTemperatures();
  delay(100); // Kurz warten für Messung
  
  for (uint8_t i = 0; i < anzahlSensoren && i < 5; i++) {
    float temp = temperaturSensoren.getTempCByIndex(i);
    
    if (temp != DEVICE_DISCONNECTED_C) {
      // Nur bei Änderung um mindestens 0.1°C aktualisieren
      if (abs(temp - lastSensorTemperaturen[i]) >= 0.1) {
        lastSensorTemperaturen[i] = sensorTemperaturen[i];
        sensorTemperaturen[i] = temp;
        Serial.println("Sensor " + String(i) + ": " + String(temp, 1) + "°C");
      }
    } else {
      sensorTemperaturen[i] = -999.0; // Fehlercode für nicht angeschlossen
    }
  }
}

String getTemperatureHTML() {
  if (anzahlSensoren == 0) {
    return "<h3>🌡️ Temperatursensoren</h3><p style='color:orange;'>Keine 1-Wire Sensoren gefunden (GPIO12)</p>";
  }
  
  String html = "<h3>🌡️ Temperatursensoren (" + String(anzahlSensoren) + " gefunden)</h3>";
  html += "<table style='margin-bottom:20px;'><tr>";
  html += "<th>Sensor</th><th>Temperatur</th><th>Status</th></tr>";
  
  for (uint8_t i = 0; i < anzahlSensoren && i < 5; i++) {
    html += "<tr>";
    html += "<td>DS18B20 #" + String(i + 1) + "</td>";
    
    if (sensorTemperaturen[i] != -999.0) {
      String tempColor = "black";
      if (sensorTemperaturen[i] < 10) tempColor = "blue";
      else if (sensorTemperaturen[i] > 25) tempColor = "red";
      
      html += "<td style='color:" + tempColor + ";font-weight:bold;'>" + String(sensorTemperaturen[i], 1) + " °C</td>";
      html += "<td style='color:green;'>✓ OK</td>";
    } else {
      html += "<td style='color:gray;'>-- °C</td>";
      html += "<td style='color:red;'>✗ Fehler</td>";
    }
    html += "</tr>";
  }
  html += "</table>";
  html += "<p style='font-size:12px;color:gray;'>🔄 Update alle 60s | GPIO12 mit 4.7kΩ Pull-up</p>";
  
  return html;
}

// ======================================================
// IR-Switch Küche Funktionen (Kabel EG11)
// ======================================================
void handleIRSwitchKitchen() {
  // Aktuelle Zustände der beiden Taster lesen
  uint8_t currentLeftSwitch = inputState[IR_SWITCH_KITCHEN_LEFT];   // PCF8574 #1 Pin 0
  uint8_t currentRightSwitch = inputState[IR_SWITCH_KITCHEN_RIGHT]; // PCF8574 #1 Pin 1
  
  // Linker Taster: Flanke LOW → HIGH (Taster gedrückt)
  if (currentLeftSwitch == HIGH && lastIRSwitchLeft == LOW) {
    Serial.println("IR-Switch Küche LINKS gedrückt → Toggle Küchenarbeitslampe");
    toggleKuechenarbeitslampe(); // R07 (idx 7) Toggle
    delay(50); // Entprellen
  }
  
  // Rechter Taster: Flanke LOW → HIGH (Taster gedrückt)  
  if (currentRightSwitch == HIGH && lastIRSwitchRight == LOW) {
    Serial.println("IR-Switch Küche RECHTS gedrückt → Toggle Küchenarbeitslampe");
    toggleKuechenarbeitslampe(); // R07 (idx 7) Toggle
    delay(50); // Entprellen
  }
  
  // Zustände für nächste Iteration speichern
  lastIRSwitchLeft = currentLeftSwitch;
  lastIRSwitchRight = currentRightSwitch;
}

// ======================================================
// Kreuzschaltung EG Funktionen (2 Schalter)
// ======================================================
void handleKreuzschaltungEG() {
  // Aktuelle Zustände der beiden EG Schalter lesen (Bit-kombiniert)
  // Bit 0 = KREUZ_EG1, Bit 1 = KREUZ_EG2
  uint8_t currentStateEG = inputState[KREUZ_EG1] | (inputState[KREUZ_EG2] << 1);
  
  // Wenn sich der Schalter-Zustand geändert hat → Lampe togglen
  if (kreuzstateEG != currentStateEG) {
    Serial.println("EG Kreuzschaltung geändert → Toggle EG Flurlampe (R09)");
    toggleEGFlurlampe(); // R09 (idx 9) Toggle
    
    // Debug-Ausgabe der Schalterstellung
    Serial.printf("EG Schalter: EG1=%d, EG2=%d → Zustand: 0x%02X\n", 
                  inputState[KREUZ_EG1], inputState[KREUZ_EG2], currentStateEG);
  }
  
  // Aktuellen Zustand für nächste Iteration speichern
  kreuzstateEG = currentStateEG;
}

// ======================================================
// Kreuzschaltung KG Funktionen (3 Schalter)
// ======================================================
void handleKreuzschaltungKG() {
  // Aktuelle Zustände der drei KG Schalter lesen (Bit-kombiniert)
  // Bit 0 = KREUZ_KG1, Bit 1 = KREUZ_KG2, Bit 2 = KREUZ_KG3
  uint8_t currentStateKG = inputState[KREUZ_KG1] | 
                          (inputState[KREUZ_KG2] << 1) | 
                          (inputState[KREUZ_KG3] << 2);
  
  // Wenn sich der Schalter-Zustand geändert hat → Lampe togglen
  if (kreuzstateKG != currentStateKG) {
    Serial.println("KG Kreuzschaltung geändert → Toggle KG Flurlampe (R06)");
    toggleKGFlurlampe(); // R06 (idx 6) Toggle
    
    // Debug-Ausgabe der Schalterstellung
    Serial.printf("KG Schalter: KG1=%d, KG2=%d, KG3=%d → Zustand: 0x%02X\n", 
                  inputState[KREUZ_KG1], inputState[KREUZ_KG2], inputState[KREUZ_KG3], currentStateKG);
  }
  
  // Aktuellen Zustand für nächste Iteration speichern
  kreuzstateKG = currentStateKG;
}


