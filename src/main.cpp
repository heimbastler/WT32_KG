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
// 📡 GPIO32  → I2C SCL (MCP23017, PCA9535, MPR121)
// 📡 GPIO33  → I2C SDA (MCP23017, PCA9535, MPR121)
// 💡 GPIO17  → OnBoard Status LED (aktiv HIGH)

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                              I2C GERÄTE                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// MCP23017 Input Expander:  0x20    (16 digitale Ein-/Ausgänge für Schalter)
//   GPA0-GPA7: IR-Switch Küche, Kreuzschaltungen, Reserve
//   GPB0-GPB7: Reserve Eingänge (8 weitere Pins)
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
#include <WiFi.h>  // WiFi für ESP-NOW benötigt (nicht für STA/AP)
#include <ArduinoOTA.h>
#include <Adafruit_MCP23X17.h> // 16-Bit GPIO Expander für Eingänge
#include <Adafruit_PCA9535.h>
#include "Adafruit_MPR121.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "espnow_gateway.h"  // ESP-NOW Gateway

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
#define ONE_WIRE_BUS 12      // GPIO12 für DS18B20 Temperatursensor (Schaltschrank)
#define TEMPERATURE_PRECISION 10  // 10-bit = 0.25°C Auflösung

// ---------- MCP23017 Adresse für Switches/Eingänge ----------
Adafruit_MCP23X17 mcpIn;  // Adresse 0x20 (Standard) - 16 digitale Ein-/Ausgänge

// ---------- IR-Switch Küche Kabel EG11 ----------
// Kabel EG11: br/ws = +5V, br = GND, grn = links, grn/ws = rechts
#define IR_SWITCH_KITCHEN_LEFT  0   // MCP23017 GPA0 → grn (linker Taster)
#define IR_SWITCH_KITCHEN_RIGHT 1   // MCP23017 GPA1 → grn/ws (rechter Taster)
// Beide Taster gegen br = GND, Pull-up über MCP23017

// ---------- Kreuzlichtschaltungen ----------
// EG Kreuzschaltung (2 Schalter für EG Flurlampe R09)
#define KREUZ_EG1 2              // MCP23017 GPA2 → Kabel EG10 (unterer Schalter Treppe OG)
#define KREUZ_EG2 3              // MCP23017 GPA3 → Kabel EG1 (oberer Schalter Eingang EG)
// KREUZ_EG3 nicht im Einsatz (war Pin 53 im alten System)

// KG Kreuzschaltung (3 Schalter für KG Flurlampe R06)  
#define KREUZ_KG1 4              // MCP23017 GPA4 → Kabel KG1 (Schalter Tür Schlafzimmer, gelb)
#define KREUZ_KG2 5              // MCP23017 GPA5 → Kabel KG1 (oberer Schalter Bad KG, weiss)
#define KREUZ_KG3 6              // MCP23017 GPA6 → Kabel EG1 (Schalter Treppe zum KG im EG, braun)

// ---------- PCA9535 Adressen für Relais ----------
// Base-Adresse: 0x20, finale Adresse = 0x20 + A2×4 + A1×2 + A0×1
// KONFLIKT VERMIEDEN: MCP23017 nutzt Adresse 0x20 → PCA9535 ab 0x22
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
uint8_t relayState[24];   // 3 PCA9535 Expander à 8 Ausgänge
uint8_t inputState[16];   // 1 MCP23017 Expander mit 16 Ein-/Ausgängen (8 verwandt)
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

// ---------- Temperatursensor Zustand (1x DS18B20 für Schaltschrank) ----------
float schaltschrankTemp = -999.0;       // Schaltschrank Temperatur (°C)
float lastSchaltschrankTemp = -999.0;   // Letzte Temperatur für Änderungserkennung

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
void handleHome();
void handleESPNow();
void handleToggle();
void handleLEDDimmer();
void handleKronleuchterDimmer();
void handlePairing();
void handleClientDetail();
void handleRemoveClient();



// --- Zeitsteuerung für Rollos ---
unsigned long fensterrolloTimer = 0;
unsigned long tuerrolloTimer = 0;
const unsigned long rolloActiveTime = 60000; // ms, wie lange das Relais anzieht (1 Minute)

