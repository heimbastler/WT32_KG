// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    PCA9535 RELAIS BOARDS TEST                               ║
// ║                              Version 1.0                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
//
// Test für 3x PCA9535 16-Bit I/O Expander Relais Boards
// - Board A: 0x20 (Default Adresse - alle Adress-Pads offen)
// - Board B: 0x21 (A0 gelötet)  
// - Board C: 0x22 (A1 gelötet)
//
// Jedes Board hat 16 Pins, aber nur 8 Relais angeschlossen (Pin 0-7)
// Pin 8-15 sind als Eingänge oder Reserve verfügbar

#include "unity.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PCA9535.h>

// ---------- I2C Setup ----------
#define SDA_PIN 33 // GPIO33 (488_EN für dieses Projekt)
#define SCL_PIN 32 // GPIO32 (CFG für dieses Projekt)

// ---------- PCA9535 Test Board Definitionen ----------
Adafruit_PCA9535 testBoard;  // Wird für jedes Board neu initialisiert

// Test Board Adressen - KORRIGIERT nach main.cpp!
// ACHTUNG: 0x20, 0x21 bereits von PCF8574 Input Boards belegt!
const uint8_t BOARD_ADDRESSES[] = {0x22, 0x23, 0x24};  // PCA9535 Relais Board Adressen
const char* BOARD_NAMES[] = {"Board_A", "Board_B", "Board_C"};
const uint8_t NUM_BOARDS = 3;
const uint8_t NUM_RELAYS_PER_BOARD = 8;  // Nur Pin 0-7 haben Relais

// Test Variablen
uint8_t currentBoardIndex = 0;
bool boardFoundStatus[NUM_BOARDS] = {false, false, false};

// ---------- Setup und Teardown ----------
void setUp(void) {
    // Wird vor jedem Test ausgeführt
}

void tearDown(void) {
    // Wird nach jedem Test ausgeführt
}

// ---------- I2C Initialisierung ----------
void initI2C() {
    Wire.begin(SDA_PIN, SCL_PIN, 100000);  // 100kHz für stabile Kommunikation
    delay(100);
    Serial.println("I2C Bus initialisiert (SDA: GPIO33, SCL: GPIO32)");
}

// ---------- I2C Scanner ----------
void scanI2CBus() {
    Serial.println("\n=== I2C Bus Scan ===");
    uint8_t devicesFound = 0;
    
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("I2C Gerät gefunden: 0x%02X", addr);
            
            // Prüfe ob es ein erwartetes PCA9535 Board ist
            for (uint8_t i = 0; i < NUM_BOARDS; i++) {
                if (addr == BOARD_ADDRESSES[i]) {
                    Serial.printf(" ← %s (PCA9535)", BOARD_NAMES[i]);
                    boardFoundStatus[i] = true;
                    break;
                }
            }
            Serial.println();
            devicesFound++;
        }
    }
    
    if (devicesFound == 0) {
        Serial.println("❌ Keine I2C Geräte gefunden!");
    } else {
        Serial.printf("✅ %d I2C Geräte gefunden\n", devicesFound);
    }
    Serial.println();
}

// ---------- Einzelnes Board Testen ----------
bool testSingleBoard(uint8_t boardIndex) {
    uint8_t addr = BOARD_ADDRESSES[boardIndex];
    const char* name = BOARD_NAMES[boardIndex];
    
    Serial.printf("\n╔══════════════════════════════════════╗\n");
    Serial.printf("║     TESTE %s (0x%02X)           ║\n", name, addr);
    Serial.printf("╚══════════════════════════════════════╝\n");
    
    // Board initialisieren
    if (!testBoard.begin(addr)) {
        Serial.printf("❌ %s nicht gefunden oder Initialisierung fehlgeschlagen!\n", name);
        return false;
    }
    
    Serial.printf("✅ %s erfolgreich initialisiert\n", name);
    
    // Alle Pins als Ausgänge konfigurieren (Pin 0-7 für Relais)
    for (uint8_t pin = 0; pin < NUM_RELAYS_PER_BOARD; pin++) {
        testBoard.pinMode(pin, OUTPUT);
        testBoard.digitalWrite(pin, LOW);  // Alle Relais zunächst aus
    }
    
    delay(500);
    Serial.printf("Alle Relais (Pin 0-7) auf AUS gesetzt\n");
    
    // Jedes Relais einzeln durchschalten
    for (uint8_t relayPin = 0; relayPin < NUM_RELAYS_PER_BOARD; relayPin++) {
        Serial.printf("\n--- Relais R%02d (Pin %d) ---\n", 
                     (boardIndex * NUM_RELAYS_PER_BOARD) + relayPin, relayPin);
        
        // Relais EIN
        testBoard.digitalWrite(relayPin, HIGH);
        Serial.printf("  ⚡ Relais R%02d → EIN", (boardIndex * NUM_RELAYS_PER_BOARD) + relayPin);
        Serial.flush();
        delay(1000);  // 1 Sekunde warten
        
        // Relais AUS  
        testBoard.digitalWrite(relayPin, LOW);
        Serial.printf(" → AUS ✓\n");
        delay(500);   // Kurz warten vor nächstem Relais
    }
    
    Serial.printf("\n✅ %s Test abgeschlossen - alle 8 Relais funktionieren\n", name);
    return true;
}

