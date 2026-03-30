// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    WT32-ETH01 (ESP32-WROOM-32) PINMAP                        ║
// ║                              Version 1.4                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ┌────────────────────────────────────────────────────────────────────────────┐
// │                       AKTUELLE GPIO ZUORDNUNG                              │
// ├────────────┬───────────────┬───────────────────────────────────────────────┤
// │   GPIO     │   Funktion    │               Beschreibung                    │
// ├────────────┼───────────────┼───────────────────────────────────────────────┤
// │ GPIO02  🔸 │ PWM           │ AC Dimmer YYAC-3S (220V) ⚠️ Boot-kritisch!   │
// │ GPIO04  🔸 │ 1-Wire        │ DS18B20 Temp-Sensor + Pull-up                │
// │ GPIO14  🔸 │ PWM           │ LED Dimmer HW-517 V0.0.1 (MOSFET)          │
// │ GPIO17  💡 │ Status LED    │ OnBoard LED (aktiv HIGH)                      │
// │ GPIO32  📡 │ I²C SCL       │ Clock für alle I²C Geräte + 4.7kΩ PU         │
// │ GPIO33  📡 │ I²C SDA       │ Daten für alle I²C Geräte + 4.7kΩ PU         │
// │ GPIO36  📥 │ IRQ Input     │ MPR121 Wired-OR IRQ (3x Touch via L.Shift)   │
// │ GPIO39  📥 │ IRQ Input     │ MCP23017 INTA/INTB (Schalter/Taster)         │
// │ GPIO32  📡 │ I²C SCL       │ Clock für alle I²C Geräte + 4.7kΩ PU         │
// │ GPIO33  📡 │ I²C SDA       │ Daten für alle I²C Geräte + 4.7kΩ PU         │
// ├────────────┼───────────────┼───────────────────────────────────────────────┤
// │ GPIO12  ⚠️ │ RESERVE       │ ⚠️ KEIN Pull-up! Boot fail wenn HIGH!        │
// │ GPIO14  ⚠️ │ MTDO          │ Boot Debug Output (optional verfügbar)      │
// ├────────────┼───────────────┼───────────────────────────────────────────────┤
// │ GPIO18  ❌ │ ETH_MDIO      │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO19  ❌ │ ETH_TXD0      │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO21  ❌ │ ETH_CLK_OUT   │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO22  ❌ │ ETH_RXD0      │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO23  ❌ │ ETH_MDC       │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO25  ❌ │ ETH_TX_EN     │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO26  ❌ │ ETH_RX_ER     │ LAN8720 PHY (reserviert - nicht frei!)       │
// │ GPIO27  ❌ │ ETH_CRS_DV    │ LAN8720 PHY (reserviert - nicht frei!)       │
// ├────────────┼───────────────┼───────────────────────────────────────────────┤
// │ GPIO00  ⚠️ │ BOOT          │ MUSS LOW sein beim Flash! (Boot Mode)        │
// │ GPIO01  ❌ │ TX0/UART0     │ Serial Debug (Flash/Upload - zum PC)         │
// │ GPIO02  ⚠️ │ BOOT          │ Darf kein Pull-up beim Programmieren haben   │
// │ GPIO03  ❌ │ RX0/UART0     │ Serial Debug (Flash/Upload - vom PC)         │
// ├────────────┼───────────────┼───────────────────────────────────────────────┤
// │ GPIO35  📥 │ Input only    │ ADC, kein Pull-up möglich, nur Eingang       │
// │ GPIO36  📥 │ Input only    │ ADC, kein Pull-up möglich, nur Eingang       │
// │ GPIO39  📥 │ Input only    │ ADC, kein Pull-up möglich, nur Eingang       │
// └────────────┴───────────────┴───────────────────────────────────────────────┘

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                              I²C GERÄTE                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// MCP23017 Input Expander:  0x20    (16 digitale Ein-/Ausgänge für Schalter)
//   GPA0-GPA6: IR-Switch Küche, Kreuzschaltungen
//   GPA7: Reserve (früher MPR121 IRQ, jetzt auf GPIO16)
//   GPB0-GPB7: Reserve Eingänge (8 weitere Pins)
//   INTA/INTB: Interrupt verbunden mit GPIO39
//
// PCA9535 Relay Boards:  0x22, 0x23, 0x24 (3x 8 Relais Ausgänge)
//   Board A (0x22): R00-R07
//   Board B (0x23): R08-R15
//   Board C (0x24): R16-R23
//
// MPR121 Touch Sensors:  0x5A, 0x5C, 0x5D  (Capacitive Touch Panels)
//   Panel 1 (0x5A): Tür Garten EG
//   Panel 2 (0x5C): Säule Garten EG
//   Panel 3 (0x5D): Säule Straße EG
//   IRQ: Alle 3 IRQs kombiniert (Wired-OR) → GPIO16 via Level Shifter (5V→3.3V)

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                     KRITISCHE BOOT-PIN-HINWEISE                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// ⚠️ GPIO0:  Muss LOW sein beim Flash-Vorgang (Programmierung)
// ⚠️ GPIO2:  Darf keinen Pull-up beim Programmieren haben
// ⚠️ GPIO12: BOOT FAIL wenn HIGH beim Start! NIEMALS Pull-up verwenden!
// ⚠️ GPIO14: MTDO - Boot Debug Output
//
// ✅ Sichere GPIOs für Pull-ups: GPIO35 (Input-only), GPIO13, GPIO16, GPIO4, GPIO15
//
// 🔸 VERWENDET → Aktuell in diesem Projekt belegt
// ❌ RESERVIERT→ Ethernet LAN8720 oder UART (nicht verwenden!)
// ⚠️ VORSICHT  → Boot-relevant oder spezielle Einschränkungen
// 📡 I²C       → I²C Bus (SCL/SDA)
// 📥 INPUT     → Nur Eingänge möglich (GPIO35, GPIO36, GPIO39)
// 💡 STATUS    → OnBoard LED




