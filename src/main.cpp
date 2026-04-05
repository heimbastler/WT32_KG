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
#include <Preferences.h>  // NVRAM Speicher für persistente Daten
#include <PubSubClient.h>  // MQTT Client für Home Assistant
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

// ---------- Boot-Verzögerung für Kreuzschaltungen ----------
unsigned long bootTime = 0;       // Zeitstempel beim Boot
const unsigned long bootDelay = 5000;  // 5 Sekunden Verzögerung nach Boot

// ---------- Temperatursensor Zustand (1x DS18B20 für Schaltschrank) ----------
float schaltschrankTemp = -999.0;       // Schaltschrank Temperatur (°C)
float lastSchaltschrankTemp = -999.0;   // Letzte Temperatur für Änderungserkennung

// ---------- Relaisnamen, Index entspricht R00...R23 ----------
// ACHTUNG: Index 0 = R00, Index 1 = R01, ... Index 22 = R22, Index 23 = R23
// TouchBoard-Positionen als Kommentar aus old.ccp ergänzt
// Namen sind editierbar über Web UI und werden in Preferences gespeichert
String relayNames[24] = {
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
  "Kronleuchter Relais",    // R11, idx 11, TouchBoard1: case 2: Nur Relais (AC Dimmer ist separat auf GPIO2)
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

// Home Assistant Device-Typ & Enable-Status für jedes Relais
// Typen: "none", "switch", "light", "cover", "fan"
// ⚠️ R00-R03 werden durch Cover-Entities gesteuert (siehe coverRelayPairs unten)
String relayHAType[24] = {
  "none",    // R00 - Fensterrollo hoch (durch Cover gesteuert)
  "none",    // R01 - Fensterrollo runter (durch Cover gesteuert)
  "none",    // R02 - Türrollo hoch (durch Cover gesteuert)
  "none",    // R03 - Türrollo runter (durch Cover gesteuert)
  "light",   // R04 - Aussenlampe Garten
  "light",   // R05 - Steinlampe
  "light",   // R06 - KG Flurlampe
  "light",   // R07 - Kuechenarbeitslampe
  "light",   // R08 - Kuechenlampe
  "light",   // R09 - EG Flurlampe
  "light",   // R10 - Traegerlampen
  "light",   // R11 - Kronleuchter Relais
  "light",   // R12 - Reserve Wohnzimmer
  "switch",  // R13 - Relais 13
  "switch",  // R14 - Relais 14
  "switch",  // R15 - Relais 15
  "switch",  // R16 - Relais 16
  "switch",  // R17 - Relais 17
  "switch",  // R18 - Relais 18
  "switch",  // R19 - Relais 19
  "switch",  // R20 - Relais 20
  "switch",  // R21 - Relais 21
  "switch",  // R22 - Relais 22
  "switch"   // R23 - Relais 23
};

// HA Discovery Enable/Disable pro Relais
// ⚠️ R00-R03 disabled, da diese durch Cover-Entities gesteuert werden
bool relayHAEnabled[24] = {
  false, false, false, false,  // R00-R03: Durch Covers gesteuert
  true,  true,  true,  true,   // R04-R07: Lampen enabled
  true,  true,  true,  true,   // R08-R11: Lampen enabled  
  true,  false, false, false,  // R12-R15: R12 enabled, Rest disabled
  false, false, false, false,  // R16-R19: disabled
  false, false, false, false   // R20-R23: disabled
};

// ======================================================
// COVER CONFIGURATION (Rollos/Jalousien) 
// ======================================================
// 2 virtuelle Cover-Devices, jedes steuert 2 physische Relais im Flip-Flop
String coverNames[2] = {
  "Fensterrollo",  // Cover 0: R00 (open) + R01 (close)
  "Türrollo"       // Cover 1: R02 (open) + R03 (close)
};

bool coverHAEnabled[2] = {
  true,  // Fensterrollo in HA anzeigen
  true   // Türrollo in HA anzeigen
};

// Relay-Zuordnung: [cover_idx][0=open_relay, 1=close_relay]
int coverRelayPairs[2][2] = {
  {0, 1},  // Cover 0 "Fensterrollo": R00=hoch, R01=runter
  {2, 3}   // Cover 1 "Türrollo": R02=hoch, R03=runter
};

Preferences preferences;  // NVRAM Speicher für persistente Relay-Namen

// ======================================================
// INPUT CONFIGURATION (MCP23017 Binary Sensors)
// ======================================================
// 16 digitale Eingänge vom MCP23017
String inputNames[16] = {
  "IR-Küche Links",    // IN00
  "IR-Küche Rechts",   // IN01
  "Kreuz EG1",         // IN02
  "Kreuz EG2",         // IN03
  "Kreuz KG1",         // IN04
  "Kreuz KG2",         // IN05
  "Kreuz KG3",         // IN06
  "Frei",              // IN07
  "Frei",              // IN08
  "Frei",              // IN09
  "Frei",              // IN10
  "Frei",              // IN11
  "Frei",              // IN12
  "Frei",              // IN13
  "Frei",              // IN14
  "Frei"               // IN15
};

// HA Device Class für Binary Sensors
// Typen: "none", "door", "window", "motion", "occupancy", "opening", "garage_door", "switch"
String inputHAType[16] = {
  "switch",  // IN00 - IR-Küche Links
  "switch",  // IN01 - IR-Küche Rechts
  "switch",  // IN02 - Kreuz EG1
  "switch",  // IN03 - Kreuz EG2
  "switch",  // IN04 - Kreuz KG1
  "switch",  // IN05 - Kreuz KG2
  "switch",  // IN06 - Kreuz KG3
  "none",    // IN07-IN15: Frei
  "none", "none", "none", "none", "none", "none", "none", "none"
};

bool inputHAEnabled[16] = {
  true,  true,  true,  true,   // IN00-IN03: Küche + EG Kreuzschaltungen
  true,  true,  true,  false,  // IN04-IN07: KG Kreuzschaltungen + 1 Frei
  false, false, false, false,  // IN08-IN11: Frei
  false, false, false, false   // IN12-IN15: Frei
};

// ======================================================
// MQTT CONFIGURATION & CLIENT
// ======================================================
struct MQTTConfig {
  char broker[64] = "192.168.178.1";  // MQTT Broker IP
  int port = 1883;
  char user[32] = "";
  char password[32] = "";
  bool enabled = false;
};

MQTTConfig mqttConfig;
WiFiClient ethClient;  // Ethernet Client für MQTT
PubSubClient mqttClient(ethClient);

// MQTT Topics
#define MQTT_BASE_TOPIC "wt32kg"
#define MQTT_DISCOVERY_PREFIX "homeassistant"

// MQTT State
unsigned long lastMQTTReconnect = 0;
const unsigned long mqttReconnectInterval = 5000;  // 5 Sekunden
bool mqttDiscoverySent = false;
unsigned long lastMQTTPublish = 0;
const unsigned long mqttPublishInterval = 1000;  // Min. 1 Sekunde zwischen State-Updates

// MPR121 Touch Enable/Disable
bool mpr121Enabled = true;  // Standard: AKTIV nach Boot

// MQTT Funktionsprototypen
void loadMQTTConfig();
void saveMQTTConfig();
void mqttConnect();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishMQTTDiscovery();
void publishRelayState(int relayIndex);
void publishCoverState(int coverIndex);
void publishMPR121State();
void publishAllStates();
void publishInputState(int inputIndex);
void setRelayState(int relayIndex, int state);
void setCoverState(int coverIndex, const char* action);  // OPEN, CLOSE, STOP
void loadMPR121State();
void saveMPR121State();
void loadCoverConfig();
void saveCoverConfig(int coverIndex, String name, bool enabled);
void loadInputConfig();
void saveInputConfig(int inputIndex, String name, String type, bool enabled);

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
void handleInfo();
void handleToggle();
void handleInputs();
void handleLEDDimmer();
void handleACDimmer();
void handlePairing();
void handleClientDetail();
void handleRemoveClient();
void handleEdit();
void handleSaveName();
void handleSaveCover();
void handleCover();
void handleSaveInput();
void handleMQTT();
void handleSaveMQTT();
void handleRestart();
void handleToggleMPR121();



// --- Zeitsteuerung für Rollos ---
unsigned long fensterrolloTimer = 0;
unsigned long tuerrolloTimer = 0;
const unsigned long rolloActiveTime = 60000; // ms, wie lange das Relais anzieht (1 Minute)

// --- Zeitsteuerung für Temperaturmessung ---
unsigned long lastTempUpdate = 0;
const unsigned long tempUpdateInterval = 30000; // 30 Sekunden (schont den µC)

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
// RELAY NAMEN LADEN/SPEICHERN (NVRAM Preferences)
// ======================================================
void loadRelayNames() {
  preferences.begin("relay-names", true); // Read-only Modus
  
  for (int i = 0; i < 24; i++) {
    String key = "R" + String(i);
    String savedName = preferences.getString(key.c_str(), "");
    
    // Nur laden wenn ein Name gespeichert wurde (nicht leer)
    if (savedName.length() > 0) {
      relayNames[i] = savedName;
      Serial.println("✅ Relay " + key + " Name geladen: " + savedName);
    }
  }
  
  preferences.end();
  Serial.println("===========================================\n");
}

void loadRelayHAConfig() {
  preferences.begin("relay-ha-config", true); // Read-only Modus
  
  for (int i = 0; i < 24; i++) {
    String keyType = "type_" + String(i);
    String keyEnabled = "en_" + String(i);
    
    // Type laden (falls gespeichert, sonst Default behalten)
    String savedType = preferences.getString(keyType.c_str(), "");
    if (savedType.length() > 0) {
      relayHAType[i] = savedType;
    }
    
    // Enabled laden (Default ist der aktuelle Wert aus dem Array)
    // Wir prüfen ob der Key existiert, indem wir auf einen unmöglichen Wert prüfen
    if (preferences.isKey(keyEnabled.c_str())) {
      relayHAEnabled[i] = preferences.getBool(keyEnabled.c_str(), true);
    }
  }
  
  preferences.end();
  Serial.println("✅ HA Device-Konfiguration geladen\n");
}

void saveRelayHAConfig(int relayIndex, String type, bool enabled) {
  if (relayIndex < 0 || relayIndex >= 24) {
    Serial.println("❌ Ungültiger Relay Index: " + String(relayIndex));
    return;
  }
  
  preferences.begin("relay-ha-config", false); // Read-Write Modus
  
  String keyType = "type_" + String(relayIndex);
  String keyEnabled = "en_" + String(relayIndex);
  
  preferences.putString(keyType.c_str(), type);
  preferences.putBool(keyEnabled.c_str(), enabled);
  
  preferences.end();
  
  // Arrays aktualisieren
  relayHAType[relayIndex] = type;
  relayHAEnabled[relayIndex] = enabled;
  
  Serial.println("✅ R" + String(relayIndex) + " HA Config: Type=" + type + ", Enabled=" + String(enabled));
}

// ======================================================
// COVER CONFIG LADEN/SPEICHERN (NVRAM Preferences)
// ======================================================
void loadCoverConfig() {
  preferences.begin("cover-config", true); // Read-only Modus
  
  for (int i = 0; i < 2; i++) {
    String keyName = "cname_" + String(i);
    String keyEnabled = "cen_" + String(i);
    
    // Name laden (falls gespeichert, sonst Default behalten)
    String savedName = preferences.getString(keyName.c_str(), "");
    if (savedName.length() > 0) {
      coverNames[i] = savedName;
    }
    
    // Enabled laden
    if (preferences.isKey(keyEnabled.c_str())) {
      coverHAEnabled[i] = preferences.getBool(keyEnabled.c_str(), true);
    }
  }
  
  preferences.end();
  Serial.println("✅ Cover-Konfiguration geladen\n");
}

void saveCoverConfig(int coverIndex, String name, bool enabled) {
  if (coverIndex < 0 || coverIndex >= 2) {
    Serial.println("❌ Ungültiger Cover Index: " + String(coverIndex));
    return;
  }
  
  preferences.begin("cover-config", false); // Read-Write Modus
  
  String keyName = "cname_" + String(coverIndex);
  String keyEnabled = "cen_" + String(coverIndex);
  
  preferences.putString(keyName.c_str(), name);
  preferences.putBool(keyEnabled.c_str(), enabled);
  
  preferences.end();
  
  // Arrays aktualisieren
  coverNames[coverIndex] = name;
  coverHAEnabled[coverIndex] = enabled;
  
  Serial.println("✅ Cover " + String(coverIndex) + " Config: Name=" + name + ", Enabled=" + String(enabled));
}

// ======================================================
// INPUT CONFIG LADEN/SPEICHERN (NVRAM Preferences)
// ======================================================
void loadInputConfig() {
  preferences.begin("input-config", true); // Read-only Modus
  
  for (int i = 0; i < 16; i++) {
    String keyName = "iname_" + String(i);
    String keyType = "itype_" + String(i);
    String keyEnabled = "ien_" + String(i);
    
    // Name laden (falls gespeichert, sonst Default behalten)
    String savedName = preferences.getString(keyName.c_str(), "");
    if (savedName.length() > 0) {
      inputNames[i] = savedName;
    }
    
    // Type laden
    String savedType = preferences.getString(keyType.c_str(), "");
    if (savedType.length() > 0) {
      inputHAType[i] = savedType;
    }
    
    // Enabled laden
    if (preferences.isKey(keyEnabled.c_str())) {
      inputHAEnabled[i] = preferences.getBool(keyEnabled.c_str(), false);
    }
  }
  
  preferences.end();
  Serial.println("✅ Input-Konfiguration geladen\n");
}

void saveInputConfig(int inputIndex, String name, String type, bool enabled) {
  if (inputIndex < 0 || inputIndex >= 16) {
    Serial.println("❌ Ungültiger Input Index: " + String(inputIndex));
    return;
  }
  
  preferences.begin("input-config", false); // Read-Write Modus
  
  String keyName = "iname_" + String(inputIndex);
  String keyType = "itype_" + String(inputIndex);
  String keyEnabled = "ien_" + String(inputIndex);
  
  preferences.putString(keyName.c_str(), name);
  preferences.putString(keyType.c_str(), type);
  preferences.putBool(keyEnabled.c_str(), enabled);
  
  preferences.end();
  
  // Arrays aktualisieren
  inputNames[inputIndex] = name;
  inputHAType[inputIndex] = type;
  inputHAEnabled[inputIndex] = enabled;
  
  Serial.println("✅ IN" + String(inputIndex) + " Config: Name=" + name + ", Type=" + type + ", Enabled=" + String(enabled));
}

void saveRelayName(int relayIndex, String newName) {
  if (relayIndex < 0 || relayIndex >= 24) {
    Serial.println("❌ Ungültiger Relay Index: " + String(relayIndex));
    return;
  }
  
  preferences.begin("relay-names", false); // Read-Write Modus
  String key = "R" + String(relayIndex);
  preferences.putString(key.c_str(), newName);
  preferences.end();
  
  relayNames[relayIndex] = newName;
  Serial.println("✅ Relay " + key + " Name gespeichert: " + newName);
}

// ======================================================
// MQTT CONFIGURATION & FUNCTIONS
// ======================================================
void loadMQTTConfig() {
  preferences.begin("mqtt-config", true); // Read-only
  
  mqttConfig.enabled = preferences.getBool("enabled", false);
  preferences.getString("broker", mqttConfig.broker, 64);
  mqttConfig.port = preferences.getInt("port", 1883);
  preferences.getString("user", mqttConfig.user, 32);
  preferences.getString("password", mqttConfig.password, 32);
  
  preferences.end();
  
  Serial.println("\n=== MQTT Konfiguration geladen ===");
  Serial.println("Enabled: " + String(mqttConfig.enabled ? "Ja" : "Nein"));
  Serial.println("Broker: " + String(mqttConfig.broker) + ":" + String(mqttConfig.port));
  Serial.println("User: " + String(mqttConfig.user));
  Serial.println("======================================\n");
}

void loadMPR121State() {
  preferences.begin("mpr121-config", true); // Read-only
  mpr121Enabled = preferences.getBool("enabled", true);  // Default: true
  preferences.end();
  
  Serial.println("\n=== MPR121 Status geladen ===");
  Serial.println("Touch-Eingabe: " + String(mpr121Enabled ? "AKTIV" : "DEAKTIVIERT"));
  Serial.println("================================\n");
}

void saveMPR121State() {
  preferences.begin("mpr121-config", false); // Read-write
  preferences.putBool("enabled", mpr121Enabled);
  preferences.end();
  
  Serial.println("📝 MPR121 Status gespeichert: " + String(mpr121Enabled ? "AKTIV" : "DEAKTIVIERT"));
}

void saveMQTTConfig() {
  preferences.begin("mqtt-config", false); // Read-Write
  
  preferences.putBool("enabled", mqttConfig.enabled);
  preferences.putString("broker", mqttConfig.broker);
  preferences.putInt("port", mqttConfig.port);
  preferences.putString("user", mqttConfig.user);
  preferences.putString("password", mqttConfig.password);
  
  preferences.end();
  
  Serial.println("✅ MQTT Konfiguration gespeichert");
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Payload in String konvertieren
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("MQTT RX: ");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(message);
  
  // ======================================================
  // COVER COMMAND TOPIC: wt32kg/cover/X/set
  // ======================================================
  String topicStr = String(topic);
  if (topicStr.startsWith(String(MQTT_BASE_TOPIC) + "/cover/")) {
    // Cover-Index extrahieren
    int startIdx = String(MQTT_BASE_TOPIC).length() + 7;  // "/cover/"
    int endIdx = topicStr.indexOf("/set");
    if (endIdx == -1) return;
    
    String coverNumStr = topicStr.substring(startIdx, endIdx);
    int coverIndex = coverNumStr.toInt();
    
    if (coverIndex < 0 || coverIndex >= 2) return;
    
    // Command verarbeiten (OPEN, CLOSE, STOP)
    String cmd = String(message);
    cmd.toUpperCase();
    
    setCoverState(coverIndex, cmd.c_str());
    return;
  }
  
  // ======================================================
  // RELAY COMMAND TOPICS: wt32kg/relay/XX/set
  // ======================================================
  if (topicStr.startsWith(String(MQTT_BASE_TOPIC) + "/relay/")) {
    // Relay-Index extrahieren
    int startIdx = String(MQTT_BASE_TOPIC).length() + 7;  // "/relay/"
    int endIdx = topicStr.indexOf("/set");
    if (endIdx == -1) return;
    
    String relayNumStr = topicStr.substring(startIdx, endIdx);
    int relayIndex = relayNumStr.toInt();
    
    if (relayIndex < 0 || relayIndex >= 24) return;
    
    // ⚠️ R00-R03 werden über Cover-Commands gesteuert, nicht über Relay-Commands
    if (relayIndex >= 0 && relayIndex <= 3) {
      Serial.println("⚠️ R" + String(relayIndex) + " wird über Cover-Devices gesteuert");
      return;
    }
    
    // Command verarbeiten (nur für R04-R23)
    String cmd = String(message);
    cmd.toUpperCase();
    
    if (cmd == "ON") {
      setRelayState(relayIndex, 1);
    } else if (cmd == "OFF") {
      setRelayState(relayIndex, 0);
    }
    return;
  }
  
  // ======================================================
  // MPR121 COMMAND TOPIC: wt32kg/mpr121/set
  // ======================================================
  if (topicStr == String(MQTT_BASE_TOPIC) + "/mpr121/set") {
    String cmd = String(message);
    cmd.toUpperCase();
    
    if (cmd == "ON") {
      mpr121Enabled = true;
      saveMPR121State();
      publishMPR121State();
      Serial.println("✅ MPR121 Touch-Eingabe via MQTT AKTIVIERT");
    } else if (cmd == "OFF") {
      mpr121Enabled = false;
      saveMPR121State();
      publishMPR121State();
      Serial.println("⛔ MPR121 Touch-Eingabe via MQTT DEAKTIVIERT");
    }
  }
}

void mqttConnect() {
  if (!mqttConfig.enabled) return;
  
  if (millis() - lastMQTTReconnect < mqttReconnectInterval) return;
  lastMQTTReconnect = millis();
  
  Serial.print("MQTT: Verbinde zu ");
  Serial.print(mqttConfig.broker);
  Serial.print(":");
  Serial.println(mqttConfig.port);
  
  // Client ID mit MAC-Adresse
  String clientId = "WT32KG-" + WiFi.macAddress();
  clientId.replace(":", "");
  
  bool connected;
  if (strlen(mqttConfig.user) > 0) {
    connected = mqttClient.connect(clientId.c_str(), mqttConfig.user, mqttConfig.password);
  } else {
    connected = mqttClient.connect(clientId.c_str());
  }
  
  if (connected) {
    Serial.println("✅ MQTT verbunden!");
    
    // Subscribe zu allen Relay Command-Topics (R00-R23, aber R00-R03 durch Cover gesteuert)
    for (int i = 0; i < 24; i++) {
      String cmdTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/set";
      mqttClient.subscribe(cmdTopic.c_str());
    }
    
    // Subscribe zu Cover Command Topics
    for (int i = 0; i < 2; i++) {
      String coverCmdTopic = String(MQTT_BASE_TOPIC) + "/cover/" + String(i) + "/set";
      mqttClient.subscribe(coverCmdTopic.c_str());
    }
    
    // Subscribe zu MPR121 Command Topic
    String mpr121CmdTopic = String(MQTT_BASE_TOPIC) + "/mpr121/set";
    mqttClient.subscribe(mpr121CmdTopic.c_str());
    
    // Discovery senden (nur einmal nach Verbindung)
    publishMQTTDiscovery();
    
    // Alle States publishen
    publishAllStates();
    
    mqttDiscoverySent = true;
  } else {
    Serial.print("❌ MQTT Verbindung fehlgeschlagen, rc=");
    Serial.println(mqttClient.state());
  }
}

void publishMQTTDiscovery() {
  if (!mqttClient.connected()) return;
  
  Serial.println("📡 Sende Home Assistant Discovery Messages...");
  
  // Device-Info (wird bei allen Entities verwendet)
  String deviceInfo = "\"device\":{";
  deviceInfo += "\"identifiers\":[\"wt32kg\"],";
  deviceInfo += "\"name\":\"WT32-KG Controller\",";
  deviceInfo += "\"model\":\"WT32-ETH01\",";
  deviceInfo += "\"manufacturer\":\"ESP32\"";
  deviceInfo += "}";
  
  // Für alle 24 Relais Discovery-Messages senden
  for (int i = 0; i < 24; i++) {
    // Skip wenn nicht enabled
    if (!relayHAEnabled[i]) {
      Serial.println("  ⏭️ R" + String(i) + " übersprungen (deaktiviert)");
      continue;
    }
    
    String type = relayHAType[i];
    String name = relayNames[i] + " (R" + (i < 10 ? "0" : "") + String(i) + ")";
    String deviceId = "wt32kg_relay_" + String(i);
    String objectId = "relay_" + String(i);
    
    // Skip bei "none"
    if (type == "none") {
      Serial.println("  ⏭️ R" + String(i) + " übersprungen (type=none)");
      continue;
    }
    
    // SWITCH Discovery
    if (type == "switch") {
      String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/switch/" + deviceId + "/config";
      String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/state";
      String commandTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/set";
      
      String payload = "{";
      payload += "\"name\":\"" + name + "\",";
      payload += "\"unique_id\":\"" + deviceId + "\",";
      payload += "\"object_id\":\"" + objectId + "\",";
      payload += "\"state_topic\":\"" + stateTopic + "\",";
      payload += "\"command_topic\":\"" + commandTopic + "\",";
      payload += "\"payload_on\":\"ON\",\"payload_off\":\"OFF\",";
      payload += "\"state_on\":\"ON\",\"state_off\":\"OFF\",";
      payload += deviceInfo + "}";
      
      mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
      String relayNum = (i < 10 ? "R0" : "R") + String(i);
      Serial.println("  ✅ " + relayNum + ": " + name + " (Switch)");
    }
    
    // LIGHT Discovery
    else if (type == "light") {
      String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/light/" + deviceId + "/config";
      String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/state";
      String commandTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/set";
      
      String payload = "{";
      payload += "\"name\":\"" + name + "\",";
      payload += "\"unique_id\":\"" + deviceId + "\",";
      payload += "\"object_id\":\"" + objectId + "\",";
      payload += "\"state_topic\":\"" + stateTopic + "\",";
      payload += "\"command_topic\":\"" + commandTopic + "\",";
      payload += "\"payload_on\":\"ON\",\"payload_off\":\"OFF\",";
      payload += "\"state_on\":\"ON\",\"state_off\":\"OFF\",";
      payload += deviceInfo + "}";
      
      mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
      String relayNum = (i < 10 ? "R0" : "R") + String(i);
      Serial.println("  ✅ " + relayNum + ": " + name + " (Light)");
    }
    
    // COVER Discovery (Rollladen/Jalousie)
    else if (type == "cover") {
      String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/cover/" + deviceId + "/config";
      String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/state";
      String commandTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/set";
      
      String payload = "{";
      payload += "\"name\":\"" + name + "\",";
      payload += "\"unique_id\":\"" + deviceId + "\",";
      payload += "\"object_id\":\"" + objectId + "\",";
      payload += "\"command_topic\":\"" + commandTopic + "\",";
      payload += "\"state_topic\":\"" + stateTopic + "\",";
      payload += "\"payload_open\":\"ON\",";
      payload += "\"payload_close\":\"OFF\",";
      payload += "\"payload_stop\":\"STOP\",";
      payload += "\"state_open\":\"ON\",";
      payload += "\"state_closed\":\"OFF\",";
      payload += "\"device_class\":\"blind\",";
      payload += deviceInfo + "}";
      
      mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
      String relayNum = (i < 10 ? "R0" : "R") + String(i);
      Serial.println("  ✅ " + relayNum + ": " + name + " (Cover)");
    }
    
    // FAN Discovery
    else if (type == "fan") {
      String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/fan/" + deviceId + "/config";
      String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/state";
      String commandTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/set";
      
      String payload = "{";
      payload += "\"name\":\"" + name + "\",";
      payload += "\"unique_id\":\"" + deviceId + "\",";
      payload += "\"object_id\":\"" + objectId + "\",";
      payload += "\"state_topic\":\"" + stateTopic + "\",";
      payload += "\"command_topic\":\"" + commandTopic + "\",";
      payload += "\"payload_on\":\"ON\",\"payload_off\":\"OFF\",";
      payload += "\"state_on\":\"ON\",\"state_off\":\"OFF\",";
      payload += deviceInfo + "}";
      
      mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
      String relayNum = (i < 10 ? "R0" : "R") + String(i);
      Serial.println("  ✅ " + relayNum + ": " + name + " (Fan)");
    }
    
    delay(50);  // Kleine Verzögerung zwischen Messages
  }
  
  // MPR121 Touch Enable/Disable Switch
  String mpr121DeviceId = "wt32kg_mpr121_enable";
  String mpr121Name = "MPR121 Touch-Eingabe";
  String mpr121DiscoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/switch/" + mpr121DeviceId + "/config";
  String mpr121StateTopic = String(MQTT_BASE_TOPIC) + "/mpr121/state";
  String mpr121CommandTopic = String(MQTT_BASE_TOPIC) + "/mpr121/set";
  
  String mpr121Payload = "{";
  mpr121Payload += "\"name\":\"" + mpr121Name + "\",";
  mpr121Payload += "\"unique_id\":\"" + mpr121DeviceId + "\",";
  mpr121Payload += "\"object_id\":\"mpr121_enable\",";
  mpr121Payload += "\"state_topic\":\"" + mpr121StateTopic + "\",";
  mpr121Payload += "\"command_topic\":\"" + mpr121CommandTopic + "\",";
  mpr121Payload += "\"payload_on\":\"ON\",";
  mpr121Payload += "\"payload_off\":\"OFF\",";
  mpr121Payload += "\"state_on\":\"ON\",";
  mpr121Payload += "\"state_off\":\"OFF\",";
  mpr121Payload += "\"icon\":\"mdi:gesture-tap\",";
  mpr121Payload += deviceInfo;
  mpr121Payload += "}";
  
  mqttClient.publish(mpr121DiscoveryTopic.c_str(), mpr121Payload.c_str(), true);
  Serial.println("  ✅ MPR121 Touch Enable Switch: " + mpr121Name);
  
  // ======================================================
  // COVER DISCOVERY (Virtuelle Cover-Devices)
  // ======================================================
  for (int i = 0; i < 2; i++) {
    if (!coverHAEnabled[i]) {
      Serial.println("  ⏭️ Cover " + String(i) + " übersprungen (deaktiviert)");
      continue;
    }
    
    String coverDeviceId = "wt32kg_cover_" + String(i);
    String coverName = coverNames[i];
    String coverObjectId = "cover_" + String(i);
    
    String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/cover/" + coverDeviceId + "/config";
    String stateTopic = String(MQTT_BASE_TOPIC) + "/cover/" + String(i) + "/state";
    String commandTopic = String(MQTT_BASE_TOPIC) + "/cover/" + String(i) + "/set";
    
    String payload = "{";
    payload += "\"name\":\"" + coverName + "\",";
    payload += "\"unique_id\":\"" + coverDeviceId + "\",";
    payload += "\"object_id\":\"" + coverObjectId + "\",";
    payload += "\"command_topic\":\"" + commandTopic + "\",";
    payload += "\"state_topic\":\"" + stateTopic + "\",";
    payload += "\"payload_open\":\"OPEN\",";
    payload += "\"payload_close\":\"CLOSE\",";
    payload += "\"payload_stop\":\"STOP\",";
    payload += "\"state_opening\":\"opening\",";
    payload += "\"state_closing\":\"closing\",";
    payload += "\"state_stopped\":\"stopped\",";
    payload += "\"state_open\":\"open\",";
    payload += "\"state_closed\":\"closed\",";
    payload += "\"optimistic\":false,";
    payload += "\"device_class\":\"blind\",";
    payload += deviceInfo + "}";
    
    mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
    Serial.println("  ✅ Cover " + String(i) + ": " + coverName + " (Cover Device)");
    delay(50);
  }
  
  // ======================================================
  // BINARY SENSOR DISCOVERY (MCP23017 Inputs)
  // ======================================================
  for (int i = 0; i < 16; i++) {
    // Skip wenn nicht enabled
    if (!inputHAEnabled[i]) {
      Serial.println("  ⏭️ IN" + String(i) + " übersprungen (deaktiviert)");
      continue;
    }
    
    String type = inputHAType[i];
    if (type == "none") {
      Serial.println("  ⏭️ IN" + String(i) + " übersprungen (type=none)");
      continue;
    }
    
    String inputDeviceId = "wt32kg_input_" + String(i);
    String inputName = inputNames[i] + " (IN" + (i < 10 ? "0" : "") + String(i) + ")";
    String inputObjectId = "input_" + String(i);
    
    String discoveryTopic = String(MQTT_DISCOVERY_PREFIX) + "/binary_sensor/" + inputDeviceId + "/config";
    String stateTopic = String(MQTT_BASE_TOPIC) + "/input/" + String(i) + "/state";
    
    String payload = "{";
    payload += "\"name\":\"" + inputName + "\",";
    payload += "\"unique_id\":\"" + inputDeviceId + "\",";
    payload += "\"object_id\":\"" + inputObjectId + "\",";
    payload += "\"state_topic\":\"" + stateTopic + "\",";
    payload += "\"payload_on\":\"ON\",";
    payload += "\"payload_off\":\"OFF\",";
    
    // Device Class setzen (falls nicht "switch")
    if (type != "switch") {
      payload += "\"device_class\":\"" + type + "\",";
    }
    
    payload += deviceInfo + "}";
    
    mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
    String inputNum = (i < 10 ? "IN0" : "IN") + String(i);
    Serial.println("  ✅ " + inputNum + ": " + inputNames[i] + " (Binary Sensor - " + type + ")");
    delay(50);
  }
  
  Serial.println("📡 Discovery abgeschlossen!");
}

void publishRelayState(int relayIndex) {
  if (!mqttClient.connected() || !mqttConfig.enabled) return;
  if (relayIndex < 0 || relayIndex >= 24) return;
  
  // Kein Throttling mehr - sofortiges State-Update für korrekten Sync
  // (MQTT retained messages sind effizient genug)
  
  String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(relayIndex) + "/state";
  String statePayload = relayState[relayIndex] ? "ON" : "OFF";
  
  mqttClient.publish(stateTopic.c_str(), statePayload.c_str(), true);  // retained
}

void publishMPR121State() {
  if (!mqttClient.connected() || !mqttConfig.enabled) return;
  
  String stateTopic = String(MQTT_BASE_TOPIC) + "/mpr121/state";
  String statePayload = mpr121Enabled ? "ON" : "OFF";
  
  mqttClient.publish(stateTopic.c_str(), statePayload.c_str(), true);  // retained
}

void publishCoverState(int coverIndex) {
  if (!mqttClient.connected() || !mqttConfig.enabled) return;
  if (coverIndex < 0 || coverIndex >= 2) return;
  
  String stateTopic = String(MQTT_BASE_TOPIC) + "/cover/" + String(coverIndex) + "/state";
  
  // State ermitteln aus den beiden Relais
  int openRelay = coverRelayPairs[coverIndex][0];
  int closeRelay = coverRelayPairs[coverIndex][1];
  
  String statePayload;
  if (relayState[openRelay] == 1 && relayState[closeRelay] == 0) {
    statePayload = "opening";  // Fährt hoch/öffnet
  } else if (relayState[openRelay] == 0 && relayState[closeRelay] == 1) {
    statePayload = "closing";  // Fährt runter/schließt
  } else {
    statePayload = "stopped";   // Beide aus oder beide an (Fehlerfall)
  }
  
  mqttClient.publish(stateTopic.c_str(), statePayload.c_str(), true);  // retained
}

void publishAllStates() {
  if (!mqttClient.connected() || !mqttConfig.enabled) return;
  
  for (int i = 0; i < 24; i++) {
    String stateTopic = String(MQTT_BASE_TOPIC) + "/relay/" + String(i) + "/state";
    String statePayload = relayState[i] ? "ON" : "OFF";
    mqttClient.publish(stateTopic.c_str(), statePayload.c_str(), true);
    delay(20);  // Kleine Verzögerung
  }
  
  // Cover States publishen
  for (int i = 0; i < 2; i++) {
    publishCoverState(i);
    delay(20);
  }
  
  // Input States publishen
  for (int i = 0; i < 16; i++) {
    publishInputState(i);
    delay(20);
  }
  
  // MPR121 State publishen
  publishMPR121State();
}

void publishInputState(int inputIndex) {
  if (!mqttClient.connected() || !mqttConfig.enabled) return;
  if (inputIndex < 0 || inputIndex >= 16) return;
  
  String stateTopic = String(MQTT_BASE_TOPIC) + "/input/" + String(inputIndex) + "/state";
  String statePayload = inputState[inputIndex] ? "ON" : "OFF";  // ON = OPEN, OFF = CLOSED
  
  mqttClient.publish(stateTopic.c_str(), statePayload.c_str(), true);  // retained
}

// ======================================================
// SETUP
// ======================================================
void setup() {
  Serial.begin(115200);
  Serial.println("=== WT32-KG Smart Home Controller ===");
  
  // Boot-Zeitstempel setzen für Kreuzschaltungs-Verzögerung
  bootTime = millis();
  
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

  // MCP23017 Input Expander
  // ============================================================
  Serial.println("\n=== MCP23017 Input Expander Initialisierung ===");
  if (!mcpIn.begin_I2C(0x20)) {
    Serial.println("❌ ERROR: MCP23017 nicht gefunden auf Adresse 0x20!");
  } else {
    Serial.println("✅ MCP23017 initialisiert auf Adresse 0x20");
    
    // Debugging: Erste Lese zu sehen ob I2C funktioniert
    uint16_t testRead = mcpIn.readGPIOAB();
    Serial.print("🔍 Test Reading GPIO: 0x");
    Serial.println(testRead, HEX);
    
    // GPIO39 als Interrupt-Pin konfigurieren (INPUT_PULLUP für Active-Low)
    pinMode(MCP23017_IRQ_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(MCP23017_IRQ_PIN), mcpISR, FALLING);
    Serial.println("✅ GPIO39 Interrupt-Handler registriert (FALLING edge)");
  }
  Serial.println("=========================================\n");

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

  // MCP23017 Pin-Konfiguration
  // ================================================================
  Serial.println("=== MCP23017 Pin-Konfiguration ===");
  
  // Port A (GPA0-GPA7): INPUT mit aktivierten Pull-ups
  for (int i = 0; i < 8; i++) {
    mcpIn.pinMode(i, INPUT);
    // 🔴 WICHTIG: GPPU = Internal 100kΩ Pull-up MANUELL via Register setzen
    // Die Adafruit-Lib könnte INPUT_PULLUP nicht korrekt setzen!
  }
  Serial.println("✅ Port A (GPA0-GPA7): INPUT konfiguriert");
  
  // Port B (GPB0-GPB7): INPUT (Reserve)
  for (int i = 8; i < 16; i++) {
    mcpIn.pinMode(i, INPUT);
  }
  Serial.println("✅ Port B (GPB0-GPB7): INPUT (Reserve) konfiguriert");
  
  // 🔴 KRITISCH: Pull-ups via MCP23017 Registers aktivieren (nicht via Adafruit-Lib!)
  // GPPU Register (0x0C für Port A, 0x0D für Port B): 1 = Pull-up ON
  Wire.beginTransmission(0x20);
  Wire.write(0x0C);  // GPPUA Register (ziehen bei GPA0-GPA7)
  Wire.write(0xFF);  // Alle 8 Bits auf 1 = Pullups ON
  Wire.endTransmission();
  
  Wire.beginTransmission(0x20);
  Wire.write(0x0D);  // GPPUB Register (ziehen bei GPB0-GPB7)
  Wire.write(0xFF);  // Alle 8 Bits auf 1 = Pullups ON
  Wire.endTransmission();
  Serial.println("✅ GPPU Register (0x0C, 0x0D): Pull-ups aktiviert");
  
  // 🔴 KRITISCH: Interrupt-Register konfigurieren (Change Detection)
  // GPINTEN Register (0x04 für Port A, 0x05 für Port B): Enable Interrupts
  Wire.beginTransmission(0x20);
  Wire.write(0x04);  // GPINTENA Register
  Wire.write(0xFF);  // Alle 8 Pins GPA0-GPA7 interrupt-enabled
  Wire.endTransmission();
  
  Wire.beginTransmission(0x20);
  Wire.write(0x05);  // GPINTENB Register
  Wire.write(0x00);  // Port B: keine Interrupts (Reserve)
  Wire.endTransmission();
  
  // INTCON Register (0x08/0x09): 0 = compare to previous value (change detection)
  Wire.beginTransmission(0x20);
  Wire.write(0x08);  // INTCONA Register
  Wire.write(0x00);  // 0 = Trigger bei Änderung (nicht compare to DEFVAL)
  Wire.endTransmission();
  Serial.println("✅ Interrupt Register (GPINTEN, INTCON): Konfiguriert für Change Detection");
  
  // Einmal durchlesen um initial Clear zu machen
  uint16_t initialRead = mcpIn.readGPIOAB();
  Serial.print("📊 Initial GPIO Reading: 0x");
  Serial.println(initialRead, HEX);
  
  // KRITISCH: inputState[] mit echten Werten füllen
  uint8_t portA = initialRead & 0xFF;
  uint8_t portB = (initialRead >> 8) & 0xFF;
  for (int i = 0; i < 8; i++) {
    inputState[i] = (portA >> i) & 1;
    inputState[i + 8] = (portB >> i) & 1;
  }
  
  // KRITISCH: Kreuzschaltungs-Zustände synchronisieren BEVOR Handler laufen
  kreuzstateEG = inputState[KREUZ_EG1] | (inputState[KREUZ_EG2] << 1);
  kreuzstateKG = inputState[KREUZ_KG1] | (inputState[KREUZ_KG2] << 1) | (inputState[KREUZ_KG3] << 2);
  Serial.printf("✅ Kreuzschaltungen initialisiert: EG=0x%02X, KG=0x%02X\n", kreuzstateEG, kreuzstateKG);
  
  // KRITISCH: IR-Switch Zustände synchronisieren (verhindert Boot-Toggle)
  lastIRSwitchLeft = inputState[IR_SWITCH_KITCHEN_LEFT];
  lastIRSwitchRight = inputState[IR_SWITCH_KITCHEN_RIGHT];
  Serial.printf("✅ IR-Switches initialisiert: Left=%d, Right=%d\n", lastIRSwitchLeft, lastIRSwitchRight);
  Serial.println("=========================================\n");

  // Relais als OUTPUT und alle AUS (LOW für nicht-invertierte Relais)
  // KRITISCH: Output-Register ZUERST auf LOW setzen, BEVOR pinMode(OUTPUT)
  // Dies verhindert Glitches beim Umschalten von INPUT auf OUTPUT
  Serial.println("⚡ Initialisiere Relais (glitch-frei)...");
  
  // Schritt 1: Output-Register auf LOW setzen (während Pins noch INPUT sind)
  for (int i = 0; i < 8; i++) {
    pcaRel1.digitalWrite(i, LOW);
    pcaRel2.digitalWrite(i, LOW);
    pcaRel3.digitalWrite(i, LOW);
  }
  delay(50);  // Warten bis Output-Register gesetzt sind
  
  // Schritt 2: Pins auf OUTPUT umschalten (jetzt bereits auf LOW)
  for (int i = 0; i < 8; i++) {
    pcaRel1.pinMode(i, OUTPUT);
    pcaRel2.pinMode(i, OUTPUT);
    pcaRel3.pinMode(i, OUTPUT);
  }
  delay(10);
  
  // Relay State Array initialisieren
  for (int i = 0; i < 24; i++) {
    relayState[i] = 0;
  }
  
  // Schritt 3: Sicherheits-Check - nochmals explizit LOW
  delay(10);
  pcaRel1.digitalWrite(6, LOW);  // R06 - KG Flurlampe
  pcaRel1.digitalWrite(7, LOW);  // R07 - Küchenarbeitslampe
  pcaRel2.digitalWrite(1, LOW);  // R09 - EG Flurlampe
  Serial.println("✅ Alle Relais initialisiert (R06 & R09 explizit AUS)");

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
  server.on("/info", handleInfo);
  server.on("/edit", handleEdit);
  server.on("/toggle", handleToggle);
  server.on("/inputs", handleInputs);
  server.on("/led", handleLEDDimmer);
  server.on("/kronleuchter", handleACDimmer);
  server.on("/savename", handleSaveName);
  server.on("/savecover", handleSaveCover);
  server.on("/saveinput", handleSaveInput);
  server.on("/cover", handleCover);
  server.on("/mqtt", handleMQTT);
  server.on("/savemqtt", handleSaveMQTT);
  server.on("/restart", handleRestart);
  server.on("/toggle_mpr121", handleToggleMPR121);
  server.on("/pairing", handlePairing);
  server.on("/client", handleClientDetail);
  server.on("/remove_client", handleRemoveClient);
  
  // Relay-Namen aus NVRAM laden
  Serial.println("\n=== Relay Namen laden ===");
  loadRelayNames();
  
  // HA Device-Konfiguration laden
  loadRelayHAConfig();
  
  // Cover-Konfiguration laden
  loadCoverConfig();
  
  // Input-Konfiguration laden
  loadInputConfig();
  
  // MPR121 State laden
  loadMPR121State();
  
  // MQTT Config laden und verbinden
  Serial.println("\n=== MQTT laden ===");
  loadMQTTConfig();
  
  if (mqttConfig.enabled) {
    mqttClient.setServer(mqttConfig.broker, mqttConfig.port);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(512);  // Größerer Buffer für Discovery-Messages
    mqttConnect();  // Erste Verbindung
  }
  
  server.begin();
  Serial.println("Webserver gestartet");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
  ArduinoOTA.handle();  // OTA Updates verarbeiten
  server.handleClient();
  
  // MQTT Client verarbeiten
  if (mqttConfig.enabled) {
    if (!mqttClient.connected()) {
      mqttConnect();  // Auto-Reconnect
    } else {
      mqttClient.loop();  // MQTT Messages verarbeiten
    }
  }
  
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
  
  // === MCP23017 INTERRUPT-GESTEUERTE VERARBEITUNG ===
  if (mcpInterruptFlag) {
    mcpInterruptFlag = false;  // Flag zurücksetzen
    Serial.println("⚡ MCP23017 Interrupt ausgelöst!");
    
    // GPIO-Status lesen (cleared automatisch den Interrupt)
    uint16_t gpio_ab = mcpIn.readGPIOAB();
    uint8_t portA = gpio_ab & 0xFF;
    uint8_t portB = (gpio_ab >> 8) & 0xFF;
    
    // Debug: Welche Pins haben sich geändert
    Serial.print("📊 GPIO: Port A=0x");
    Serial.print(portA, HEX);
    Serial.print(" Port B=0x");
    Serial.println(portB, HEX);
    
    // Eingänge in inputState[] speichern (für Web-UI)
    // State-Change Detection für MQTT Publishing
    static uint8_t lastInputState[16] = {0};
    bool stateChanged = false;
    
    for (int i = 0; i < 8; i++) {
      uint8_t newStateA = (portA >> i) & 1;
      uint8_t newStateB = (portB >> i) & 1;
      
      // Port A (IN00-IN07)
      if (inputState[i] != newStateA) {
        inputState[i] = newStateA;
        lastInputState[i] = newStateA;
        publishInputState(i);  // MQTT State publishen
        stateChanged = true;
      }
      
      // Port B (IN08-IN15)
      if (inputState[i + 8] != newStateB) {
        inputState[i + 8] = newStateB;
        lastInputState[i + 8] = newStateB;
        publishInputState(i + 8);  // MQTT State publishen
        stateChanged = true;
      }
    }
    
    if (stateChanged) {
      Serial.println("📡 Input States published via MQTT");
    }
    
    // Schalter-Logik verarbeiten (nur nach Boot-Verzögerung)
    if (millis() - bootTime > bootDelay) {
      handleIRSwitchKitchen();
      handleKreuzschaltungEG();
      handleKreuzschaltungKG();
    } else {
      Serial.println("⏳ Boot-Verzögerung aktiv, Schaltlogik übersprungen");
    }
  }
  
  // === MPR121 INTERRUPT-GESTEUERTE VERARBEITUNG (VORBEREITET) ===
  // WENN AKTIVIERT: Uncomment die folgenden Zeilen und entferne Polling
  // if (mpr121InterruptFlag) {
  //   mpr121InterruptFlag = false;  // Flag zurücksetzen
  //   Serial.println("⚡ MPR121 Interrupt ausgelöst!");
  //   handleTouchEvents();  // Touch-Events verarbeiten
  // }
  
  // Touch Events verarbeiten (Polling-basiert, alle 100ms)
  static unsigned long lastI2CRead = 0;
  const unsigned long i2cReadInterval = 100;
  if (mpr121Enabled && millis() - lastI2CRead >= i2cReadInterval) {
    lastI2CRead = millis();
    handleTouchEvents();
  }

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
    publishRelayState(0);  // MQTT State publishen
    publishRelayState(1);  // MQTT State publishen
    publishCoverState(0);  // Cover-State aktualisieren
  }
  if (tuerrolloTimer > 0 && millis() - tuerrolloTimer > (rolloActiveTime * 5)) {
    Serial.println("SICHERHEITS-STOPP Türrollo nach 5 Minuten");
    relayState[2] = 0; relayState[3] = 0;
    pcaRel1.digitalWrite(2, LOW);
    pcaRel1.digitalWrite(3, LOW);
    tuerrolloTimer = 0;
    publishRelayState(2);  // MQTT State publishen
    publishRelayState(3);  // MQTT State publishen
    publishCoverState(1);  // Cover-State aktualisieren
  }

  delay(10);  // Reduziert von 50ms → schnellere Reaktionszeit
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
  // Tasmota Dark Mode Style
  html += "body{font-family:Verdana,Arial,sans-serif;margin:0;padding:0;background:#1a1a1a;color:#eee;font-size:12px;}";
  html += ".container{max-width:600px;margin:0 auto;background:#252525;overflow:hidden;}";
  html += "h2{color:#eee;text-align:center;padding:12px 10px;font-size:16px;margin:0;background:#1fa3ec;font-weight:normal;}";
  html += "h3{color:#eee;margin:12px 8px 8px 8px;border-left:2px solid #1fa3ec;padding-left:8px;font-size:14px;font-weight:normal;}";
  
  // Tab-Navigation (Tasmota-Style)
  html += ".tabs{display:flex;background:#1a1a1a;margin:0;padding:0;list-style:none;border-bottom:1px solid #333;}";
  html += ".tab{flex:1;text-align:center;padding:10px 5px;cursor:pointer;background:#1a1a1a;border:none;font-size:12px;color:#aaa;text-decoration:none;display:block;transition:background 0.2s;}";
  html += ".tab:hover{background:#333;}";
  html += ".tab.active{background:#1fa3ec;color:#fff;}";
  
  // Content Area
  html += ".content{padding:8px;}";
  
  // Button Styles (Tasmota-ähnlich)
  html += ".btn{padding:6px 10px;margin:2px;border:1px solid #333;border-radius:3px;cursor:pointer;font-size:12px;transition:all 0.2s;min-width:200px;display:inline-block;text-align:center;text-decoration:none;background:#444;color:#eee;}";
  html += ".btn:hover{background:#555;}";
  html += ".btn:active{transform:scale(0.98);}";
  html += ".btn-on{background:#47c266;color:#fff;border-color:#47c266;}";
  html += ".btn-on:hover{background:#5cd67a;}";
  html += ".btn-off{background:#d43535;color:#fff;border-color:#d43535;}";
  html += ".btn-off:hover{background:#e84848;}";
  html += ".btn-rollo{background:#ff9800;color:#fff;border-color:#ff9800;}";
  html += ".btn-rollo:hover{background:#ffaa33;}";
  html += ".btn-neutral{background:#607D8B;color:#fff;border-color:#607D8B;}";
  html += ".btn-neutral:hover{background:#738fa0;}";
  
  // Layout Styles
  html += ".grid{display:grid;grid-template-columns:1fr;gap:8px;margin:8px 0;}";
  html += ".btn-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin:8px 0;}";
  html += ".card{background:#2a2a2a;padding:8px;border-radius:3px;border:1px solid #333;margin:8px 0;}";
  html += ".rollo-group{display:flex;flex-wrap:wrap;gap:3px;align-items:center;margin:6px 0;}";
  html += ".rollo-label{min-width:100px;font-size:12px;color:#aaa;}";
  html += ".rollo-status{margin-left:10px;color:#666;font-size:12px;}";
  
  // Table Styles
  html += "table{border-collapse:collapse;width:100%;margin:8px 0;font-size:11px;}";
  html += "td,th{border:1px solid #333;padding:4px;text-align:center;}";
  html += "th{background:#1a1a1a;font-weight:bold;color:#999;}";
  html += "td{background:#2a2a2a;}";
  
  // Input Range Slider
  html += "input[type='range']{-webkit-appearance:none;width:100%;height:6px;border-radius:3px;background:#444;outline:none;margin:10px 0;}";
  html += "input[type='range']::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:16px;height:16px;border-radius:50%;background:#1fa3ec;cursor:pointer;}";
  html += "input[type='range']::-moz-range-thumb{width:16px;height:16px;border-radius:50%;background:#1fa3ec;cursor:pointer;border:none;}";
  
  // Labels
  html += "label{color:#aaa;font-size:12px;}";
  html += "label b{color:#eee;}";
  html += "</style>";
  
  // JavaScript für AJAX-Requests ohne Seiten-Reload
  html += "<script>";
  html += "function toggleRelay(r,btnElement){";
  html += "console.log('Toggle relay '+r);";
  // OPTIMISTIC UI: Button sofort umschalten für schnelles Feedback
  html += "var btn=btnElement;";
  html += "var wasOn=btn.classList.contains('btn-on')||btn.classList.contains('btn-neutral');";
  html += "btn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  // Sofort neuen Zustand anzeigen (optimistic)
  html += "if(wasOn){";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-rollo');}else{btn.classList.add('btn-off');}";
  html += "}else{";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-neutral');}else{btn.classList.add('btn-on');}";
  html += "}";
  // Rollo-Pair optimistisch updaten (z.B. bei R0->R1 oder R2->R3)
  html += "if(r>=0&&r<=3){";
  html += "var pairIdx=(r==0)?1:(r==1)?0:(r==2)?3:2;";
  html += "var pairBtn=document.querySelector('[onclick*=\"toggleRelay('+pairIdx+',this)\"]');";
  html += "if(pairBtn&&!wasOn){";  // Nur wenn wir gerade starten (nicht stoppen)
  html += "pairBtn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  html += "pairBtn.classList.add('btn-rollo');";
  html += "if(pairIdx==0){pairBtn.textContent='▲ Hoch';}";
  html += "if(pairIdx==1){pairBtn.textContent='▼ Runter';}";
  html += "if(pairIdx==2){pairBtn.textContent='▲ Hoch';}";
  html += "if(pairIdx==3){pairBtn.textContent='▼ Runter';}";
  html += "}";
  html += "}";
  // Button-Text optimistisch updaten
  html += "if(r==0){btn.textContent=wasOn?'▲ Hoch':'⏹️ Stopp';}";
  html += "if(r==1){btn.textContent=wasOn?'▼ Runter':'⏹️ Stopp';}";
  html += "if(r==2){btn.textContent=wasOn?'▲ Hoch':'⏹️ Stopp';}";
  html += "if(r==3){btn.textContent=wasOn?'▼ Runter':'⏹️ Stopp';}";
  // Server-Request im Hintergrund
  html += "fetch('/toggle?r='+r).then(function(response){";
  html += "return response.json();";
  html += "}).then(function(data){";
  html += "console.log('Server confirmed state:'+data.state);";
  // Server-Status mit UI abgleichen (falls abweichend korrigieren)
  html += "btn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  html += "if(data.state==1){";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-neutral');}";
  html += "else{btn.classList.add('btn-on');}";
  html += "}else{";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-rollo');}";
  html += "else{btn.classList.add('btn-off');}";
  html += "}";
  // Button-Text für Rollos updaten
  html += "if(r==0){btn.textContent=data.state==1?'⏹️ Stopp (R00)':'▲ Hoch (R00)';}";
  html += "if(r==1){btn.textContent=data.state==1?'⏹️ Stopp (R01)':'▼ Runter (R01)';}";
  html += "if(r==2){btn.textContent=data.state==1?'⏹️ Stopp (R02)':'▲ Hoch (R02)';}";
  html += "if(r==3){btn.textContent=data.state==1?'⏹️ Stopp (R03)':'▼ Runter (R03)';}";
  // Rollo-Pair: Anderen Button auch updaten (z.B. bei Fensterrollo R00<->R01)
  html += "if(data.pair!==undefined){";
  html += "var pairBtn=document.querySelector('[onclick*=\"toggleRelay('+data.pair+',this)\"]');";
  html += "if(pairBtn){";
  html += "pairBtn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  html += "if(data.pairState==1){pairBtn.classList.add('btn-neutral');}";
  html += "else{pairBtn.classList.add('btn-rollo');}";
  html += "if(data.pair==0){pairBtn.textContent=data.pairState==1?'⏹️ Stopp (R00)':'▲ Hoch (R00)';}";
  html += "if(data.pair==1){pairBtn.textContent=data.pairState==1?'⏹️ Stopp (R01)':'▼ Runter (R01)';}";
  html += "if(data.pair==2){pairBtn.textContent=data.pairState==1?'⏹️ Stopp (R02)':'▲ Hoch (R02)';}";
  html += "if(data.pair==3){pairBtn.textContent=data.pairState==1?'⏹️ Stopp (R03)':'▼ Runter (R03)';}";
  html += "}";
  html += "}";
  // Rollo-Status-Anzeige updaten
  html += "if(r==0||r==1){";
  html += "var statusSpan=document.getElementById('fenster-status');";
  html += "if(statusSpan){";
  html += "if(data.state==1&&r==0){statusSpan.textContent=' (Fährt hoch)';}";
  html += "else if(data.state==1&&r==1){statusSpan.textContent=' (Fährt runter)';}";
  html += "else if(data.pairState==0){statusSpan.textContent=' (Stopp)';}";
  html += "}";
  html += "}";
  html += "if(r==2||r==3){";
  html += "var statusSpan=document.getElementById('tuer-status');";
  html += "if(statusSpan){";
  html += "if(data.state==1&&r==2){statusSpan.textContent=' (Fährt hoch)';}";
  html += "else if(data.state==1&&r==3){statusSpan.textContent=' (Fährt runter)';}";
  html += "else if(data.pairState==0){statusSpan.textContent=' (Stopp)';}";
  html += "}";
  html += "}";
  // Button-Text für normale Relais updaten (4-23)
  html += "if(r>=4){";
  html += "var text=btn.textContent;";
  html += "if(text.includes(':')){";
  html += "var parts=text.split(':');";
  html += "parts[parts.length-1]=data.state==1?' EIN':' AUS';";
  html += "btn.textContent=parts.join(':');";
  html += "}";
  html += "}";
  html += "}).catch(function(error){";
  html += "console.error('Toggle error:',error);";
  // Bei Fehler Button zurückschalten
  html += "btn.classList.remove('btn-on','btn-off','btn-rollo','btn-neutral');";
  html += "if(wasOn){";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-neutral');}else{btn.classList.add('btn-on');}";
  html += "}else{";
  html += "if(r==0||r==1||r==2||r==3){btn.classList.add('btn-rollo');}else{btn.classList.add('btn-off');}";
  html += "}";
  html += "});";
  html += "}";
  // JavaScript für Auto-Refresh der Eingangszustände
  html += "function updateInputs(){";
  html += "fetch('/inputs').then(function(response){";
  html += "return response.json();";
  html += "}).then(function(data){";
  html += "for(var i=0;i<16;i++){";
  html += "var cell=document.getElementById('input-'+i+'-status');";
  html += "if(cell){";
  html += "var state=data.inputs[i];";
  html += "cell.textContent=state?'OPEN':'CLOSED';";
  html += "cell.style.backgroundColor=state?'lightcoral':'lightgreen';";
  html += "}";
  html += "}";
  html += "}).catch(function(error){";
  html += "console.error('Input update error:',error);";
  html += "});";
  html += "}";
  html += "setInterval(updateInputs,500);";  // Update alle 500ms
  
  // JavaScript für Cover-Steuerung (OPEN/CLOSE/STOP)
  html += "function coverAction(coverIndex, action, btnElement) {";
  html += "  console.log('Cover ' + coverIndex + ' Action: ' + action);";
  html += "  fetch('/cover?idx=' + coverIndex + '&action=' + action)";
  html += "  .then(function(response) { return response.json(); })";
  html += "  .then(function(data) {";
  html += "    console.log('Cover response:', data);";
  html += "    location.reload();";
  html += "  })";
  html += "  .catch(function(error) {";
  html += "    console.error('Cover error:', error);";
  html += "  });";
  html += "}";
  
  html += "</script>";
  
  html += "</head><body>";
  
  html += "<div class='container'>";
  html += "<h2>🏠 WT32-KG Smart Home</h2>";
  
  // Tab-Navigation
  html += "<div class='tabs'>";
  html += "<a href='/home' class='tab" + String(activeTab == "home" ? " active" : "") + "'>🏠 Home</a>";
  html += "<a href='/espnow' class='tab" + String(activeTab == "espnow" ? " active" : "") + "'>📡 ESP-NOW</a>";
  html += "<a href='/mqtt' class='tab" + String(activeTab == "mqtt" ? " active" : "") + "'>📨 MQTT</a>";
  html += "<a href='/info' class='tab" + String(activeTab == "info" ? " active" : "") + "'>ℹ️ Info</a>";
  html += "<a href='/edit' class='tab" + String(activeTab == "edit" ? " active" : "") + "'>✏️ Edit</a>";
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
  
  // MPR121 Touch Enable/Disable
  html += "<h3>📱 Touch-Eingabe (MPR121)</h3>";
  html += "<div class='card'>";
  String mpr121BtnClass = mpr121Enabled ? "btn-on" : "btn-off";
  String mpr121BtnText = mpr121Enabled ? "🔵 Aktuell AKTIV - Klicken zum Deaktivieren" : "🔴 Aktuell DEAKTIVIERT - Klicken zum Aktivieren";
  html += "<button onclick='toggleMPR121(this)' class='btn " + mpr121BtnClass + "' style='min-width:280px;'>" + mpr121BtnText + "</button>";
  html += "<p style='font-size:11px;color:#aaa;margin:10px 0 0 0;'>";
  html += "🎯 Aktiviert/Deaktiviert die Auswertung der 3× MPR121 Touch-Sensoren<br>";
  html += "💡 Bei Problemen mit ungewollten Touch-Events deaktivieren";
  html += "</p>";
  html += "</div>";
  
  html += "<script>";
  html += "function toggleMPR121(btn) {";
  html += "  fetch('/toggle_mpr121')";
  html += "  .then(response => response.json())";
  html += "  .then(data => {";
  html += "    if (data.enabled) {";
  html += "      btn.className = 'btn btn-on';";
  html += "      btn.innerHTML = '🔵 Aktuell AKTIV - Klicken zum Deaktivieren';";
  html += "    } else {";
  html += "      btn.className = 'btn btn-off';";
  html += "      btn.innerHTML = '🔴 Aktuell DEAKTIVIERT - Klicken zum Aktivieren';";
  html += "    }";
  html += "  });";
  html += "}";
  html += "</script>";
  
  // LED Dimmer
  html += "<h3>💡 LED Dimmer (GPIO15 - HW-517)</h3>";
  html += "<div class='card'>";
  int brightnessPercent = (ledDimmerBrightness * 100) / 255;
  html += "<div style='margin:20px 0;'>";
  html += "<label for='ledSlider'>Helligkeit: <b>" + String(brightnessPercent) + "%</b></label><br>";
  html += "<input type='range' id='ledSlider' min='0' max='100' value='" + String(brightnessPercent) + "' ";
  html += "style='width:300px;' oninput='updateLED(this.value)'><br>";
  html += "<span style='font-size:12px;'>0%</span>";
  html += "<span style='float:right;font-size:12px;'>100%</span>";
  html += "</div>";
  html += "</div>";  // Close card
  
  html += "<script>";
  html += "function updateLED(value) {";
  html += "  document.querySelector('label b').textContent = value + '%';";
  html += "  fetch('/led?brightness=' + value);";
  html += "}";
  html += "</script>";
  
  // AC Dimmer (unabhängig von R11)
  html += "<h3>💡 AC Dimmer (GPIO2 - YYAC-3S)</h3>";
  html += "<div class='card'>";
  int acPercent = (acDimmerBrightness * 100) / 255;
  html += "<div style='margin:20px 0;'>";
  html += "<label for='kronleuchterSlider'>Helligkeit: <b>" + String(acPercent) + "%</b></label><br>";
  html += "<input type='range' id='kronleuchterSlider' min='0' max='100' value='" + String(acPercent) + "' ";
  html += "style='width:300px;' oninput='updateKronleuchter(this.value)'><br>";
  html += "<span style='font-size:12px;'>0%</span>";
  html += "<span style='float:right;font-size:12px;'>100%</span>";
  html += "</div>";
  html += "</div>";  // Close card
  
  html += "<script>";
  html += "function updateKronleuchter(value) {";
  html += "  document.querySelectorAll('label b')[1].textContent = value + '%';";
  html += "  fetch('/kronleuchter?brightness=' + value);";
  html += "}";
  html += "</script>";

  // Cover-Devices (Rollläden)
  html += "<h3>🪟 Rollläden</h3>";
  html += "<div class='card'>";
  
  // Cover 0: Fensterrollo (R00+R01)
  html += "<div style='margin:15px 0;padding:10px;background:#2a2a2a;border-radius:5px;border-left:3px solid #4caf50;'>";
  html += "<div style='margin-bottom:10px;font-weight:bold;color:#4caf50;'>" + coverNames[0] + "</div>";
  
  // 3 Buttons: OPEN, CLOSE, STOP
  html += "<div style='display:flex;gap:5px;margin-bottom:10px;'>";
  
  // OPEN Button
  bool cover0Opening = (relayState[0] == 1 && relayState[1] == 0);
  String openClass = cover0Opening ? "btn-on" : "btn-rollo";
  html += "<button onclick='coverAction(0, \"OPEN\", this)' class='btn " + openClass + "' style='flex:1;max-width:120px;'>";
  html += cover0Opening ? "▲ Fährt hoch..." : "▲ Öffnen";
  html += "</button>";
  
  // STOP Button
  html += "<button onclick='coverAction(0, \"STOP\", this)' class='btn btn-neutral' style='flex:1;max-width:120px;'>⏹️ Stopp</button>";
  
  // CLOSE Button
  bool cover0Closing = (relayState[0] == 0 && relayState[1] == 1);
  String closeClass = cover0Closing ? "btn-on" : "btn-rollo";
  html += "<button onclick='coverAction(0, \"CLOSE\", this)' class='btn " + closeClass + "' style='flex:1;max-width:120px;'>";
  html += cover0Closing ? "▼ Fährt runter..." : "▼ Schließen";
  html += "</button>";
  
  html += "</div>";
  
  // Status-Anzeige
  String cover0Status = "Gestoppt";
  if (cover0Opening) cover0Status = "Fährt hoch";
  else if (cover0Closing) cover0Status = "Fährt runter";
  html += "<div style='font-size:11px;color:#aaa;text-align:center;' id='cover0-status'>Status: " + cover0Status + "</div>";
  html += "</div>";
  
  // Cover 1: Türrollo (R02+R03)
  html += "<div style='margin:15px 0;padding:10px;background:#2a2a2a;border-radius:5px;border-left:3px solid #4caf50;'>";
  html += "<div style='margin-bottom:10px;font-weight:bold;color:#4caf50;'>" + coverNames[1] + "</div>";
  
  // 3 Buttons: OPEN, CLOSE, STOP
  html += "<div style='display:flex;gap:5px;margin-bottom:10px;'>";
  
  // OPEN Button
  bool cover1Opening = (relayState[2] == 1 && relayState[3] == 0);
  String openClass1 = cover1Opening ? "btn-on" : "btn-rollo";
  html += "<button onclick='coverAction(1, \"OPEN\", this)' class='btn " + openClass1 + "' style='flex:1;max-width:120px;'>";
  html += cover1Opening ? "▲ Fährt hoch..." : "▲ Öffnen";
  html += "</button>";
  
  // STOP Button
  html += "<button onclick='coverAction(1, \"STOP\", this)' class='btn btn-neutral' style='flex:1;max-width:120px;'>⏹️ Stopp</button>";
  
  // CLOSE Button
  bool cover1Closing = (relayState[2] == 0 && relayState[3] == 1);
  String closeClass1 = cover1Closing ? "btn-on" : "btn-rollo";
  html += "<button onclick='coverAction(1, \"CLOSE\", this)' class='btn " + closeClass1 + "' style='flex:1;max-width:120px;'>";
  html += cover1Closing ? "▼ Fährt runter..." : "▼ Schließen";
  html += "</button>";
  
  html += "</div>";
  
  // Status-Anzeige
  String cover1Status = "Gestoppt";
  if (cover1Opening) cover1Status = "Fährt hoch";
  else if (cover1Closing) cover1Status = "Fährt runter";
  html += "<div style='font-size:11px;color:#aaa;text-align:center;' id='cover1-status'>Status: " + cover1Status + "</div>";
  html += "</div>";
  
  html += "</div>";

  // Lampen & Relais - alle R04-R23 in 2-spaltigem Grid
  html += "<h3>💡 Lampen & Relais</h3>";
  html += "<div class='card'>";
  html += "<div class='btn-grid'>";
  
  // R04-R23 alle in einer Schleife
  for (int i = 4; i < 24; i++) {
    String relaisClass = relayState[i] ? "btn-on" : "btn-off";
    String relaisNum = (i < 10) ? "R0" + String(i) : "R" + String(i);
    String icon = "💡";
    String relaisStatus = "";
    
    // R11 Kronleuchter hat spezielles Format mit EIN/AUS
    if (i == 11) {
      icon = "⚡";
      relaisStatus = relayState[i] ? ": EIN" : ": AUS";
      html += "<button onclick='toggleRelay(" + String(i) + ",this)' class='btn " + relaisClass + "'>" + icon + " " + String(relayNames[i]) + " (" + relaisNum + ")" + relaisStatus + "</button>";
    } else {
      // Alle anderen Relais
      if (i >= 13) icon = "🔌"; // Freie Relais bekommen Stecker-Icon
      relaisStatus = relayState[i] ? ": EIN" : ": AUS";
      html += "<button onclick='toggleRelay(" + String(i) + ",this)' class='btn " + relaisClass + "'>" + icon + " " + String(relayNames[i]) + " (" + relaisNum + ")" + relaisStatus + "</button>";
    }
  }
  
  html += "</div>"; // btn-grid
  html += "</div>"; // card
  
  html += "</div>"; // content

  // Eingänge
  html += "<h3>📊 Digitale Eingänge</h3>";
  html += "<table><tr>";
  html += "<th>Eingang</th><th>Funktion</th><th>Status</th></tr>";
  
  for (int i = 0; i < 16; i++) {
    String status = inputState[i] ? "OPEN" : "CLOSED";
    String color = inputState[i] ? "lightcoral" : "lightgreen";
    html += "<tr>";
    html += "<td>IN" + String(i) + "</td>";
    html += "<td>" + inputNames[i] + "</td>";
    html += "<td id='input-" + String(i) + "-status' style='background-color:" + color + ";font-weight:bold;'>" + status + "</td>";
    html += "</tr>";
  }
  html += "</table>";
  
  // Temperatursensoren nach digitalen Eingängen
  html += getTemperatureHTML();
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== ESP-NOW Seite =====
void handleInfo() {
  String html = getHTMLHeader("info");
  
  // Netzwerk-Informationen
  html += "<h3>🌐 Netzwerk</h3>";
  html += "<table>";
  html += "<tr><th style='width:40%;text-align:left;'>Parameter</th><th style='text-align:left;'>Wert</th></tr>";
  
  if (ETH.linkUp()) {
    html += "<tr><td>Status</td><td style='color:#47c266;'>✅ Verbunden (Ethernet)</td></tr>";
    html += "<tr><td>IP-Adresse</td><td>" + ETH.localIP().toString() + "</td></tr>";
    html += "<tr><td>MAC-Adresse</td><td>" + ETH.macAddress() + "</td></tr>";
    html += "<tr><td>Hostname</td><td>" + String(HOSTNAME) + "</td></tr>";
    html += "<tr><td>Link Speed</td><td>" + String(ETH.linkSpeed()) + " Mbps</td></tr>";
    html += "<tr><td>Full Duplex</td><td>" + String(ETH.fullDuplex() ? "Ja" : "Nein") + "</td></tr>";
  } else {
    html += "<tr><td>Status</td><td style='color:#d43535;'>❌ Nicht verbunden</td></tr>";
  }
  
  // WiFi MAC für ESP-NOW
  html += "<tr><td>WiFi MAC (ESP-NOW)</td><td>" + WiFi.macAddress() + "</td></tr>";
  html += "</table>";
  
  // System-Informationen
  html += "<h3>💾 System</h3>";
  html += "<table>";
  html += "<tr><th style='text-align:left;'>Parameter</th><th style='text-align:left;'>Wert</th></tr>";
  html += "<tr><td style='text-align:left;'>Chip Modell</td><td style='text-align:left;'>ESP32 WT32-ETH01</td></tr>";
  html += "<tr><td style='text-align:left;'>Uptime</td><td style='text-align:left;'>" + String(millis() / 1000) + " Sekunden</td></tr>";
  html += "<tr><td style='text-align:left;'>Freier Heap</td><td style='text-align:left;'>" + String(ESP.getFreeHeap() / 1024.0, 2) + " KB</td></tr>";
  html += "<tr><td style='text-align:left;'>Firmware</td><td style='text-align:left;'>WT32-KG Controller v1.0</td></tr>";
  html += "</table>";
  
  // Restart Button
  html += "<div style='margin:20px 0;text-align:center;'>";
  html += "<button onclick='restartESP()' class='btn btn-neutral' style='min-width:200px;'>🔄 ESP32 neu starten</button>";
  html += "</div>";
  
  // JavaScript für Restart
  html += "<script>";
  html += "function restartESP() {";
  html += "  if (confirm('ESP32 wirklich neu starten?\\n\\nWebserver wird für ~10 Sekunden nicht erreichbar sein.')) {";
  html += "    fetch('/restart')";
  html += "    .then(function() {";
  html += "      alert('✅ ESP32 wird neu gestartet...\\n\\nBitte warten Sie 10 Sekunden.');";
  html += "      setTimeout(function() { location.reload(); }, 10000);";
  html += "    })";
  html += "    .catch(function(error) {";
  html += "      alert('❌ Fehler beim Restart: ' + error);";
  html += "    });";
  html += "  }";
  html += "}";
  html += "</script>";
  
  // GitHub Repository
  html += "<h3>📦 Repository & Dokumentation</h3>";
  html += "<div class='card' style='text-align:center;'>";
  html += "<div style='margin:15px 0;'>";
  html += "<a href='https://github.com/heimbastler/WT32_KG' target='_blank' class='btn btn-neutral' style='min-width:250px;'>";
  html += "🐙 GitHub Repository öffnen";
  html += "</a>";
  html += "</div>";
  
  // QR-Code zum Repository
  html += "<div style='margin:20px 0;'>";
  html += "<p style='color:#aaa;font-size:12px;margin-bottom:10px;'>📱 QR-Code scannen für Repo-Zugriff:</p>";
  html += "<img src='https://api.qrserver.com/v1/create-qr-code/?size=200x200&data=https://github.com/heimbastler/WT32_KG' ";
  html += "alt='QR Code GitHub Repo' style='border:2px solid #333;border-radius:8px;background:white;padding:10px;'>";
  html += "</div>";
  
  html += "<div style='margin-top:15px;'>";
  html += "<p style='font-size:11px;color:#888;'>";
  html += "📚 Dokumentation, Schaltpläne, Pin-Belegung & Updates";
  html += "</p>";
  html += "</div>";
  
  // Platzhalter für zukünftiges Pin-Belegungs-Bild
  // TODO: Pin-Belegungsdiagramm hier einfügen
  // html += "<div style='margin:20px 0;'>";
  // html += "<h4>🔌 Steckerbelegung</h4>";
  // html += "<img src='/pinout.png' alt='Pin Belegung' style='max-width:100%;border:2px solid #333;border-radius:8px;'>";
  // html += "</div>";
  
  html += "</div>";
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

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

// ===== Edit-Seite für Relay-Namen =====
void handleEdit() {
  String html = getHTMLHeader("edit");
  
  html += "<h3>✏️ Cover & Relay Konfiguration</h3>";
  html += "<div class='card'>";
  html += "<p style='font-size:12px;color:#aaa;margin:0 0 15px 0;'>";
  html += "Namen und HA-Status werden permanent im NVRAM gespeichert.<br>";
  html += "💡 <b>Cover</b> = virtuelle Rollläden (steuern je 2 physische Relais) | <b>Light</b> = Lampen | <b>Switch</b> = Schalter";
  html += "</p>";
  
  // ======================================================
  // COVER-EINTRÄGE (2 virtuelle Cover-Devices)
  // ======================================================
  html += "<h4 style='margin:20px 0 10px 0;color:#1fa3ec;border-bottom:1px solid #555;padding-bottom:5px;'>🪟 Cover-Devices</h4>";
  
  for (int i = 0; i < 2; i++) {
    String coverNum = "Cover " + String(i);
    String relayPair = (i == 0) ? "(R00+R01)" : "(R02+R03)";
    
    html += "<div style='margin:15px 0;padding:10px;background:#2a2a2a;border-radius:5px;border-left:3px solid #4caf50;'>";
    
    // Erste Zeile: Cover-Nummer & Name
    html += "<div style='display:flex;align-items:center;gap:10px;margin-bottom:8px;'>";
    html += "<label style='min-width:100px;font-weight:bold;color:#4caf50;'>" + coverNum + " " + relayPair + ":</label>";
    html += "<input type='text' id='covername" + String(i) + "' value='" + coverNames[i] + "' ";
    html += "style='flex:1;padding:6px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
    html += "maxlength='30'>";
    html += "</div>";
    
    // Zweite Zeile: HA Enable Checkbox
    html += "<div style='display:flex;align-items:center;gap:15px;margin-bottom:8px;'>";
    html += "<div style='display:flex;align-items:center;gap:5px;'>";
    String checked = coverHAEnabled[i] ? " checked" : "";
    html += "<input type='checkbox' id='coverenabled" + String(i) + "'" + checked + " ";
    html += "style='width:18px;height:18px;cursor:pointer;'>";
    html += "<label for='coverenabled" + String(i) + "' style='font-size:11px;color:#aaa;cursor:pointer;'>In Home Assistant anzeigen</label>";
    html += "</div>";
    html += "</div>";
    
    // Dritte Zeile: Speichern-Button
    html += "<div style='text-align:right;'>";
    html += "<button onclick='saveCoverConfig(" + String(i) + ")' class='btn btn-neutral' style='min-width:100px;font-size:12px;'>💾 Speichern</button>";
    html += "</div>";
    
    html += "</div>";  // Close cover card
  }
  
  // ======================================================
  // RELAY-EINTRÄGE (R04-R23, 20 Relais)
  // ======================================================
  html += "<h4 style='margin:30px 0 10px 0;color:#1fa3ec;border-bottom:1px solid #555;padding-bottom:5px;'>💡 Relay-Devices (R04-R23)</h4>";
  
  // Formular für R04-R23 (20 Relais)
  for (int i = 4; i < 24; i++) {
    String relaisNum = (i < 10) ? "R0" + String(i) : "R" + String(i);
    
    html += "<div style='margin:15px 0;padding:10px;background:#2a2a2a;border-radius:5px;border-left:3px solid #1fa3ec;'>";
    
    // Erste Zeile: Relais-Nummer & Name
    html += "<div style='display:flex;align-items:center;gap:10px;margin-bottom:8px;'>";
    html += "<label style='min-width:60px;font-weight:bold;color:#1fa3ec;'>" + relaisNum + ":</label>";
    html += "<input type='text' id='name" + String(i) + "' value='" + relayNames[i] + "' ";
    html += "style='flex:1;padding:6px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
    html += "maxlength='30'>";
    html += "</div>";
    
    // Zweite Zeile: Device-Typ Dropdown & HA Enable Checkbox
    html += "<div style='display:flex;align-items:center;gap:15px;margin-bottom:8px;'>";
    
    // Device-Typ Dropdown
    html += "<div style='display:flex;align-items:center;gap:5px;flex:1;'>";
    html += "<label style='font-size:11px;color:#aaa;min-width:70px;'>HA Typ:</label>";
    html += "<select id='type" + String(i) + "' style='padding:5px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:11px;flex:1;max-width:150px;'>";
    
    // Optionen mit aktuellem Wert ausgewählt
    String types[] = {"none", "switch", "light", "cover", "fan"};
    String typeLabels[] = {"❌ Kein HA", "🔘 Switch", "💡 Light", "🪟 Cover", "🌀 Fan"};
    for (int t = 0; t < 5; t++) {
      String selected = (relayHAType[i] == types[t]) ? " selected" : "";
      html += "<option value='" + types[t] + "'" + selected + ">" + typeLabels[t] + "</option>";
    }
    
    html += "</select>";
    html += "</div>";
    
    // HA Enable Checkbox
    html += "<div style='display:flex;align-items:center;gap:5px;'>";
    String checked = relayHAEnabled[i] ? " checked" : "";
    html += "<input type='checkbox' id='enabled" + String(i) + "'" + checked + " ";
    html += "style='width:18px;height:18px;cursor:pointer;'>";
    html += "<label for='enabled" + String(i) + "' style='font-size:11px;color:#aaa;cursor:pointer;'>HA anzeigen</label>";
    html += "</div>";
    
    html += "</div>";
    
    // Dritte Zeile: Speichern-Button
    html += "<div style='text-align:right;'>";
    html += "<button onclick='saveConfig(" + String(i) + ")' class='btn btn-neutral' style='min-width:100px;font-size:12px;'>💾 Speichern</button>";
    html += "</div>";
    
    html += "</div>";  // Close relay card
  }
  
  // ======================================================
  // INPUT-EINTRÄGE (IN00-IN15, 16 Binary Sensors)
  // ======================================================
  html += "<h4 style='margin:30px 0 10px 0;color:#1fa3ec;border-bottom:1px solid #555;padding-bottom:5px;'>📥 Input-Devices (IN00-IN15)</h4>";
  
  for (int i = 0; i < 16; i++) {
    String inputNum = (i < 10) ? "IN0" + String(i) : "IN" + String(i);
    
    html += "<div style='margin:15px 0;padding:10px;background:#2a2a2a;border-radius:5px;border-left:3px solid #ff9800;'>";
    
    // Erste Zeile: Input-Nummer & Name
    html += "<div style='display:flex;align-items:center;gap:10px;margin-bottom:8px;'>";
    html += "<label style='min-width:60px;font-weight:bold;color:#ff9800;'>" + inputNum + ":</label>";
    html += "<input type='text' id='inputname" + String(i) + "' value='" + inputNames[i] + "' ";
    html += "style='flex:1;padding:6px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
    html += "maxlength='30'>";
    html += "</div>";
    
    // Zweite Zeile: Device-Typ Dropdown & HA Enable Checkbox
    html += "<div style='display:flex;align-items:center;gap:15px;margin-bottom:8px;'>";
    
    // Device-Typ Dropdown (Binary Sensor Device Classes)
    html += "<div style='display:flex;align-items:center;gap:5px;flex:1;'>";
    html += "<label style='font-size:11px;color:#aaa;min-width:70px;'>HA Typ:</label>";
    html += "<select id='inputtype" + String(i) + "' style='padding:5px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:11px;flex:1;max-width:150px;'>";
    
    // Binary Sensor Device Classes
    String types[] = {"none", "switch", "door", "window", "motion", "occupancy", "opening", "garage_door"};
    String typeLabels[] = {"❌ Kein HA", "🔘 Switch", "🚪 Door", "🪟 Window", "👋 Motion", "👤 Occupancy", "📂 Opening", "🏠 Garage"};
    for (int t = 0; t < 8; t++) {
      String selected = (inputHAType[i] == types[t]) ? " selected" : "";
      html += "<option value='" + types[t] + "'" + selected + ">" + typeLabels[t] + "</option>";
    }
    
    html += "</select>";
    html += "</div>";
    
    // HA Enable Checkbox
    html += "<div style='display:flex;align-items:center;gap:5px;'>";
    String checked = inputHAEnabled[i] ? " checked" : "";
    html += "<input type='checkbox' id='inputenabled" + String(i) + "'" + checked + " ";
    html += "style='width:18px;height:18px;cursor:pointer;'>";
    html += "<label for='inputenabled" + String(i) + "' style='font-size:11px;color:#aaa;cursor:pointer;'>HA anzeigen</label>";
    html += "</div>";
    
    html += "</div>";
    
    // Dritte Zeile: Speichern-Button
    html += "<div style='text-align:right;'>";
    html += "<button onclick='saveInputConfig(" + String(i) + ")' class='btn btn-neutral' style='min-width:100px;font-size:12px;'>💾 Speichern</button>";
    html += "</div>";
    
    html += "</div>";  // Close input card
  }
  
  html += "</div>";
  
  // JavaScript für Save-Funktionen
  html += "<script>";
  
  // Cover Config speichern
  html += "function saveCoverConfig(coverIndex) {";
  html += "  var name = document.getElementById('covername' + coverIndex).value.trim();";
  html += "  var enabled = document.getElementById('coverenabled' + coverIndex).checked ? '1' : '0';";
  html += "  if (name === '') {";
  html += "    alert('Name darf nicht leer sein!');";
  html += "    return;";
  html += "  }";
  html += "  fetch('/savecover?cover=' + coverIndex + '&name=' + encodeURIComponent(name) + '&enabled=' + enabled)";
  html += "  .then(function(response) { return response.json(); })";
  html += "  .then(function(data) {";
  html += "    if (data.success) {";
  html += "      alert('✅ Cover-Konfiguration gespeichert\\n\\nName: ' + name + '\\nHA: ' + (enabled === '1' ? 'Ja' : 'Nein'));";
  html += "    } else {";
  html += "      alert('❌ Fehler beim Speichern');";
  html += "    }";
  html += "  })";
  html += "  .catch(function(error) {";
  html += "    alert('❌ Fehler: ' + error);";
  html += "  });";
  html += "}";
  
  // Relay Config speichern
  html += "function saveConfig(relayIndex) {";
  html += "  var name = document.getElementById('name' + relayIndex).value.trim();";
  html += "  var type = document.getElementById('type' + relayIndex).value;";
  html += "  var enabled = document.getElementById('enabled' + relayIndex).checked ? '1' : '0';";
  html += "  if (name === '') {";
  html += "    alert('Name darf nicht leer sein!');";
  html += "    return;";
  html += "  }";
  html += "  fetch('/savename?relay=' + relayIndex + '&name=' + encodeURIComponent(name) + '&type=' + type + '&enabled=' + enabled)";
  html += "  .then(function(response) { return response.json(); })";
  html += "  .then(function(data) {";
  html += "    if (data.success) {";
  html += "      alert('✅ Konfiguration gespeichert\\n\\nName: ' + name + '\\nTyp: ' + type + '\\nHA: ' + (enabled === '1' ? 'Ja' : 'Nein'));";
  html += "    } else {";
  html += "      alert('❌ Fehler beim Speichern');";
  html += "    }";
  html += "  })";
  html += "  .catch(function(error) {";
  html += "    alert('❌ Fehler: ' + error);";
  html += "  });";
  html += "}";
  
  // Input Config speichern
  html += "function saveInputConfig(inputIndex) {";
  html += "  var name = document.getElementById('inputname' + inputIndex).value.trim();";
  html += "  var type = document.getElementById('inputtype' + inputIndex).value;";
  html += "  var enabled = document.getElementById('inputenabled' + inputIndex).checked ? '1' : '0';";
  html += "  if (name === '') {";
  html += "    alert('Name darf nicht leer sein!');";
  html += "    return;";
  html += "  }";
  html += "  fetch('/saveinput?input=' + inputIndex + '&name=' + encodeURIComponent(name) + '&type=' + type + '&enabled=' + enabled)";
  html += "  .then(function(response) { return response.json(); })";
  html += "  .then(function(data) {";
  html += "    if (data.success) {";
  html += "      alert('✅ Input-Konfiguration gespeichert\\\\n\\\\nName: ' + name + '\\\\nTyp: ' + type + '\\\\nHA: ' + (enabled === '1' ? 'Ja' : 'Nein'));";
  html += "    } else {";
  html += "      alert('❌ Fehler beim Speichern');";
  html += "    }";
  html += "  })";
  html += "  .catch(function(error) {";
  html += "    alert('❌ Fehler: ' + error);";
  html += "  });";
  html += "}";
  
  html += "</script>";
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== API-Handler zum Speichern der Relay-Namen & HA-Config =====
void handleSaveName() {
  if (!server.hasArg("relay") || !server.hasArg("name")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing parameters\"}");
    return;
  }
  
  int relayIndex = server.arg("relay").toInt();
  String newName = server.arg("name");
  String newType = server.hasArg("type") ? server.arg("type") : "switch";
  bool newEnabled = server.hasArg("enabled") && server.arg("enabled") == "1";
  
  // Validierung
  if (relayIndex < 0 || relayIndex >= 24) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid relay index\"}");
    return;
  }
  
  if (newName.length() == 0 || newName.length() > 30) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Name length must be 1-30 characters\"}");
    return;
  }
  
  // Type validieren
  if (newType != "none" && newType != "switch" && newType != "light" && newType != "cover" && newType != "fan") {
    newType = "switch";  // Fallback auf switch
  }
  
  // Name speichern
  saveRelayName(relayIndex, newName);
  
  // HA-Config speichern
  saveRelayHAConfig(relayIndex, newType, newEnabled);
  
  // MQTT Discovery neu senden (falls MQTT aktiv)
  if (mqttConfig.enabled && mqttClient.connected()) {
    Serial.println("🔄 MQTT Discovery wird neu gesendet...");
    publishMQTTDiscovery();
  }
  
  // Erfolg zurückmelden
  String json = "{\"success\":true,\"relay\":" + String(relayIndex) + ",\"name\":\"" + newName + "\",\"type\":\"" + newType + "\",\"enabled\":" + String(newEnabled ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ===== API-Handler zum Speichern der Cover-Config =====
void handleSaveCover() {
  if (!server.hasArg("cover") || !server.hasArg("name")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing parameters\"}");
    return;
  }
  
  int coverIndex = server.arg("cover").toInt();
  String newName = server.arg("name");
  bool newEnabled = server.hasArg("enabled") && server.arg("enabled") == "1";
  
  // Validierung
  if (coverIndex < 0 || coverIndex >= 2) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid cover index\"}");
    return;
  }
  
  if (newName.length() == 0 || newName.length() > 30) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Name length must be 1-30 characters\"}");
    return;
  }
  
  // Cover-Config speichern
  saveCoverConfig(coverIndex, newName, newEnabled);
  
  // MQTT Discovery neu senden (falls MQTT aktiv)
  if (mqttConfig.enabled && mqttClient.connected()) {
    Serial.println("🔄 MQTT Discovery wird neu gesendet...");
    publishMQTTDiscovery();
  }
  
  // Erfolg zurückmelden
  String json = "{\"success\":true,\"cover\":" + String(coverIndex) + ",\"name\":\"" + newName + "\",\"enabled\":" + String(newEnabled ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ===== API-Handler für Cover-Actions (OPEN/CLOSE/STOP) =====
void handleCover() {
  if (!server.hasArg("idx") || !server.hasArg("action")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing parameters\"}");
    return;
  }
  
  int coverIndex = server.arg("idx").toInt();
  String action = server.arg("action");
  action.toUpperCase();
  
  // Validierung
  if (coverIndex < 0 || coverIndex >= 2) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid cover index\"}");
    return;
  }
  
  if (action != "OPEN" && action != "CLOSE" && action != "STOP") {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid action\"}");
    return;
  }
  
  // Cover-Action ausführen
  setCoverState(coverIndex, action.c_str());
  
  // Response mit aktuellem Status
  int openRelay = coverRelayPairs[coverIndex][0];
  int closeRelay = coverRelayPairs[coverIndex][1];
  
  String json = "{\"success\":true,";
  json += "\"cover\":" + String(coverIndex) + ",";
  json += "\"action\":\"" + action + "\",";
  json += "\"openRelay\":" + String(relayState[openRelay]) + ",";
  json += "\"closeRelay\":" + String(relayState[closeRelay]) + "}";
  
  server.send(200, "application/json", json);
}