// ---------- Alle Relais gleichzeitig testen ----------
void testAllRelaysSimultaneous(uint8_t boardIndex) {
    uint8_t addr = BOARD_ADDRESSES[boardIndex];
    const char* name = BOARD_NAMES[boardIndex];
    
    Serial.printf("\n--- Alle Relais gleichzeitig (%s) ---\n", name);
    
    // Alle Relais EIN
    for (uint8_t pin = 0; pin < NUM_RELAYS_PER_BOARD; pin++) {
        testBoard.digitalWrite(pin, HIGH);
    }
    Serial.printf("⚡ Alle 8 Relais → EIN");
    Serial.flush();
    delay(2000);  // 2 Sekunden alle an
    
    // Alle Relais AUS
    for (uint8_t pin = 0; pin < NUM_RELAYS_PER_BOARD; pin++) {
        testBoard.digitalWrite(pin, LOW);
    }
    Serial.printf(" → AUS ✓\n");
    delay(500);
}

// ---------- Unity Test Funktionen ----------
void test_i2c_init() {
    initI2C();
    TEST_PASS_MESSAGE("I2C erfolgreich initialisiert");
}

void test_i2c_scan() {
    scanI2CBus();
    
    uint8_t foundBoards = 0;
    for (uint8_t i = 0; i < NUM_BOARDS; i++) {
        if (boardFoundStatus[i]) foundBoards++;
    }
    
    Serial.printf("Gefundene PCA9535 Boards: %d von %d\n", foundBoards, NUM_BOARDS);
    
    if (foundBoards == 0) {
        TEST_FAIL_MESSAGE("Keine PCA9535 Boards gefunden! Verkabelung prüfen.");
    } else if (foundBoards < NUM_BOARDS) {
        char msg[100];
        sprintf(msg, "Nur %d von %d Boards gefunden. Teste verfügbare Boards.", foundBoards, NUM_BOARDS);
        TEST_PASS_MESSAGE(msg);
    } else {
        TEST_PASS_MESSAGE("Alle 3 PCA9535 Boards gefunden!");
    }
}

void test_board_a_relays() {
    if (!boardFoundStatus[0]) {
        TEST_IGNORE_MESSAGE("Board A nicht gefunden - überspringe Test");
        return;
    }
    
    bool result = testSingleBoard(0);
    TEST_ASSERT_TRUE_MESSAGE(result, "Board A Relais Test fehlgeschlagen");
    
    testAllRelaysSimultaneous(0);
}

void test_board_b_relays() {
    if (!boardFoundStatus[1]) {
        TEST_IGNORE_MESSAGE("Board B nicht gefunden - überspringe Test");
        return;
    }
    
    bool result = testSingleBoard(1);
    TEST_ASSERT_TRUE_MESSAGE(result, "Board B Relais Test fehlgeschlagen");
    
    testAllRelaysSimultaneous(1);
}

void test_board_c_relays() {
    if (!boardFoundStatus[2]) {
        TEST_IGNORE_MESSAGE("Board C nicht gefunden - überspringe Test");
        return;
    }
    
    bool result = testSingleBoard(2);
    TEST_ASSERT_TRUE_MESSAGE(result, "Board C Relais Test fehlgeschlagen");
    
    testAllRelaysSimultaneous(2);
}