#include <Wire.h>
#include <WebServer.h>
#include <ETH.h>
#include <WiFi.h>  // WiFi für ESP-NOW benötigt (nicht für STA/AP)
#include <ArduinoOTA.h>
#include <ESPmDNS.h>  // mDNS für Hostname-Auflösung
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

// ---------- Interrupt Pins ----------
#define MCP23017_IRQ_PIN 39   // GPIO39 für MCP23017 INTA/INTB (Schalter/Taster) - Input-only
#define MPR121_IRQ_PIN 36     // GPIO36 für MPR121 Wired-OR IRQ (3x Touch via Level Shifter) - Input-only

// ---------- PWM Setup für LED & AC Dimmer ----------
#define LED_DIMMER_PIN 14        // GPIO14 für MOSFET LED Dimmer HW-517 V0.0.1
#define AC_DIMMER_PIN 2          // GPIO2 für AC Dimmer YYAC-3S (220V Kronleuchter) ⚠️ Boot-kritisch!
#define PWM_CHANNEL_LED 0        // LEDC Kanal 0 für LED Dimmer
#define PWM_CHANNEL_AC 1         // LEDC Kanal 1 für AC Dimmer
#define PWM_FREQ 5000            // 5kHz Frequenz
#define PWM_RESOLUTION 8         // 8-bit Resolution (0-255)

// ---------- 1-Wire Setup für Temperatursensoren ----------
#define ONE_WIRE_BUS 4       // GPIO4 für DS18B20 Temperatursensor (Schaltschrank)
                             // 📥 GPIO35 = Input-only PIN (kein OUTPUT möglich)
                             // ⚠️ WICHTIG: GPIO12 NICHT verwenden! Boot fail wenn Pull-up HIGH!
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
uint8_t ledDimmerBrightness = 0;         // LED Dimmer Helligkeit (0-255)
uint8_t acDimmerBrightness = 0;          // AC Dimmer Helligkeit (0-255)
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
void setLEDDimmerBrightness(uint8_t brightness);
void setACDimmerBrightness(uint8_t brightness);

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
void handleACDimmer();
void handlePairing();
void handleClientDetail();
void handleRemoveClient();



// --- Zeitsteuerung für Rollos ---
unsigned long fensterrolloTimer = 0;
unsigned long tuerrolloTimer = 0;
const unsigned long rolloActiveTime = 60000; // ms, wie lange das Relais anzieht (1 Minute)

// --- Zeitsteuerung für Temperaturmessung ---
unsigned long lastTempUpdate = 0;
const unsigned long tempUpdateInterval = 10000; // 10 Sekunden (nicht zu schnell, aber regelmäßig)

// --- Zeitsteuerung für I2C Lese-Operationen (Throttling) ---
unsigned long lastI2CRead = 0;
const unsigned long i2cReadInterval = 100; // Eingänge nur alle 100ms lesen