// ===== API-Handler zum Speichern der Input-Config =====
void handleSaveInput() {
  if (!server.hasArg("input") || !server.hasArg("name")) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing parameters\"}");
    return;
  }
  
  int inputIndex = server.arg("input").toInt();
  String newName = server.arg("name");
  String newType = server.hasArg("type") ? server.arg("type") : "switch";
  bool newEnabled = server.hasArg("enabled") && server.arg("enabled") == "1";
  
  // Validierung
  if (inputIndex < 0 || inputIndex >= 16) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid input index\"}");
    return;
  }
  
  if (newName.length() == 0 || newName.length() > 30) {
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Name length must be 1-30 characters\"}");
    return;
  }
  
  // Type validieren (Binary Sensor Device Classes)
  if (newType != "none" && newType != "switch" && newType != "door" && newType != "window" && 
      newType != "motion" && newType != "occupancy" && newType != "opening" && newType != "garage_door") {
    newType = "switch";  // Fallback auf switch
  }
  
  // Input-Config speichern
  saveInputConfig(inputIndex, newName, newType, newEnabled);
  
  // MQTT Discovery neu senden (falls MQTT aktiv)
  if (mqttConfig.enabled && mqttClient.connected()) {
    Serial.println("🔄 MQTT Discovery wird neu gesendet...");
    publishMQTTDiscovery();
  }
  
  // Erfolg zurückmelden
  String json = "{\"success\":true,\"input\":" + String(inputIndex) + ",\"name\":\"" + newName + "\",\"type\":\"" + newType + "\",\"enabled\":" + String(newEnabled ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ===== MQTT-Konfigurationsseite =====
void handleMQTT() {
  String html = getHTMLHeader("mqtt");
  
  html += "<h3>📨 MQTT Konfiguration</h3>";
  html += "<div class='card'>";
  
  // Status-Anzeige
  html += "<div style='margin-bottom:15px;padding:10px;background:";
  html += mqttClient.connected() ? "#1b5e20" : "#b71c1c";
  html += ";border-radius:3px;'>";
  html += "<p style='margin:0;color:#fff;'><b>Status: ";
  html += mqttClient.connected() ? "✅ Verbunden" : "❌ Nicht verbunden";
  html += "</b></p>";
  if (mqttClient.connected()) {
    html += "<p style='margin:5px 0 0 0;color:#ddd;font-size:11px;'>Home Assistant Discovery aktiv</p>";
  }
  html += "</div>";
  
  // Konfigurationsformular
  html += "<form onsubmit='saveMQTTConfig(event)' style='margin:15px 0;'>";
  
  // Enable Checkbox
  html += "<div style='margin:10px 0;'>";
  html += "<label style='display:flex;align-items:center;cursor:pointer;'>";
  html += "<input type='checkbox' id='mqtt_enabled' " + String(mqttConfig.enabled ? "checked" : "") + " ";
  html += "style='margin-right:10px;width:16px;height:16px;cursor:pointer;'>";
  html += "<span>MQTT aktivieren</span>";
  html += "</label>";
  html += "</div>";
  
  // Broker IP
  html += "<div style='margin:15px 0;'>";
  html += "<label>Broker IP:</label><br>";
  html += "<input type='text' id='mqtt_broker' value='" + String(mqttConfig.broker) + "' ";
  html += "style='width:100%;padding:8px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
  html += "placeholder='192.168.178.1'>";
  html += "</div>";
  
  // Port
  html += "<div style='margin:15px 0;'>";
  html += "<label>Port:</label><br>";
  html += "<input type='number' id='mqtt_port' value='" + String(mqttConfig.port) + "' ";
  html += "style='width:100%;padding:8px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
  html += "placeholder='1883'>";
  html += "</div>";
  
  // User
  html += "<div style='margin:15px 0;'>";
  html += "<label>User (optional):</label><br>";
  html += "<input type='text' id='mqtt_user' value='" + String(mqttConfig.user) + "' ";
  html += "style='width:100%;padding:8px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
  html += "placeholder='mqtt_user'>";
  html += "</div>";
  
  // Password
  html += "<div style='margin:15px 0;'>";
  html += "<label>Passwort (optional):</label><br>";
  html += "<input type='password' id='mqtt_pass' value='" + String(mqttConfig.password) + "' ";
  html += "style='width:100%;padding:8px;background:#333;border:1px solid #555;color:#eee;border-radius:3px;font-size:12px;' ";
  html += "placeholder='****'>";
  html += "</div>";
  
  // Buttons
  html += "<div style='margin:20px 0;display:flex;gap:10px;'>";
  html += "<button type='submit' class='btn btn-on'>💾 Speichern & Neu starten</button>";
  html += "<button type='button' onclick='testMQTT()' class='btn btn-neutral'>🔍 Verbindung testen</button>";
  html += "</div>";
  
  html += "</form>";
  
  // Info-Box
  html += "<div class='card' style='background:#1a2332;border-color:#1fa3ec;'>";
  html += "<p style='margin:5px 0;font-size:11px;'><b>📌 Home Assistant Integration:</b></p>";
  html += "<p style='margin:5px 0;font-size:11px;'>• Base Topic: <code style='background:#333;padding:2px 4px;'>" + String(MQTT_BASE_TOPIC) + "</code></p>";
  html += "<p style='margin:5px 0;font-size:11px;'>• Discovery Prefix: <code style='background:#333;padding:2px 4px;'>" + String(MQTT_DISCOVERY_PREFIX) + "</code></p>";
  html += "<p style='margin:5px 0;font-size:11px;'>• Entities: 20 Lights/Switches (R04-R23), 2 Covers, 16 Binary Sensors, 1 Switch (MPR121)</p>";
  html += "<p style='margin:5px 0;font-size:11px;'>• Auto-Discovery beim Boot</p>";
  html += "</div>";
  
  html += "</div>";
  
  // JavaScript
  html += "<script>";
  html += "function saveMQTTConfig(event) {";
  html += "  event.preventDefault();";
  html += "  var enabled = document.getElementById('mqtt_enabled').checked;";
  html += "  var broker = document.getElementById('mqtt_broker').value.trim();";
  html += "  var port = document.getElementById('mqtt_port').value;";
  html += "  var user = document.getElementById('mqtt_user').value.trim();";
  html += "  var pass = document.getElementById('mqtt_pass').value.trim();";
  html += "  if (enabled && broker === '') {";
  html += "    alert('Broker IP darf nicht leer sein!');";
  html += "    return;";
  html += "  }";
  html += "  var params = '?enabled=' + (enabled?'1':'0');";
  html += "  params += '&broker=' + encodeURIComponent(broker);";
  html += "  params += '&port=' + port;";
  html += "  params += '&user=' + encodeURIComponent(user);";
  html += "  params += '&pass=' + encodeURIComponent(pass);";
  html += "  fetch('/savemqtt' + params)";
  html += "  .then(function(response) { return response.json(); })";
  html += "  .then(function(data) {";
  html += "    if (data.success) {";
  html += "      alert('✅ MQTT Config gespeichert!\\n\\n⚠️ ESP32 startet neu...');";
  html += "      setTimeout(function() { location.reload(); }, 3000);";
  html += "    } else {";
  html += "      alert('❌ Fehler beim Speichern');";
  html += "    }";
  html += "  });";
  html += "}";
  html += "function testMQTT() {";
  html += "  alert('🔍 Test-Verbindung wird geprüft...\\n(Siehe Serial Monitor für Details)');";
  html += "}";
  html += "</script>";
  
  html += getHTMLFooter();
  server.send(200, "text/html; charset=UTF-8", html);
}

// ===== MPR121 Touch Enable/Disable Toggle =====
void handleToggleMPR121() {
  mpr121Enabled = !mpr121Enabled;
  saveMPR121State();
  publishMPR121State();  // MQTT State publishen
  
  Serial.println("📱 MPR121 Touch-Eingabe " + String(mpr121Enabled ? "AKTIVIERT" : "DEAKTIVIERT"));
  
  // JSON Response für optimistic UI
  String json = "{\"enabled\":" + String(mpr121Enabled ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// ===== ESP32 Restart =====
void handleRestart() {
  String html = "<html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='10; url=/info'>";
  html += "<title>Restart</title>";
  html += "<style>body{font-family:Arial;text-align:center;padding:50px;background:#1a1a1a;color:#eee;}</style>";
  html += "</head><body>";
  html += "<div style='background:#252525;padding:30px;border-radius:8px;display:inline-block;box-shadow:0 2px 5px rgba(0,0,0,0.3);'>";
  html += "<h2 style='margin-top:0;color:#1fa3ec;'>🔄 ESP32 wird neu gestartet</h2>";
  html += "<p style='font-size:18px;margin:20px 0;'>Bitte warten Sie ~10 Sekunden...</p>";
  html += "<div style='margin:20px 0;'>";
  html += "<div style='width:200px;height:4px;background:#333;border-radius:2px;overflow:hidden;margin:0 auto;'>";
  html += "<div style='width:0%;height:100%;background:#1fa3ec;animation:progress 10s linear;'></div>";
  html += "</div>";
  html += "</div>";
  html += "<p style='color:#888;font-size:12px;'>Sie werden automatisch weitergeleitet...</p>";
  html += "</div>";
  html += "<style>@keyframes progress{to{width:100%;}}</style>";
  html += "</body></html>";
  
  server.send(200, "text/html; charset=UTF-8", html);
  
  Serial.println("\n🔄 ESP32 Restart angefordert...");
  delay(1000);
  ESP.restart();
}

// ===== MQTT Config Speichern =====
void handleSaveMQTT() {
  mqttConfig.enabled = server.hasArg("enabled") && server.arg("enabled") == "1";
  
  if (server.hasArg("broker")) {
    server.arg("broker").toCharArray(mqttConfig.broker, 64);
  }
  if (server.hasArg("port")) {
    mqttConfig.port = server.arg("port").toInt();
  }
  if (server.hasArg("user")) {
    server.arg("user").toCharArray(mqttConfig.user, 32);
  }
  if (server.hasArg("pass")) {
    server.arg("pass").toCharArray(mqttConfig.password, 32);
  }
  
  saveMQTTConfig();
  
  String json = "{\"success\":true,\"restart\":true}";
  server.send(200, "application/json", json);
  
  // Nach kurzer Verzögerung neu starten
  delay(1000);
  ESP.restart();
}

// ===== Relay State setzen mit MQTT Publish =====
void setRelayState(int relayIndex, int state) {
  if (relayIndex < 0 || relayIndex >= 24) return;
  
  relayState[relayIndex] = state;
  
  // Hardware schalten
  int board = relayIndex / 8;  // 0, 1, 2
  int pin = relayIndex % 8;    // 0-7
  
  if (board == 0) {
    pcaRel1.digitalWrite(pin, state ? HIGH : LOW);
  } else if (board == 1) {
    pcaRel2.digitalWrite(pin, state ? HIGH : LOW);
  } else if (board == 2) {
    pcaRel3.digitalWrite(pin, state ? HIGH : LOW);
  }
  
  // MQTT State publishen
  publishRelayState(relayIndex);
  
  Serial.print("Relay R");
  if (relayIndex < 10) Serial.print("0");
  Serial.print(relayIndex);
  Serial.print(" → ");
  Serial.println(state ? "EIN" : "AUS");
}

// ======================================================
// COVER STATE SETZEN (OPEN/CLOSE/STOP)
// ======================================================
void setCoverState(int coverIndex, const char* action) {
  if (coverIndex < 0 || coverIndex >= 2) return;
  
  int openRelay = coverRelayPairs[coverIndex][0];
  int closeRelay = coverRelayPairs[coverIndex][1];
  
  Serial.print("🔧 setCoverState() Cover ");
  Serial.print(coverIndex);
  Serial.print(" (");
  Serial.print(coverNames[coverIndex]);
  Serial.print(") → ");
  Serial.println(action);
  
  if (strcmp(action, "OPEN") == 0) {
    // Hoch fahren: Open-Relay EIN, Close-Relay AUS
    setRelayState(openRelay, 1);
    setRelayState(closeRelay, 0);
  } 
  else if (strcmp(action, "CLOSE") == 0) {
    // Runter fahren: Open-Relay AUS, Close-Relay EIN
    setRelayState(openRelay, 0);
    setRelayState(closeRelay, 1);
  } 
  else if (strcmp(action, "STOP") == 0) {
    // Stopp: Beide Relais AUS
    setRelayState(openRelay, 0);
    setRelayState(closeRelay, 0);
  }
  
  // Cover State publishen
  publishCoverState(coverIndex);
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
    
    // MQTT State publishen
    publishRelayState(idx);
  }
  
  // AJAX-Support: JSON mit aktuellem Status zurückgeben
  // Bei Rollos (0-3) beide Stati zurückgeben, da sie sich gegenseitig beeinflussen
  String response;
  if (idx == 0 || idx == 1) {
    // Fensterrollo: beide Stati (R00 und R01) zurückgeben
    response = "{\"relay\":" + String(idx) + ",\"state\":" + String(relayState[idx]) + 
               ",\"pair\":" + String(idx == 0 ? 1 : 0) + ",\"pairState\":" + String(relayState[idx == 0 ? 1 : 0]) + "}";
  } else if (idx == 2 || idx == 3) {
    // Türrollo: beide Stati (R02 und R03) zurückgeben
    response = "{\"relay\":" + String(idx) + ",\"state\":" + String(relayState[idx]) + 
               ",\"pair\":" + String(idx == 2 ? 3 : 2) + ",\"pairState\":" + String(relayState[idx == 2 ? 3 : 2]) + "}";
  } else {
    // Normale Relais: nur eigener Status
    response = "{\"relay\":" + String(idx) + ",\"state\":" + String(relayState[idx]) + "}";
  }
  server.send(200, "application/json", response);
}

void handleInputs() {
  // JSON mit allen 16 Eingangszuständen zurückgeben
  String response = "{\"inputs\":[";
  for (int i = 0; i < 16; i++) {
    response += String(inputState[i]);
    if (i < 15) response += ",";
  }
  response += "]}";
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
    publishRelayState(0);  // MQTT State publishen
  } else {
    // Start hoch, runter stoppen
    relayState[0] = 1;
    relayState[1] = 0;
    pcaRel1.digitalWrite(0, HIGH);   // Hoch EIN
    pcaRel1.digitalWrite(1, LOW);  // Runter AUS
    Serial.println("Fensterrollo START hoch");
    fensterrolloTimer = millis();
    publishRelayState(0);  // MQTT State publishen (hoch)
    publishRelayState(1);  // MQTT State publishen (runter)
  }
  publishCoverState(0);  // Cover-State aktualisieren
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
    publishRelayState(1);  // MQTT State publishen
  } else {
    // Start runter, hoch stoppen
    relayState[0] = 0;
    relayState[1] = 1;
    pcaRel1.digitalWrite(0, LOW);  // Hoch AUS
    pcaRel1.digitalWrite(1, HIGH);   // Runter EIN
    Serial.println("Fensterrollo START runter");
    fensterrolloTimer = millis();
    publishRelayState(0);  // MQTT State publishen (hoch)
    publishRelayState(1);  // MQTT State publishen (runter)
  }
  publishCoverState(0);  // Cover-State aktualisieren
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
    publishRelayState(2);  // MQTT State publishen
  } else {
    // Start hoch, runter stoppen
    relayState[2] = 1;
    relayState[3] = 0;
    pcaRel1.digitalWrite(2, HIGH);   // Hoch EIN
    pcaRel1.digitalWrite(3, LOW);  // Runter AUS
    Serial.println("Türrollo START hoch");
    tuerrolloTimer = millis();
    publishRelayState(2);  // MQTT State publishen (hoch)
    publishRelayState(3);  // MQTT State publishen (runter)
  }
  publishCoverState(1);  // Cover-State aktualisieren
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
    publishRelayState(3);  // MQTT State publishen
  } else {
    // Start runter, hoch stoppen
    relayState[2] = 0;
    relayState[3] = 1;
    pcaRel1.digitalWrite(2, LOW);  // Hoch AUS
    pcaRel1.digitalWrite(3, HIGH);   // Runter EIN
    Serial.println("Türrollo START runter");
    tuerrolloTimer = millis();
    publishRelayState(2);  // MQTT State publishen (hoch)
    publishRelayState(3);  // MQTT State publishen (runter)
  }
  publishCoverState(1);  // Cover-State aktualisieren
}
void toggleAussenlampeGarten() {
  // R04 (idx 4)
  // TouchBoard1: case 0: unten 2te von links
  int idx = 4;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
  publishRelayState(idx);  // MQTT State publishen
}
void toggleSteinlampe() {
  // R05 (idx 5)
  // TouchBoard2: case 6: 2te links (auskommentiert in old)
  int idx = 5;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
  publishRelayState(idx);  // MQTT State publishen
}
void toggleKGFlurlampe() {
  // R06 (idx 6)
  // Kein direkter Touch, Schalter/EG/KG
  int idx = 6;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(idx, relayState[idx] ? HIGH : LOW);
  publishRelayState(idx);  // MQTT State publishen
}
void toggleKuechenarbeitslampe() {
  // R07 (idx 7)
  // TouchBoard2: case 3: unten links
  int idx = 7;
  relayState[idx] = !relayState[idx];
  pcaRel1.digitalWrite(7, relayState[idx] ? HIGH : LOW);  // Board 1, Pin 7
  publishRelayState(idx);  // MQTT State publishen
}
void toggleKuechenlampe() {
  // R08 (idx 8)
  // TouchBoard2: case 2: 3te links
  int idx = 8;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(0, relayState[idx] ? HIGH : LOW);  // Board 2, Pin 0
  publishRelayState(idx);  // MQTT State publishen
}
void toggleEGFlurlampe() {
  // R09 (idx 9)
  // TouchBoard2: case 0: 3te rechts
  int idx = 9;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(1, relayState[idx] ? HIGH : LOW);  // Board 2, Pin 1
  publishRelayState(idx);  // MQTT State publishen
}
void toggleTraegerlampen() {
  // R10 (idx 10)
  // TouchBoard2: case 1: unten rechts
  int idx = 10;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(2, relayState[idx] ? HIGH : LOW);  // Board 2, Pin 2
  publishRelayState(idx);  // MQTT State publishen
}
void toggleWohnzimmerlampe1() {
  // R11 (idx 11)
  // TouchBoard1: case 2: oben 1te von links
  int idx = 11;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(3, relayState[idx] ? HIGH : LOW);  // Board 2, Pin 3
  publishRelayState(idx);  // MQTT State publishen
}
void toggleWohnzimmerlampe2() {
  // R12 (idx 12)
  // Kein direkter Touch, nur Gruppe
  int idx = 12;
  relayState[idx] = !relayState[idx];
  pcaRel2.digitalWrite(4, relayState[idx] ? HIGH : LOW);  // Board 2, Pin 4
  publishRelayState(idx);  // MQTT State publishen
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
    publishRelayState(idx);  // MQTT State publishen
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
      // R11 ist jetzt unabhängig vom AC Dimmer - Relais muss separat geschaltet werden
    }
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void toggleKronleuchter() {
  // AC Dimmer toggle: AUS → 50% → AUS (ohne R11 Relais)
  if (acDimmerBrightness == 0) {
    setACDimmerBrightness(127); // 50% Helligkeit
  } else {
    setACDimmerBrightness(0); // AUS
  }
  Serial.println("AC Dimmer toggle (R11 Relais ist jetzt unabhängig)");
}

