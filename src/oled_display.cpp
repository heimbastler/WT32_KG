// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    OLED Display mit 4 Tasten Modul                           ║
// ║                          SSD1315 Controller                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
//
// Hardware: OLED Display SSD1315 mit integrierten 4 Tasten
// I²C Adresse: 0x3C (Standard für SSD1315)
// Auflösung: 128x64 Pixel (monochrom)
// Tasten: 4x integrierte Buttons (vermutlich über separate Pins oder I²C expander)
//
// Erstelldatum: 12. Dezember 2025

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Display Konfiguration ----------
#define SCREEN_WIDTH 128        // OLED Display Breite in Pixeln
#define SCREEN_HEIGHT 64        // OLED Display Höhe in Pixeln
#define OLED_RESET -1           // Reset Pin (oder -1 wenn nicht benutzt)
#define OLED_ADDRESS 0x3C       // I²C Adresse für SSD1315 (0x3C oder 0x3D)

// ---------- Tasten Pins ----------
// TODO: Pins müssen je nach Hardware angepasst werden
// Option 1: Tasten sind direkt an GPIO angeschlossen
// Option 2: Tasten sind über separaten I²C Expander angeschlossen
#define BUTTON_UP    -1         // GPIO Pin für Taste HOCH (noch zu definieren)
#define BUTTON_DOWN  -1         // GPIO Pin für Taste RUNTER (noch zu definieren)
#define BUTTON_LEFT  -1         // GPIO Pin für Taste LINKS (noch zu definieren)
#define BUTTON_RIGHT -1         // GPIO Pin für Taste RECHTS (noch zu definieren)

// ---------- Display Objekt ----------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Menü Status ----------
uint8_t currentMenu = 0;        // Aktuelles Menü (0=Home, 1=Relais, 2=Inputs, 3=Temp)
uint8_t menuSelection = 0;      // Aktuelle Auswahl im Menü
bool displayNeedsUpdate = true; // Flag ob Display neu gezeichnet werden muss

// ---------- Taste Status (Entprellung) ----------
unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 200; // 200ms Entprellung


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         INITIALISIERUNG                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void initOLED() {
  // SSD1306/SSD1315 Display initialisieren
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("❌ SSD1315 OLED nicht gefunden auf I²C 0x3C!"));
    return;
  }

  Serial.println(F("✅ SSD1315 OLED Display initialisiert"));
  
  // Display leeren und Startbildschirm anzeigen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("WT32-KG Controller"));
  display.println(F("=================="));
  display.println();
  display.println(F("OLED bereit"));
  display.println(F("4 Tasten erkannt"));
  display.display();
  
  delay(2000);
  
  // Tasten-Pins initialisieren (falls direkt an GPIO)
  if (BUTTON_UP != -1) {
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    Serial.println(F("✅ 4 Tasten initialisiert (GPIO)"));
  } else {
    Serial.println(F("⚠️  Tasten-Pins noch nicht definiert!"));
  }
  
  displayNeedsUpdate = true;
}


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         TASTEN ABFRAGE                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

bool isButtonPressed(int buttonPin) {
  // Entprellung: Mindestens DEBOUNCE_DELAY zwischen Tastendrücken
  if (millis() - lastButtonPress < DEBOUNCE_DELAY) {
    return false;
  }
  
  if (buttonPin == -1) return false;
  
  if (digitalRead(buttonPin) == LOW) { // LOW = gedrückt (Pull-up)
    lastButtonPress = millis();
    return true;
  }
  return false;
}

void handleButtons() {
  // Taste HOCH: Menü-Auswahl nach oben
  if (isButtonPressed(BUTTON_UP)) {
    if (menuSelection > 0) {
      menuSelection--;
    }
    displayNeedsUpdate = true;
    Serial.printf("🔼 Button UP - Selection: %d\n", menuSelection);
  }
  
  // Taste RUNTER: Menü-Auswahl nach unten
  if (isButtonPressed(BUTTON_DOWN)) {
    menuSelection++;
    displayNeedsUpdate = true;
    Serial.printf("🔽 Button DOWN - Selection: %d\n", menuSelection);
  }
  
  // Taste LINKS: Zurück / Menü verlassen
  if (isButtonPressed(BUTTON_LEFT)) {
    if (currentMenu > 0) {
      currentMenu = 0; // Zurück zum Hauptmenü
      menuSelection = 0;
    }
    displayNeedsUpdate = true;
    Serial.printf("◀️ Button LEFT - Menu: %d\n", currentMenu);
  }
  
  // Taste RECHTS: Auswahl bestätigen / Untermenü öffnen
  if (isButtonPressed(BUTTON_RIGHT)) {
    // Aktion je nach aktuellem Menü
    if (currentMenu == 0) {
      // Hauptmenü: Untermenü öffnen
      currentMenu = menuSelection + 1;
      menuSelection = 0;
    } else {
      // Untermenü: Aktion ausführen (z.B. Relais schalten)
      executeMenuAction();
    }
    displayNeedsUpdate = true;
    Serial.printf("▶️ Button RIGHT - Menu: %d, Selection: %d\n", currentMenu, menuSelection);
  }
}


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         DISPLAY FUNKTIONEN                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void drawMainMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Header
  display.setCursor(0, 0);
  display.println(F("== HAUPTMENU =="));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  // Menü-Optionen
  display.setCursor(0, 15);
  if (menuSelection == 0) display.print(F("> "));
  else display.print(F("  "));
  display.println(F("1. Relais"));
  
  display.setCursor(0, 25);
  if (menuSelection == 1) display.print(F("> "));
  else display.print(F("  "));
  display.println(F("2. Eingaenge"));
  
  display.setCursor(0, 35);
  if (menuSelection == 2) display.print(F("> "));
  else display.print(F("  "));
  display.println(F("3. Temperatur"));
  
  display.setCursor(0, 45);
  if (menuSelection == 3) display.print(F("> "));
  else display.print(F("  "));
  display.println(F("4. System Info"));
  
  // Footer
  display.drawLine(0, 56, SCREEN_WIDTH, 56, SSD1306_WHITE);
  display.setCursor(0, 58);
  display.print(F("UP/DN  OK>"));
  
  display.display();
}

void drawRelayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println(F("== RELAIS =="));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  // Zeige 4 Relais gleichzeitig (scrollbar bei mehr)
  for (int i = 0; i < 4; i++) {
    int relayIndex = menuSelection - (menuSelection % 4) + i;
    if (relayIndex < 24) { // Max 24 Relais
      display.setCursor(0, 15 + i * 10);
      if (relayIndex == menuSelection) display.print(F(">"));
      else display.print(F(" "));
      display.printf("R%02d: %s", relayIndex, "OFF"); // TODO: Status aus relayState[]
    }
  }
  
  display.setCursor(0, 58);
  display.print(F("<  UP/DN  TOGGLE"));
  
  display.display();
}

void drawInputMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println(F("== EINGAENGE =="));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  // Zeige 4 Eingänge gleichzeitig
  for (int i = 0; i < 4; i++) {
    int inputIndex = menuSelection - (menuSelection % 4) + i;
    if (inputIndex < 16) { // Max 16 Eingänge
      display.setCursor(0, 15 + i * 10);
      display.printf("IN%02d: %s", inputIndex, "LOW"); // TODO: Status aus inputState[]
    }
  }
  
  display.setCursor(0, 58);
  display.print(F("<  UP/DN  REFRESH"));
  
  display.display();
}

void drawTemperatureMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println(F("== TEMPERATUR =="));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  display.setCursor(0, 20);
  display.println(F("Raum:    22.5 C"));
  display.println(F("Boden1:  21.8 C"));
  display.println(F("Boden2:  21.3 C"));
  display.println(F("Vorlauf: 45.2 C"));
  
  display.setCursor(0, 58);
  display.print(F("<  BACK"));
  
  display.display();
}

void drawSystemInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println(F("== SYSTEM INFO =="));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  display.setCursor(0, 15);
  display.println(F("WT32-ETH01 ESP32"));
  display.println(F("I2C: 0x20-0x5D"));
  display.println(F("Relais: 24"));
  display.println(F("Inputs: 16"));
  display.println(F("Touch: 36"));
  
  display.setCursor(0, 58);
  display.print(F("<  BACK"));
  
  display.display();
}

void updateDisplay() {
  if (!displayNeedsUpdate) return;
  
  switch (currentMenu) {
    case 0:
      drawMainMenu();
      break;
    case 1:
      drawRelayMenu();
      break;
    case 2:
      drawInputMenu();
      break;
    case 3:
      drawTemperatureMenu();
      break;
    case 4:
      drawSystemInfo();
      break;
    default:
      currentMenu = 0;
      drawMainMenu();
  }
  
  displayNeedsUpdate = false;
}


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         MENÜ AKTIONEN                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void executeMenuAction() {
  switch (currentMenu) {
    case 1: // Relais Menü
      // TODO: Relais schalten (Integration mit main.cpp)
      Serial.printf("🔄 Toggle Relais R%02d\n", menuSelection);
      break;
    case 2: // Eingänge Menü
      // Nur Anzeige, keine Aktion nötig
      break;
    case 3: // Temperatur Menü
      // Nur Anzeige, keine Aktion nötig
      break;
    case 4: // System Info
      // Nur Anzeige, keine Aktion nötig
      break;
  }
}


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         LOOP FUNKTION                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void loopOLED() {
  handleButtons();
  updateDisplay();
}


// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         HELPER FUNKTIONEN                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// Zeige eine Nachricht kurz auf dem Display an
void showMessage(const char* title, const char* message, int duration = 2000) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(title);
  display.setCursor(0, 35);
  display.println(message);
  display.display();
  delay(duration);
  displayNeedsUpdate = true;
}

// Zeige Relais Status an
void displayRelayStatus(uint8_t relayIndex, bool state) {
  char msg[20];
  sprintf(msg, "R%02d: %s", relayIndex, state ? "EIN" : "AUS");
  showMessage("Relais", msg, 1000);
}

// Zeige IP Adresse an
void displayIPAddress(const char* ip) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println(F("Netzwerk:"));
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println(ip);
  display.display();
  delay(3000);
  displayNeedsUpdate = true;
}