// --- Zeitsteuerung für Temperaturmessung ---
unsigned long lastTempUpdate = 0;
const unsigned long tempUpdateInterval = 60000; // 1 Minute

// --- Zeitsteuerung für I2C Lese-Operationen (Throttling) ---
unsigned long lastI2CRead = 0;
const unsigned long i2cReadInterval = 100; // Eingänge nur alle 100ms lesen

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  Serial.println("=== WT32-KG Smart Home Controller ===");
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(500);

  // ===== I2C SCANNER =====
  Serial.println("\n=== I2C Bus Scanner ===");
  Serial.println("Scanning for I2C devices...");
  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("✅ I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" (");
      Serial.print(address);
      Serial.println(")");
      count++;
    }
  }
  if (count == 0) {
    Serial.println("❌ No I2C devices found!");
  } else {
    Serial.print("✅ Found ");
    Serial.print(count);
    Serial.println(" I2C device(s)");
  }
  Serial.println("===================\n");

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
      pcaRel1.digitalWrite(i, LOW);
      pcaRel2.digitalWrite(i, LOW);  
      pcaRel3.digitalWrite(i, LOW);
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

  // MCP23017 Input Expander - DEAKTIVIERT (nicht angeschlossen)
  /*
  if (!mcpIn.begin_I2C(0x20)) {
    Serial.println("ERROR: MCP23017 nicht gefunden auf Adresse 0x20!");
  } else {
    Serial.println("MCP23017 initialisiert auf Adresse 0x20");
  }
  */
  Serial.println("MCP23017: Übersprungen (nicht angeschlossen)");

  // PCA9535 Relais starten
  Serial.println("\n=== PCA9535 Relais-Boards Initialisierung ===");
  bool pca1_ok = pcaRel1.begin(0x22);
  bool pca2_ok = pcaRel2.begin(0x23);
  bool pca3_ok = pcaRel3.begin(0x24);
  
  if (pca1_ok) {
    Serial.println("✅ PCA9535 Board 1 (0x22) initialisiert");
  } else {
    Serial.println("❌ PCA9535 Board 1 (0x22) NICHT gefunden!");
  }
  
  if (pca2_ok) {
    Serial.println("✅ PCA9535 Board 2 (0x23) initialisiert");
  } else {
    Serial.println("❌ PCA9535 Board 2 (0x23) NICHT gefunden!");
  }
  
  if (pca3_ok) {
    Serial.println("✅ PCA9535 Board 3 (0x24) initialisiert");
  } else {
    Serial.println("❌ PCA9535 Board 3 (0x24) NICHT gefunden!");
  }
  Serial.println("==========================================\n");

  // Switches als INPUT - DEAKTIVIERT
  /*
  for (int i = 0; i < 8; i++) {
    mcpIn.pinMode(i, INPUT);
  }
  // MCP23017 Port B: GPB0-GPB7 als Reserve Eingänge
  for (int i = 8; i < 16; i++) {
    mcpIn.pinMode(i, INPUT);
  }
  */
  Serial.println("MCP23017 Eingänge: Übersprungen");

  // Relais als OUTPUT und alle AUS (LOW für nicht-invertierte Relais)
  for (int i = 0; i < 8; i++) {
    pcaRel1.pinMode(i, OUTPUT);
    pcaRel1.digitalWrite(i, LOW);
    pcaRel2.pinMode(i, OUTPUT);
    pcaRel2.digitalWrite(i, LOW);
    pcaRel3.pinMode(i, OUTPUT);
    pcaRel3.digitalWrite(i, LOW);
  }
  for (int i = 0; i < 24; i++) {
    relayState[i] = 0;
  }

  // PWM für LED Dimmer konfigurieren
  ledcSetup(PWM_CHANNEL_TREPPE, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_TREPPE_PIN, PWM_CHANNEL_TREPPE);
  setLEDTreppeBrightness(0); // Starten mit LEDs aus
  
  // PWM für Kronleuchter AC Dimmer konfigurieren
  ledcSetup(PWM_CHANNEL_KRONLEUCHTER, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(KRONLEUCHTER_DIMMER_PIN, PWM_CHANNEL_KRONLEUCHTER);
  setKronleuchterBrightness(0); // Starten mit Kronleuchter aus

  // TouchBoards - DEAKTIVIERT (nicht angeschlossen)
  // initTouchBoards();
  Serial.println("TouchBoards: Übersprungen (nicht angeschlossen)");

  // Temperatursensoren - DEAKTIVIERT
  // initTemperatureSensors();
  Serial.println("DS18B20 Sensoren: Übersprungen");

  // ESP-NOW Gateway initialisieren
  initESPNowGateway();

  // Webserver konfigurieren
  server.on("/", handleRoot);
  server.on("/home", handleHome);
  server.on("/espnow", handleESPNow);
  server.on("/toggle", handleToggle);
  server.on("/led", handleLEDDimmer);
  server.on("/kronleuchter", handleKronleuchterDimmer);
  server.on("/pairing", handlePairing);
  server.on("/client", handleClientDetail);
  server.on("/remove_client", handleRemoveClient);
  server.begin();
  Serial.println("Webserver gestartet");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  ArduinoOTA.handle();  // OTA Updates verarbeiten
  server.handleClient();
  
  // ESP-NOW Client-Timeouts prüfen (alle 10 Sekunden)
  static unsigned long lastTimeoutCheck = 0;
  if (millis() - lastTimeoutCheck > 10000) {
    checkClientTimeouts();
    lastTimeoutCheck = millis();
  }
  
  // === NUR RELAIS-STEUERUNG AKTIV ===
  // MCP23017, MPR121 und DS18B20 sind deaktiviert
  
  /* DEAKTIVIERT - Geräte nicht angeschlossen
  // Eingänge und Touch-Events nur alle 100ms lesen (I2C-Throttling)
  if (millis() - lastI2CRead >= i2cReadInterval) {
    lastI2CRead = millis();
    
    // Eingänge lesen (über MCP23017 Port A und B)
    for (int i = 0; i < 8; i++) {
      inputState[i] = mcpIn.digitalRead(i);      // Port A: GPA0-GPA7
      inputState[i + 8] = mcpIn.digitalRead(i + 8);  // Port B: GPB0-GPB7
    }

    // Touch Events verarbeiten
    handleTouchEvents();

    // IR-Switch Küche überwachen
    handleIRSwitchKitchen();

    // Kreuzschaltungen überwachen
    handleKreuzschaltungEG();
    handleKreuzschaltungKG();
  }

  // Temperaturen alle 1 Minute aktualisieren
  if (millis() - lastTempUpdate > tempUpdateInterval) {
    updateTemperatures();
    lastTempUpdate = millis();
  }
  */

  // --- Sicherheits-Timeout für Rollos (5 Minuten) ---
  // Verhindert dass Rollos bei Fehler endlos laufen
  if (fensterrolloTimer > 0 && millis() - fensterrolloTimer > (rolloActiveTime * 5)) {
    Serial.println("SICHERHEITS-STOPP Fensterrollo nach 5 Minuten");
    relayState[0] = 0; relayState[1] = 0;
    pcaRel1.digitalWrite(0, LOW);
    pcaRel1.digitalWrite(1, LOW);
    fensterrolloTimer = 0;
  }
  if (tuerrolloTimer > 0 && millis() - tuerrolloTimer > (rolloActiveTime * 5)) {
    Serial.println("SICHERHEITS-STOPP Türrollo nach 5 Minuten");
    relayState[2] = 0; relayState[3] = 0;
    pcaRel1.digitalWrite(2, LOW);
    pcaRel1.digitalWrite(3, LOW);
    tuerrolloTimer = 0;
  }

  delay(50);
}