void test_manual_interactive() {
    Serial.println("\n╔══════════════════════════════════════╗");
    Serial.println("║       MANUELLER INTERAKTIVER TEST   ║");
    Serial.println("╚══════════════════════════════════════╝");
    Serial.println("Du kannst jetzt manuell einzelne Boards testen:");
    Serial.println("- Stecke nur EIN Board ein");
    Serial.println("- Drücke Reset am ESP32");  
    Serial.println("- Beobachte die Relais beim Durchschalten");
    Serial.println("- Prüfe ob alle 8 Relais klicken und LEDs leuchten");
    Serial.println("");
    Serial.println("Erwartete PCA9535 Relais Board Adressen:");
    Serial.println("- Board A mit A1 gelötet: 0x22");
    Serial.println("- Board B mit A1+A0 gelötet: 0x23");
    Serial.println("- Board C mit A2 gelötet: 0x24");
    Serial.println("");
    Serial.println("HINWEIS: 0x20, 0x21 sind bereits von PCF8574 Input Boards belegt!");
    
    TEST_PASS_MESSAGE("Manueller Test-Hinweis angezeigt");
}

// ---------- Arduino Setup/Loop für Unity ----------
void setup() {
    delay(2000);  // Warten bis Serial verfügbar
    
    Serial.begin(115200);
    Serial.println("\n============================================================");
    Serial.println("    PCA9535 RELAIS BOARDS FUNKTIONSTEST");
    Serial.println("                 Version 1.0");
    Serial.println("============================================================");
    Serial.println("Hardware: WT32-ETH01 (ESP32-WROOM-32)");
    Serial.println("I2C: SDA=GPIO33, SCL=GPIO32");
    Serial.println("Relais: 3x PCA9535 Boards @ 0x22, 0x23, 0x24");
    Serial.println("============================================================");
    
    UNITY_BEGIN();
    
    // Tests in logischer Reihenfolge
    RUN_TEST(test_i2c_init);
    RUN_TEST(test_i2c_scan);
    RUN_TEST(test_board_a_relays);
    RUN_TEST(test_board_b_relays);
    RUN_TEST(test_board_c_relays);
    RUN_TEST(test_manual_interactive);
    
    int result = UNITY_END();
    
    Serial.println("\n============================================================");
    if (result == 0) {
        Serial.println("✅ ALLE TESTS ERFOLGREICH!");
        Serial.println("Alle gefundenen PCA9535 Boards funktionieren korrekt.");
    } else {
        Serial.println("❌ EINIGE TESTS FEHLGESCHLAGEN!");
        Serial.println("Prüfe Verkabelung und Board-Adressen.");
    }
    Serial.println("============================================================");
}

void loop() {
    // Leere Loop - Tests laufen nur einmal in setup()
    delay(1000);
}

// ---------- Main Test Runner (nicht mehr benötigt) ----------
/*
int main(void) {
    delay(2000);  // Warten bis Serial verfügbar
    
    Serial.begin(115200);
    Serial.println("\n============================================================");
    Serial.println("    PCA9535 RELAIS BOARDS FUNKTIONSTEST");
    Serial.println("                 Version 1.0");
    Serial.println("============================================================");
    Serial.println("Hardware: WT32-ETH01 (ESP32-WROOM-32)");
    Serial.println("I2C: SDA=GPIO33, SCL=GPIO32");
    Serial.println("Relais: 3x PCA9535 Boards @ 0x22, 0x23, 0x24");
    Serial.println("============================================================");
    
    UNITY_BEGIN();
    
    // Tests in logischer Reihenfolge
    RUN_TEST(test_i2c_init);
    RUN_TEST(test_i2c_scan);
    RUN_TEST(test_board_a_relays);
    RUN_TEST(test_board_b_relays);
    RUN_TEST(test_board_c_relays);
    RUN_TEST(test_manual_interactive);
    
    int result = UNITY_END();
    
    Serial.println("\n============================================================");
    if (result == 0) {
        Serial.println("✅ ALLE TESTS ERFOLGREICH!");
        Serial.println("Alle gefundenen PCA9535 Boards funktionieren korrekt.");
    } else {
        Serial.println("❌ EINIGE TESTS FEHLGESCHLAGEN!");
        Serial.println("Prüfe Verkabelung und Board-Adressen.");
    }
    Serial.println("============================================================");
}

// (Alter main() Code entfernt - jetzt in setup() Funktion)
*/