void dimKronleuchter(bool dimUp) {
  const uint8_t dimStep = 15; // Dimm-Schritte
  
  if (dimUp) {
    // Heller dimmen
    if (acDimmerBrightness < 240) {
      setACDimmerBrightness(acDimmerBrightness + dimStep);
    }
  } else {
    // Dunkler dimmen
    if (acDimmerBrightness > dimStep) {
      setACDimmerBrightness(acDimmerBrightness - dimStep);
    } else {
      setACDimmerBrightness(0); // Komplett aus
    }
  }
  // R11 Relais ist jetzt unabhängig vom AC Dimmer
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
  // ⏱️ WICHTIG: 9-bit Genauigkeit braucht ~94ms für Konversion
  // Blockierendes delay - könnte später durch non-blocking ersetzt werden
  delay(100);
  
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
    String tempColor = "#fff";
    if (schaltschrankTemp < 10) tempColor = "#2196F3";
    else if (schaltschrankTemp > 35) tempColor = "#f44336";
    else if (schaltschrankTemp > 30) tempColor = "#ff9800";
    
    html += "<td style='color:" + tempColor + ";font-weight:bold;'>" + String(schaltschrankTemp, 1) + " °C</td>";
    html += "<td style='color:#4caf50;'>✓ OK</td>";
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