// --- MCP23017 Interrupt Handler ---
volatile bool mcpInterruptFlag = false;  // Flag für Interrupt-Verarbeitung

void IRAM_ATTR mcpISR() {
  mcpInterruptFlag = true;  // Flag setzen, Verarbeitung in loop()
}

// --- MPR121 Interrupt Handler (VORBEREITET - auskommentiert) ---
// volatile bool mpr121InterruptFlag = false;  // Flag für MPR121-Interrupt
//
// void IRAM_ATTR mpr121ISR() {
//   mpr121InterruptFlag = true;  // Flag setzen, Verarbeitung in loop()
// }

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

  // Warte bis gültige IP-Adresse verfügbar ist (wichtig für OTA!)
  Serial.println("\n=== OTA Setup (Ethernet Only) ===");
  if (ETH.localIP() == IPAddress(0, 0, 0, 0)) {
    Serial.println("⚠️  WARNING: Keine IP-Adresse - OTA wird nicht verfügbar sein!");
  } else {
    Serial.print("📡 Ethernet IP: ");
    Serial.println(ETH.localIP());
    Serial.println("   OTA läuft NUR über Ethernet (LAN), NICHT über WiFi!");
    
    // OTA Setup nach Netzwerk-Initialisierung
    // ⚠️ WICHTIG: OTA nutzt automatisch das erste verfügbare Interface mit IP
    // Da Ethernet zuerst initialisiert wird, bindet OTA an ETH Interface
    ArduinoOTA.setHostname("WT32-KG-Controller");
    ArduinoOTA.setPassword("WT32_SecureOTA_2024"); // Sicheres Passwort
    ArduinoOTA.setPort(3232);  // Explizit Port 3232 setzen
    
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
    setLEDDimmerBrightness(0);
    setACDimmerBrightness(0);
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
  Serial.println("✅ OTA Ready - Hostname: WT32-KG-Controller");
  Serial.print("   Port: 3232, IP: ");
  Serial.println(ETH.localIP());
  Serial.println("   Upload-Befehl: pio run -e wt32-eth01-ota -t upload");
  
  // mDNS starten für Hostname-Auflösung
  if (MDNS.begin(HOSTNAME)) {
    Serial.print("✅ mDNS Responder gestartet: ");
    Serial.print(HOSTNAME);
    Serial.println(".local");
    MDNS.addService("http", "tcp", 80);  // HTTP Service ankündigen
    MDNS.addService("arduino", "tcp", 3232);  // OTA Service ankündigen
  } else {
    Serial.println("⚠️  mDNS Responder konnte nicht gestartet werden");
  }
  }
  Serial.println("==================\n");

  // MCP23017 Input Expander - DEAKTIVIERT (nicht angeschlossen)
  // ============================================================
  // Serial.println("\n=== MCP23017 Input Expander Initialisierung ===");
  // if (!mcpIn.begin_I2C(0x20)) {
  //   Serial.println("❌ ERROR: MCP23017 nicht gefunden auf Adresse 0x20!");
  // } else {
  //   Serial.println("✅ MCP23017 initialisiert auf Adresse 0x20");
  //   
  //   // Debugging: Erste Lese zu sehen ob I2C funktioniert
  //   uint16_t testRead = mcpIn.readGPIOAB();
  //   Serial.print("🔍 Test Reading GPIO: 0x");
  //   Serial.println(testRead, HEX);
  //   
  //   // GPIO13 als Interrupt-Pin konfigurieren (INPUT_PULLUP für Active-Low) - ZUERST!
  //   pinMode(MCP23017_IRQ_PIN, INPUT_PULLUP);
  //   Serial.println("✅ GPIO13 (ESP32 Interrupt Pin) konfiguriert");
  // }
  // Serial.println("=========================================\n");
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

  // MCP23017 Pin-Konfiguration - DEAKTIVIERT (nicht angeschlossen)
  // ================================================================
  // Serial.println("=== MCP23017 Pin-Konfiguration ===");
  // 
  // // Port A (GPA0-GPA7): INPUT mit aktivierten Pull-ups
  // for (int i = 0; i < 8; i++) {
  //   mcpIn.pinMode(i, INPUT);
  //   // 🔴 WICHTIG: GPPU = Internal 100kΩ Pull-up MANUELL via Register setzen
  //   // Die Adafruit-Lib könnte INPUT_PULLUP nicht korrekt setzen!
  // }
  // Serial.println("✅ Port A (GPA0-GPA7): INPUT konfiguriert");
  // 
  // // Port B (GPB0-GPB7): INPUT (Reserve)
  // for (int i = 8; i < 16; i++) {
  //   mcpIn.pinMode(i, INPUT);
  // }
  // Serial.println("✅ Port B (GPB0-GPB7): INPUT (Reserve) konfiguriert");
  // 
  // // 🔴 KRITISCH: Pull-ups via MCP23017 Registers aktivieren (nicht via Adafruit-Lib!)
  // // GPPU Register (0x0C für Port A, 0x0D für Port B): 1 = Pull-up ON
  // Wire.beginTransmission(0x20);
  // Wire.write(0x0C);  // GPPUA Register (ziehen bei GPA0-GPA7)
  // Wire.write(0xFF);  // Alle 8 Bits auf 1 = Pullups ON
  // Wire.endTransmission();
  // 
  // Wire.beginTransmission(0x20);
  // Wire.write(0x0D);  // GPPUB Register (ziehen bei GPB0-GPB7)
  // Wire.write(0xFF);  // Alle 8 Bits auf 1 = Pullups ON
  // Wire.endTransmission();
  // Serial.println("✅ GPPU Register (0x0C, 0x0D): Pull-ups aktiviert");
  // 
  // // Einmal durchlesen um initial Clear zu machen
  // uint16_t initialRead = mcpIn.readGPIOAB();
  // Serial.print("📊 Initial GPIO Reading: 0x");
  // Serial.println(initialRead, HEX);
  // Serial.println("=========================================\n");

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
  ledcSetup(PWM_CHANNEL_LED, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_DIMMER_PIN, PWM_CHANNEL_LED);
  setLEDDimmerBrightness(0); // Starten mit LEDs aus
  
  // PWM für AC Dimmer konfigurieren
  ledcSetup(PWM_CHANNEL_AC, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(AC_DIMMER_PIN, PWM_CHANNEL_AC);
  setACDimmerBrightness(0); // Starten mit AC Dimmer aus

  // TouchBoards - DEAKTIVIERT (nicht angeschlossen)
  // ===============================================
  // WENN AKTIVIERT: Uncomment die folgenden Zeilen
  // ===============================================
  // Serial.println("\n=== MPR121 Touch Panels Initialisierung ===");
  // initTouchBoards();
  // 
  // // GPIO16 als Interrupt-Pin konfigurieren (INPUT_PULLUP für Active-Low)
  // pinMode(MPR121_IRQ_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(MPR121_IRQ_PIN), mpr121ISR, FALLING);
  // Serial.println("✅ GPIO16 Interrupt-Handler registriert (FALLING edge)");
  // Serial.println("✅ 3x MPR121 Boards bereit (Wired-OR IRQ über Level Shifter)");
  // Serial.println("=========================================\n");
  Serial.println("TouchBoards: Übersprungen (nicht angeschlossen)");

  // Temperatursensoren initialisieren (DS18B20 auf GPIO4)
  Serial.println("\n=== DS18B20 Temperatursensor Initialisierung ===");
  initTemperatureSensors();
  
  // Initiale Temperaturmessung durchführen (damit Web UI sofort Wert hat)
  Serial.println("📊 Führe initiale Temperaturmessung durch...");
  updateTemperatures();
  
  // ⚠️ NACH Ethernet/OTA Setup, damit OTA bereits an ETH gebunden ist
  // WiFi.mode(WIFI_STA) in ESP-NOW nutzt nur WiFi-Radio für Peer-to-Peer
  // Ethernet bleibt primäres Interface für Internet/OTA/Webserver
  Serial.println("==============================================\n");

  // ESP-NOW Gateway initialisieren
  initESPNowGateway();

  // Webserver konfigurieren
  server.on("/", handleRoot);
  server.on("/home", handleHome);
  server.on("/espnow", handleESPNow);
  server.on("/toggle", handleToggle);
  server.on("/led", handleLEDDimmer);
  server.on("/kronleuchter", handleACDimmer);
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
  
  // === MCP23017 POLLING - DEAKTIVIERT (nicht angeschlossen) ===
  // =============================================================
  // static unsigned long lastMcpPoll = 0;
  // if (millis() - lastMcpPoll > 100) {
  //   lastMcpPoll = millis();
  //   
  //   // Alle 16 Pins auslesen (DEBUG-Info)
  //   uint16_t gpio_ab = mcpIn.readGPIOAB();
  //   
  //   // Einzelne Pins splitten
  //   uint8_t portA = gpio_ab & 0xFF;           // Bits 0-7 = GPA0-GPA7
  //   uint8_t portB = (gpio_ab >> 8) & 0xFF;    // Bits 8-15 = GPB0-GPB7
  //   
  //   // Debug Output (nur wenn sich was ändert oder alle 3 Sekunden)
  //   static unsigned long lastDebugPrint = 0;
  //   static uint16_t lastGpioPrint = 0xFFFF;
  //   if (gpio_ab != lastGpioPrint || millis() - lastDebugPrint > 3000) {
  //     Serial.print("📊 MCP23017 GPIO Status | Port A: 0x");
  //     Serial.print(portA, HEX);
  //     Serial.print(" Port B: 0x");
  //     Serial.println(portB, HEX);
  //     
  //     // Genaue Pin-Analyse für Port A (die einzigen momentan genutzten Pins):
  //     Serial.print("   GPA: ");
  //     for (int i = 0; i < 8; i++) {
  //       Serial.print((portA >> i) & 1 ? "H" : "L");
  //     }
  //     Serial.println();
  //     
  //     lastGpioPrint = gpio_ab;
  //     lastDebugPrint = millis();
  //   }
  //   
  //   // Eingänge in inputState[] speichern (für Web-UI)
  //   for (int i = 0; i < 8; i++) {
  //     inputState[i] = (portA >> i) & 1;      // GPA0-GPA7 → inputState[0-7]
  //     inputState[i + 8] = (portB >> i) & 1;  // GPB0-GPB7 → inputState[8-15]
  //   }
  //
  //   // IR-Switch Küche Logik
  //   handleIRSwitchKitchen();
  //   handleKreuzschaltungEG();
  //   handleKreuzschaltungKG();
  // }
  // 
  // // === MCP23017 INTERRUPT-GESTEUERTE VERARBEITUNG (ZUSÄTZLICH) ===
  // if (mcpInterruptFlag) {
  //   mcpInterruptFlag = false;  // Flag zurücksetzen
  //   Serial.println("⚡ MCP23017 Interrupt ausgelöst!");
  // }
  
  // === MPR121 INTERRUPT-GESTEUERTE VERARBEITUNG (VORBEREITET) ===
  // WENN AKTIVIERT: Uncomment die folgenden Zeilen und entferne Polling
  // if (mpr121InterruptFlag) {
  //   mpr121InterruptFlag = false;  // Flag zurücksetzen
  //   Serial.println("⚡ MPR121 Interrupt ausgelöst!");
  //   handleTouchEvents();  // Touch-Events verarbeiten
  // }
  
  // Touch Events verarbeiten (Polling-basiert, alle 100ms) - DEAKTIVIERT
  // if (millis() - lastI2CRead >= i2cReadInterval) {
  //   lastI2CRead = millis();
  //   handleTouchEvents();
  // }

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
  html += "</style>";
  
  // JavaScript für AJAX-Requests ohne Seiten-Reload
  html += "<script>";
  html += "function toggleRelay(r,btnElement){";
  html += "console.log('Toggle relay '+r);";
  html += "fetch('/toggle?r='+r).then(function(response){";
  html += "console.log('Response received');";
  html += "return response.json();";
  html += "}).then(function(data){";
  html += "console.log('Data:',data);";
  // Button finden und Status aktualisieren basierend auf Server-Response
  html += "var btn=btnElement;";
  html += "btn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  html += "console.log('State:'+data.state);";
  html += "if(data.state==1){";
  html += "console.log('Setting ON');";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-neutral');}";  // Rollos: aktiv = neutral
  html += "else{btn.classList.add('btn-on');}";  // Lampen: an = grün
  html += "}else{";
  html += "console.log('Setting OFF');";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-rollo');}";  // Rollos: inaktiv = orange
  html += "else{btn.classList.add('btn-off');}";  // Lampen: aus = rot
  html += "}";
  // Button-Text für Rollos updaten
  html += "if(r==0){btn.textContent=data.state==1?'⏹️ Stopp':'▲ Hoch';}";
  html += "if(r==1){btn.textContent=data.state==1?'⏹️ Stopp':'▼ Runter';}";
  html += "if(r==2){btn.textContent=data.state==1?'⏹️ Stopp':'▲ Hoch';}";
  html += "if(r==3){btn.textContent=data.state==1?'⏹️ Stopp':'▼ Runter';}";
  html += "}).catch(function(error){";
  html += "console.error('Error:',error);";
  html += "});";
  html += "}";
  html += "</script>";
  
  html += "</head><body>";
  
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
  html += "<h3>💡 LED Dimmer (GPIO15 - HW-517)</h3>";
  int brightnessPercent = (ledDimmerBrightness * 100) / 255;
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
  
  // AC Dimmer Kronleuchter
  html += "<h3>💡 AC Dimmer Kronleuchter (GPIO4 - YYAC-3S)</h3>";
  int acPercent = (acDimmerBrightness * 100) / 255;
  html += "<div style='margin:20px 0;'>";
  html += "<label for='kronleuchterSlider'>Helligkeit: <b>" + String(acPercent) + "%</b></label><br>";
  html += "<input type='range' id='kronleuchterSlider' min='0' max='100' value='" + String(acPercent) + "' ";
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
  
  html += "<button onclick='toggleRelay(0,this)' class='btn " + fensterHochClass + "'>" + fensterHochSymbol + "</button>";
  html += "<button onclick='toggleRelay(1,this)' class='btn " + fensterRunterClass + "'>" + fensterRunterSymbol + "</button>";
  
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
  
  html += "<button onclick='toggleRelay(2,this)' class='btn " + tuerHochClass + "'>" + tuerHochSymbol + "</button>";
  html += "<button onclick='toggleRelay(3,this)' class='btn " + tuerRunterClass + "'>" + tuerRunterSymbol + "</button>";
  
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
  html += "<button onclick='toggleRelay(4,this)' class='btn " + aussenGartenClass + "'>💡 Aussenlampe Garten (R04)</button><br>";
  
  // R05 - Steinlampe
  String steinlampeClass = relayState[5] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(5,this)' class='btn " + steinlampeClass + "'>💡 Steinlampe (R05)</button><br>";
  
  // R06 - KG Flurlampe
  String kgFlurClass = relayState[6] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(6,this)' class='btn " + kgFlurClass + "'>💡 KG Flurlampe (R06)</button><br>";
  
  // R07 - Kuechenarbeitslampe
  String kuechenArbeitClass = relayState[7] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(7,this)' class='btn " + kuechenArbeitClass + "'>💡 Kuechenarbeitslampe (R07)</button><br>";
  
  // R08 - Kuechenlampe
  String kuechenClass = relayState[8] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(8,this)' class='btn " + kuechenClass + "'>💡 Kuechenlampe (R08)</button><br>";
  
  // R09 - EG Flurlampe
  String egFlurClass = relayState[9] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(9,this)' class='btn " + egFlurClass + "'>💡 EG Flurlampe (R09)</button><br>";
  
  // R10 - Traegerlampen
  String traegerClass = relayState[10] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(10,this)' class='btn " + traegerClass + "'>💡 Traegerlampen (R10)</button><br>";
  
  // R11 - Kronleuchter (mit Dimmerfunktion)
  String kronleuchterClass = relayState[11] ? "btn-on" : "btn-off";
  acPercent = (acDimmerBrightness * 100) / 255;  // Variable wurde bereits weiter oben deklariert
  html += "<button onclick='toggleRelay(11,this)' class='btn " + kronleuchterClass + "'>💡 Kronleuchter (R11)";
  if (relayState[11]) {
    html += " (" + String(acPercent) + "%)";  
  }
  html += "</button><br>";
  
  // R12 - Reserve Wohnzimmer
  String wohnzimmer2Class = relayState[12] ? "btn-on" : "btn-off";
  html += "<button onclick='toggleRelay(12,this)' class='btn " + wohnzimmer2Class + "'>💡 Reserve Wohnzimmer (R12)</button>";
  html += "</div>";
  
  html += "</div>";

  // Freie Relais
  html += "<h3>⚙️ Freie Relais & Sonstiges</h3>";
  html += "<div class='card'>";
  
  // Freie Relais 13-23
  for (int i = 13; i < 15; i++) {
    String relaisClass = relayState[i] ? "btn-on" : "btn-off";
    String relaisStatus = relayState[i] ? "EIN" : "AUS";
    html += "<button onclick='toggleRelay(" + String(i) + ",this)' class='btn " + relaisClass + "'>🔌 " + String(relayNames[i]) + ": " + relaisStatus + "</button><br>";
  }
  
  // Freie Relais 15-23
  for (int i = 15; i < 24; i++) {
    String relaisClass = relayState[i] ? "btn-on" : "btn-off";
    String relaisStatus = relayState[i] ? "EIN" : "AUS";
    html += "<button onclick='toggleRelay(" + String(i) + ",this)' class='btn " + relaisClass + "'>🔌 " + String(relayNames[i]) + ": " + relaisStatus + "</button>";
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
  
  // AJAX-Support: JSON mit aktuellem Status zurückgeben
  // Browser kann Button-Status dynamisch updaten ohne Seiten-Reload
  String response = "{\"relay\":" + String(idx) + ",\"state\":" + String(relayState[idx]) + "}";
  server.send(200, "application/json", response);
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
// PWM LED Dimmer Funktionen (GPIO15, HW-517 V0.0.1)
// ======================================================
void setLEDDimmerBrightness(uint8_t brightness) {
  ledDimmerBrightness = brightness;
  ledcWrite(PWM_CHANNEL_LED, brightness);  // 🔴 BUGFIX: PWM_CHANNEL nicht LED_DIMMER_PIN!
  Serial.println("LED Dimmer (HW-517) Helligkeit: " + String(brightness) + " (" + String(brightness * 100 / 255) + "%)");
}

void handleLEDDimmer() {
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    if (brightness >= 0 && brightness <= 100) {
      // Umrechnung von Prozent (0-100) zu PWM-Wert (0-255)
      uint8_t pwmValue = (brightness * 255) / 100;
      setLEDDimmerBrightness(pwmValue);
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

// ======================================================
// AC Dimmer Funktionen (GPIO4, YYAC-3S)
// ======================================================
void setACDimmerBrightness(uint8_t brightness) {
  acDimmerBrightness = brightness;
  ledcWrite(PWM_CHANNEL_AC, brightness);  // 🔴 BUGFIX: PWM_CHANNEL nicht AC_DIMMER_PIN!
  Serial.println("AC Dimmer (YYAC-3S) Helligkeit: " + String(brightness) + " (" + String(brightness * 100 / 255) + "%)");
}

void handleACDimmer() {
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    if (brightness >= 0 && brightness <= 100) {
      // Umrechnung von Prozent (0-100) zu PWM-Wert (0-255)
      uint8_t pwmValue = (brightness * 255) / 100;
      setACDimmerBrightness(pwmValue);
      
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
  if (acDimmerBrightness == 0) {
    setACDimmerBrightness(127); // 50% Helligkeit
    relayState[11] = 1; // R11 EIN
    pcaRel2.digitalWrite(3, LOW); // Board B, Pin 3 (R11 = 8+3)
  } else {
    setACDimmerBrightness(0); // AUS
    relayState[11] = 0; // R11 AUS
    pcaRel2.digitalWrite(3, HIGH); // Board B, Pin 3 (R11 = 8+3)
  }
}

void dimKronleuchter(bool dimUp) {
  const uint8_t dimStep = 15; // Dimm-Schritte
  
  if (dimUp) {
    // Heller dimmen
    if (acDimmerBrightness < 240) {
      setACDimmerBrightness(acDimmerBrightness + dimStep);
      if (acDimmerBrightness > 0) {
        relayState[11] = 1; // R11 EIN wenn > 0
        pcaRel2.digitalWrite(3, LOW); // Board B, Pin 3 (R11 = 8+3)
      }
    }
  } else {
    // Dunkler dimmen
    if (acDimmerBrightness > dimStep) {
      setACDimmerBrightness(acDimmerBrightness - dimStep);
    } else {
      setACDimmerBrightness(0); // Komplett aus
      relayState[11] = 0; // R11 AUS
      pcaRel2.digitalWrite(3, HIGH); // Board B, Pin 3 (R11 = 8+3)
    }
  }
}

// ======================================================
// 1-Wire Temperatursensor Funktionen (1x DS18B20 für Schaltschrank)
// ======================================================
void initTemperatureSensors() {
  Serial.println("🌡️  1-Wire Bus starten (GPIO4)...");
  Serial.println("✅ GPIO4 = Standard GPIO mit internem Pull-up möglich!");
  
  // GPIO4 mit internem Pull-up aktivieren
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  delay(100);
  
  temperaturSensoren.begin();
  
  // Längere Pause für zuverlässige 1-Wire Enumeration (besonders wichtig bei Input-only Pins)
  delay(500);
  
  uint8_t sensorCount = temperaturSensoren.getDeviceCount();
  Serial.println("📊 DS18B20 Sensoren gefunden: " + String(sensorCount));
  
  if (sensorCount > 0) {
    DeviceAddress tempDeviceAddress;
    if (temperaturSensoren.getAddress(tempDeviceAddress, 0)) {
      // Zeige die Sensor-ROM-Adresse (für Debugging)
      Serial.print("✅ Sensor erkannt! ROM Adresse: ");
      for (uint8_t i = 0; i < 8; i++) {
        Serial.print("0x");
        if (tempDeviceAddress[i] < 0x10) Serial.print("0");
        Serial.print(tempDeviceAddress[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      temperaturSensoren.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      Serial.println("✅ DS18B20 Sensor konfiguriert");
      Serial.println("   Genauigkeit: 10-bit (0.25°C), Konversionszeit: ~188ms");
      Serial.println("✅ Polling-Intervall: 10 Sekunden");
    } else {
      Serial.println("❌ Fehler: ROM-Adresse konnte nicht gelesen werden!");
    }
  } else {
    Serial.println("❌ FEHLER: Kein DS18B20 Sensor auf GPIO4 gefunden!");
    Serial.println("⚠️  → Überprüfe:");
    Serial.println("   1. Verdrahtung: VCC=3.3V, GND=GND, Data=GPIO4");
    Serial.println("   2. 4.7kΩ Pull-up Widerstand (optional, GPIO4 hat internen Pull-up)");
    Serial.println("   3. Sensor-Kontakte (Kurzschluss? Wackelkontakt?)");
    Serial.println("   4. Sensor-Typ: DS18B20 (nicht DHT22 oder andere!)");
  }
  
  schaltschrankTemp = -999.0;
  lastSchaltschrankTemp = -999.0;
}

void updateTemperatures() {
  static unsigned long lastDebugPrint = 0;
  
  // Temperaturkonversion starten
  temperaturSensoren.requestTemperatures();
  // ⏱️ WICHTIG: 10-bit Genauigkeit braucht ~188ms für Konversion
  delay(200);
  
  float temp = temperaturSensoren.getTempCByIndex(0);
  
  // Debug: Zeige alle 10 Sekunden den Sensor-Status
  if (millis() - lastDebugPrint > 10000) {
    Serial.print("📊 1-Wire Temperature Read: ");
    if (temp != DEVICE_DISCONNECTED_C) {
      Serial.print(temp, 2);
      Serial.println("°C");
    } else {
      Serial.println("DISCONNECTED");
    }
    lastDebugPrint = millis();
  }
  
  if (temp != DEVICE_DISCONNECTED_C && temp > -55.0 && temp < 125.0) {
    // Nur bei Änderung um mindestens 0.1°C aktualisieren
    if (abs(temp - lastSchaltschrankTemp) >= 0.1) {
      lastSchaltschrankTemp = temp;
      schaltschrankTemp = temp;
      Serial.println("✅ Schaltschrank Temperatur aktualisiert: " + String(temp, 1) + "°C");
    }
  } else {
    schaltschrankTemp = -999.0; // Fehlercode für nicht angeschlossen
    if (temp == DEVICE_DISCONNECTED_C) {
      Serial.println("❌ DS18B20 Sensor nicht erreichbar (DEVICE_DISCONNECTED)");
      Serial.println("⚠️  Prüfe: Pull-up Widerstand, Verdrahtung, Sensor-Kontakte");
    } else {
      Serial.println("❌ DS18B20 ungültiger Wert: " + String(temp) + "°C (außerhalb -55°C bis 125°C)");
    }
  }
}

String getTemperatureHTML() {
  String html = "<h3>🌡️ Schaltschrank Temperatur</h3>";
  html += "<table style='margin-bottom:20px;'><tr>";
  html += "<th>Komponente</th><th>Wert</th><th>Status</th></tr>";
  
  html += "<tr>";
  html += "<td>DS18B20 Sensor (GPIO4)</td>";
  
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
  html += "<p style='font-size:12px;color:gray;'>🔄 Kontinuierliche Messung alle 10 Sekunden | GPIO4 mit internem Pull-up</p>";
  
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