// ======================================================
// Webserver Handler
// ======================================================
void handleRoot() {
  // Standardmäßig zur Home-Seite weiterleiten
  server.sendHeader("Location", "/home");
  server.send(303);
}

// ===== Gemeinsame HTML-Header-Funktion =====
String getHTMLHeader(String activeTab) {
  String html = "<html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<title>WT32-KG Controller</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;margin:0;padding:0;background:#f5f5f5;font-size:14px;}";
  html += ".container{max-width:600px;margin:0 auto;background:white;border-radius:8px;box-shadow:0 2px 5px rgba(0,0,0,0.1);overflow:hidden;}";
  html += "h2{color:#333;text-align:center;padding:15px 10px 10px 10px;font-size:18px;margin:0;background:linear-gradient(135deg, #667eea 0%, #764ba2 100%);color:white;}";
  html += "h3{color:#555;margin:15px 10px 8px 10px;border-left:3px solid #2196F3;padding-left:10px;font-size:16px;}";
  
  // Tab-Navigation
  html += ".tabs{display:flex;background:#e0e0e0;margin:0;padding:0;list-style:none;border-bottom:2px solid #4CAF50;}";
  html += ".tab{flex:1;text-align:center;padding:12px 5px;cursor:pointer;background:#e0e0e0;border:none;font-size:13px;font-weight:bold;color:#555;text-decoration:none;display:block;transition:all 0.3s;}";
  html += ".tab:hover{background:#d0d0d0;}";
  html += ".tab.active{background:#4CAF50;color:white;border-bottom:3px solid #2e7d32;}";
  
  // Content Area
  html += ".content{padding:10px;}";
  
  // Button Styles
  html += ".btn{padding:8px 12px;margin:3px;border:none;border-radius:6px;cursor:pointer;font-size:13px;font-weight:bold;transition:all 0.2s;min-width:120px;display:inline-block;text-align:center;text-decoration:none;}";
  html += ".btn:active{transform:scale(0.95);}";
  html += ".btn-on{background:#4CAF50;color:white;}";
  html += ".btn-off{background:#f44336;color:white;}";
  html += ".btn-rollo{background:#FF9800;color:white;min-width:70px;margin:2px;padding:8px 10px;}";
  html += ".btn-neutral{background:#607D8B;color:white;}";
  
  // Layout Styles
  html += ".grid{display:grid;grid-template-columns:1fr;gap:10px;margin:10px 0;}";
  html += ".card{background:#fafafa;padding:10px;border-radius:6px;border:1px solid #ddd;margin:10px 0;}";
  html += ".rollo-group{display:flex;flex-wrap:wrap;gap:3px;align-items:center;margin:8px 0;}";
  html += ".rollo-label{min-width:100px;font-weight:bold;font-size:13px;}";
  
  // Table Styles
  html += "table{border-collapse:collapse;width:100%;margin:10px 0;font-size:12px;}";
  html += "td,th{border:1px solid #ddd;padding:5px;text-align:center;}";
  html += "th{background:#f0f0f0;font-weight:bold;}";
  html += "</style></head><body>";
  
  html += "<div class='container'>";
  html += "<h2>🏠 WT32-KG Smart Home</h2>";
  
  // Tab-Navigation
  html += "<div class='tabs'>";
  html += "<a href='/home' class='tab" + String(activeTab == "home" ? " active" : "") + "'>🏠 Home</a>";
  html += "<a href='/espnow' class='tab" + String(activeTab == "espnow" ? " active" : "") + "'>📡 ESP-NOW</a>";
  html += "</div>";
  
  html += "<div class='content'>";
  return html;
}

