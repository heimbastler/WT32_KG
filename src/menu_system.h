#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Menu page definitions for WT32-KG Smart Home Controller
enum MenuPage {
    PAGE_MAIN = 0,           // 1. Hauptanzeige (Relais Status, Netzwerk)
    PAGE_RELAYS,             // 2. Relais Steuerung (24 Relais)
    PAGE_INPUTS,             // 3. Digitale Eingänge (16 MCP23017)
    PAGE_TOUCH_STATUS,       // 4. Touch Panel Status (3x MPR121)
    PAGE_TEMPERATURE,        // 5. Temperatursensoren (DS18B20)
    PAGE_NETWORK,            // 6. Netzwerk Info (IP, WiFi)
    PAGE_WIFI_TOGGLE,        // 7. WiFi Aktivieren/Deaktivieren
    PAGE_SYSTEM_INFO,        // 8. System Information
    PAGE_RELAY_GROUPS,       // 9. Relais Gruppen (Alle Lampen, etc.)
    PAGE_GIT_INFO,           // 10. GitHub Repository Info + QR Code
    PAGE_ERROR_STATUS        // 11. Fehler & Diagnose
};

class MenuSystem {
private:
    Adafruit_SSD1306* display;
    MenuPage currentPage;
    int currentSelection;  // Aktuelle Auswahl im Menü (für Navigation)
    int scrollOffset;      // Für Scrolling bei langen Listen
    
    // Button states (mit Entprellung)
    bool button1Pressed, button2Pressed, button3Pressed, button4Pressed;
    unsigned long lastButtonTime;
    unsigned long lastActivityTime;
    
    // Externe Daten-Referenzen (werden von main.cpp gesetzt)
    uint8_t* relayStates;      // Zeiger auf relayState[24] aus main.cpp
    uint8_t* inputStates;      // Zeiger auf inputState[16] aus main.cpp
    float* temperatureValues;  // Zeiger auf Temperatur-Array
    
    // WiFi Status
    bool wifiEnabled;
    bool wifiConnected;
    String wifiSSID;
    String wifiIP;
    
    // Private Hilfsfunktionen
    void drawPage();
    void drawPageIndicator();
    void drawScrollBar(int totalItems, int visibleItems);
    
    // Individual page drawing functions
    void drawMainPage();
    void drawRelaysPage();
    void drawInputsPage();
    void drawTouchPage();
    void drawTemperaturePage();
    void drawNetworkPage();
    void drawWifiTogglePage();
    void drawSystemInfoPage();
    void drawRelayGroupsPage();
    void drawGitInfoPage();
    void drawErrorPage();
    
public:
    MenuSystem(Adafruit_SSD1306* disp);
    
    void init();
    void update();
    
    // Button handlers (4 Tasten auf OLED Modul)
    void handleButtonMenu();   // K1 (*) - Seite wechseln
    void handleButtonEnter();  // K2 (#) - Bestätigen/Aktion
    void handleButtonUp();     // K3 (↑) - Hoch/Erhöhen
    void handleButtonDown();   // K4 (↓) - Runter/Verringern
    
    // Getters
    MenuPage getCurrentPage();
    int getCurrentSelection();
    
    // Setters - Verbindung zu main.cpp Daten
    void setRelayStates(uint8_t* states);
    void setInputStates(uint8_t* states);
    void setTemperatureValues(float* temps);
    void setWifiStatus(bool enabled, bool connected, String ssid, String ip);
    
    // Getters
    bool isWifiEnabled();
    
    // Aktionen (werden von main.cpp implementiert)
    void (*onRelayToggle)(int relayIndex) = nullptr;  // Callback für Relais schalten
    void (*onGroupToggle)(int groupId) = nullptr;     // Callback für Gruppen schalten
    void (*onWifiToggle)(bool enable) = nullptr;      // Callback für WiFi an/aus
    
    // Display Update Flag
    bool needsRedraw;
};

// Global menu system instance
extern MenuSystem* menuSystem;

// Global functions
void initMenuSystem(Adafruit_SSD1306* display);
void updateMenuSystem();

#endif