String getHTMLFooter() {
  return "</div></div></body></html>";
}

// ===== Home-Seite =====
void handleHome() {
  String html = getHTMLHeader("home");
  
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
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== ESP-NOW Seite =====
void handleESPNow() {
  String html = getHTMLHeader("espnow");
  
  html += "<h3>📡 ESP-NOW Gateway</h3>";
  html += "<div style='margin:10px 0;'>";
  html += "<a href='/pairing' class='btn " + String(espnowPairingMode ? "btn-on" : "btn-neutral") + "' style='min-width:150px;'>";
  html += espnowPairingMode ? "🔗 Pairing AKTIV" : "🔗 Pairing aktivieren";
  html += "</a>";
  html += "</div>";
  
  if (espnowClientCount == 0) {
    html += "<div class='card'>";
    html += "<p style='text-align:center;color:#888;'>Keine Clients gepairt.</p>";
    html += "<p style='text-align:center;font-size:12px;'>Aktivieren Sie den Pairing-Modus und starten Sie einen ESP-NOW Client zum Pairen.</p>";
    html += "</div>";
  } else {
    html += "<table><tr><th>Name</th><th>Typ</th><th>Status</th><th>Aktion</th></tr>";
    for (int i = 0; i < espnowClientCount; i++) {
      ESPNowClient *c = &espnowClients[i];
      html += "<tr><td>" + String(c->name) + "</td>";
      html += "<td>";
      switch (c->type) {
        case CLIENT_TYPE_SENSOR: html += "📊 Sensor"; break;
        case CLIENT_TYPE_SWITCH: html += "🔘 Schalter"; break;
        case CLIENT_TYPE_RELAY: html += "⚡ Relais"; break;
        case CLIENT_TYPE_DIMMER: html += "💡 Dimmer"; break;
        default: html += "❓ Custom"; break;
      }
      html += "</td><td>";
      html += c->online ? "🟢 Online" : "🔴 Offline";
      html += "</td><td>";
      html += "<a href='/client?id=" + String(i) + "' class='btn btn-neutral' style='min-width:70px;font-size:11px;padding:5px 8px;'>Details</a>";
      html += "</td></tr>";
    }
    html += "</table>";
    
    // Info-Box
    html += "<div class='card' style='background:#e3f2fd;border-color:#2196F3;'>";
    html += "<p style='margin:5px 0;font-size:12px;'><b>ℹ️ Gateway Status:</b></p>";
    html += "<p style='margin:5px 0;font-size:12px;'>• Gepairt: " + String(espnowClientCount) + " Client(s)</p>";
    html += "<p style='margin:5px 0;font-size:12px;'>• Pairing-Modus: " + String(espnowPairingMode ? "Aktiv" : "Inaktiv") + "</p>";
    html += "<p style='margin:5px 0;font-size:12px;'>• Gateway MAC: " + WiFi.macAddress() + "</p>";
    html += "</div>";
  }
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== ESP-NOW Handler =====
void handlePairing() {
  // Pairing-Modus umschalten
  espnowPairingMode = !espnowPairingMode;
  enablePairingMode(espnowPairingMode);
  
  String html = "<html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='2; url=/espnow'>";  // Redirect zu ESP-NOW Seite
  html += "<title>Pairing</title>";
  html += "<style>body{font-family:Arial;text-align:center;padding:50px;background:#f5f5f5;}</style>";
  html += "</head><body>";
  html += "<div style='background:white;padding:30px;border-radius:8px;display:inline-block;box-shadow:0 2px 5px rgba(0,0,0,0.1);'>";
  html += "<h2 style='margin-top:0;'>" + String(espnowPairingMode ? "🔗 Pairing-Modus AKTIV" : "❌ Pairing-Modus INAKTIV") + "</h2>";
  if (espnowPairingMode) {
    html += "<p>Starten Sie jetzt Ihren ESP-NOW Client zum Pairen.</p>";
    html += "<p style='color:#4CAF50;font-weight:bold;'>Pairing-Fenster: 60 Sekunden</p>";
  } else {
    html += "<p>Pairing-Modus wurde deaktiviert.</p>";
  }
  html += "<p style='color:#888;font-size:12px;'>Sie werden in 2 Sekunden weitergeleitet...</p>";
  html += "</div></body></html>";
  server.send(200, "text/html; charset=UTF-8", html);
}

void handleClientDetail() {
  int clientId = server.arg("id").toInt();
  
  String html = getHTMLHeader("espnow");
  html += getClientDetailHTML(clientId);
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

void handleRemoveClient() {
  int clientId = server.arg("id").toInt();
  
  if (clientId >= 0 && clientId < espnowClientCount) {
    removeESPNowClient(espnowClients[clientId].mac);
  }
  
  // Redirect zurück zur ESP-NOW Seite
  server.sendHeader("Location", "/espnow");
  server.send(303);
}

void handleToggle() {
  int idx = server.arg("r").toInt();
  Serial.print("🔧 handleToggle() called - Index: ");
  Serial.println(idx);
  
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
    // Standard Toggle für alle anderen Relais (nicht-invertiert: LOW=AUS, HIGH=EIN)
    relayState[idx] = !relayState[idx];
    int chip = idx / 8;
    int pin = idx % 8;
    
    Serial.print("  Chip: ");
    Serial.print(chip);
    Serial.print(" (0x");
    Serial.print(0x22 + chip, HEX);
    Serial.print("), Pin: ");
    Serial.print(pin);
    Serial.print(", New State: ");
    Serial.println(relayState[idx] ? "ON (HIGH)" : "OFF (LOW)");
    
    if (chip == 0) {
      pcaRel1.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
      Serial.println("  -> Written to pcaRel1");
    }
    if (chip == 1) {
      pcaRel2.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
      Serial.println("  -> Written to pcaRel2");
    }
    if (chip == 2) {
      pcaRel3.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
      Serial.println("  -> Written to pcaRel3");
    }
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
    pcaRel1.digitalWrite(0, LOW);  // AUS
    Serial.println("Fensterrollo STOPP (war hoch)");
    fensterrolloTimer = 0;
  } else {
    // Start hoch, runter stoppen
    relayState[0] = 1;
    relayState[1] = 0;
    pcaRel1.digitalWrite(0, HIGH);   // Hoch EIN
    pcaRel1.digitalWrite(1, LOW);  // Runter AUS
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
    pcaRel1.digitalWrite(1, LOW);  // AUS
    Serial.println("Fensterrollo STOPP (war runter)");
    fensterrolloTimer = 0;
  } else {
    // Start runter, hoch stoppen
    relayState[0] = 0;
    relayState[1] = 1;
    pcaRel1.digitalWrite(0, LOW);  // Hoch AUS
    pcaRel1.digitalWrite(1, HIGH);   // Runter EIN
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
    pcaRel1.digitalWrite(2, LOW);  // AUS
    Serial.println("Türrollo STOPP (war hoch)");
    tuerrolloTimer = 0;
  } else {
    // Start hoch, runter stoppen
    relayState[2] = 1;
    relayState[3] = 0;
    pcaRel1.digitalWrite(2, HIGH);   // Hoch EIN
    pcaRel1.digitalWrite(3, LOW);  // Runter AUS
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
    pcaRel1.digitalWrite(3, LOW);  // AUS
    Serial.println("Türrollo STOPP (war runter)");
    tuerrolloTimer = 0;
  } else {
    // Start runter, hoch stoppen
    relayState[2] = 0;
    relayState[3] = 1;
    pcaRel1.digitalWrite(2, LOW);  // Hoch AUS
    pcaRel1.digitalWrite(3, HIGH);   // Runter EIN
    Serial.println("Türrollo START runter");
    tuerrolloTimer = millis();
  }
}
void toggleAussenlampeGarten() {
  // R04 (idx 4)
  // TouchBoard1: case 0: unten 2te von links
  int idx = 4;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
}
void toggleSteinlampe() {
  // R05 (idx 5)
  // TouchBoard2: case 6: 2te links (auskommentiert in old)
  int idx = 5;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
}
void toggleKGFlurlampe() {
  // R06 (idx 6)
  // Kein direkter Touch, Schalter/EG/KG
  int idx = 6;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
}
void toggleKuechenarbeitslampe() {
  // R07 (idx 7)
  // TouchBoard2: case 3: unten links
  int idx = 7;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(0, relayState[idx] ? HIGH : LOW);
}
void toggleKuechenlampe() {
  // R08 (idx 8)
  // TouchBoard2: case 2: 3te links
  int idx = 8;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(1, relayState[idx] ? HIGH : LOW);
}
void toggleEGFlurlampe() {
  // R09 (idx 9)
  // TouchBoard2: case 0: 3te rechts
  int idx = 9;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(2, relayState[idx] ? HIGH : LOW);
}
void toggleTraegerlampen() {
  // R10 (idx 10)
  // TouchBoard2: case 1: unten rechts
  int idx = 10;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(3, relayState[idx] ? HIGH : LOW);
}
void toggleWohnzimmerlampe1() {
  // R11 (idx 11)
  // TouchBoard1: case 2: oben 1te von links
  int idx = 11;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(4, relayState[idx] ? HIGH : LOW);
}
void toggleWohnzimmerlampe2() {
  // R12 (idx 12)
  // Kein direkter Touch, nur Gruppe
  int idx = 12;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(5, relayState[idx] ? HIGH : LOW);
}
void toggleLamps() {
  // Gruppe: R05, R07, R08, R09, R10, R11, R12 (idx 5,7,8,9,10,11,12)
  // Touch Zuordnung siehe readTouchInputs2/readTouchInputs3
  for (int idx : {5,7,8,9,10,11,12}) {
    relayState[idx] = !relayState[idx];
    int chip = idx / 8;
    int pin = idx % 8;
    if (chip == 0) pcaRel1.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
    if (chip == 1) pcaRel2.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
    if (chip == 2) pcaRel3.digitalWrite(pin, relayState[idx] ? HIGH : LOW);
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
// 1-Wire Temperatursensor Funktionen (1x DS18B20 für Schaltschrank)
// ======================================================
void initTemperatureSensors() {
  temperaturSensoren.begin();
  uint8_t sensorCount = temperaturSensoren.getDeviceCount();
  
  Serial.println("1-Wire Temperatursensor initialisiert");
  Serial.println("Sensoren gefunden: " + String(sensorCount));
  
  if (sensorCount > 0) {
    DeviceAddress tempDeviceAddress;
    if (temperaturSensoren.getAddress(tempDeviceAddress, 0)) {
      temperaturSensoren.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      Serial.println("DS18B20 Sensor konfiguriert (Schaltschrank)");
    }
  } else {
    Serial.println("WARNUNG: Kein DS18B20 Sensor gefunden!");
  }
  
  schaltschrankTemp = -999.0;
  lastSchaltschrankTemp = -999.0;
}

void updateTemperatures() {
  temperaturSensoren.requestTemperatures();
  delay(100); // Kurz warten für Messung
  
  float temp = temperaturSensoren.getTempCByIndex(0);
  
  if (temp != DEVICE_DISCONNECTED_C) {
    // Nur bei Änderung um mindestens 0.1°C aktualisieren
    if (abs(temp - lastSchaltschrankTemp) >= 0.1) {
      lastSchaltschrankTemp = schaltschrankTemp;
      schaltschrankTemp = temp;
      Serial.println("Schaltschrank Temperatur: " + String(temp, 1) + "°C");
    }
  } else {
    schaltschrankTemp = -999.0; // Fehlercode für nicht angeschlossen
    Serial.println("FEHLER: DS18B20 Sensor nicht erreichbar!");
  }
}

String getTemperatureHTML() {
  String html = "<h3>🌡️ Schaltschrank Temperatur</h3>";
  html += "<table style='margin-bottom:20px;'><tr>";
  html += "<th>Komponente</th><th>Wert</th><th>Status</th></tr>";
  
  html += "<tr>";
  html += "<td>DS18B20 Sensor (GPIO12)</td>";
  
  if (schaltschrankTemp != -999.0) {
    String tempColor = "black";
    if (schaltschrankTemp < 10) tempColor = "blue";
    else if (schaltschrankTemp > 35) tempColor = "red";
    else if (schaltschrankTemp > 30) tempColor = "orange";
    
    html += "<td style='color:" + tempColor + ";font-weight:bold;'>" + String(schaltschrankTemp, 1) + " °C</td>";
    html += "<td style='color:green;'>✓ OK</td>";
  } else {
    html += "<td style='color:gray;'>-- °C</td>";
    html += "<td style='color:red;'>✗ Fehler</td>";
  }
  html += "</tr>";
  html += "</table>";
  html += "<p style='font-size:12px;color:gray;'>🔄 Kontinuierliche Messung | GPIO12 mit 4.7kΩ Pull-up</p>";
  
  return html;
}

// ======================================================
// IR-Switch Küche Funktionen (Kabel EG11)
// ======================================================
void handleIRSwitchKitchen() {
  // Aktuelle Zustände der beiden Taster lesen
  uint8_t currentLeftSwitch = inputState[IR_SWITCH_KITCHEN_LEFT];   // MCP23017 GPA0
  uint8_t currentRightSwitch = inputState[IR_SWITCH_KITCHEN_RIGHT]; // MCP23017 GPA1
  
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


