#include "menu_system.h"
#include <WiFi.h>

// Global menu system instance
MenuSystem* menuSystem = nullptr;

// Relais Namen aus main.cpp (extern)
extern const char* relayNames[24];

MenuSystem::MenuSystem(Adafruit_SSD1306* disp) {
    display = disp;
    currentPage = PAGE_MAIN;
    currentSelection = 0;
    scrollOffset = 0;
    
    // Initialize button states
    button1Pressed = button2Pressed = button3Pressed = button4Pressed = false;
    lastButtonTime = 0;
    lastActivityTime = millis();
    
    // Initialize data pointers
    relayStates = nullptr;
    inputStates = nullptr;
    temperatureValues = nullptr;
    
    // Initialize WiFi settings
    wifiEnabled = true;
    wifiConnected = false;
    wifiSSID = "";
    wifiIP = "";
    
    needsRedraw = true;
}

void MenuSystem::init() {
    // Initialize button pins
    // TODO: Pins müssen definiert werden je nach Hardware
    // pinMode(BUTTON_MENU_PIN, INPUT_PULLUP);  // K1 (*) Menu/Page
    // pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP); // K2 (#) Enter/Confirm
    // pinMode(BUTTON_UP_PIN, INPUT_PULLUP);    // K3 (↑) Up
    // pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);  // K4 (↓) Down
    
    Serial.println("=== WT32-KG Menu System Initialized ===");
    Serial.println("Button Layout: [K1]*  [K2]#  [K3]↑  [K4]↓");
    Serial.println("K1 (*) = Page change");
    Serial.println("K2 (#) = Action/Toggle");
    Serial.println("K3 (↑) = Up/Scroll");
    Serial.println("K4 (↓) = Down/Scroll");
    Serial.println("========================================");
}

void MenuSystem::update() {
    unsigned long currentTime = millis();
    
    // TODO: Button reading (wenn Pins definiert sind)
    // Hier würde die Button-Abfrage kommen wie im PID Projekt
    
    // Auto-return to main page after 30 seconds
    if(currentPage != PAGE_MAIN && currentTime - lastActivityTime > 30000) {
        currentPage = PAGE_MAIN;
        currentSelection = 0;
        scrollOffset = 0;
        needsRedraw = true;
    }
    
    // Draw current page if update needed
    if(needsRedraw) {
        drawPage();
        needsRedraw = false;
    }
}

void MenuSystem::drawPage() {
    if(!display) return;
    
    display->clearDisplay();
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    
    switch(currentPage) {
        case PAGE_MAIN:
            drawMainPage();
            break;
        case PAGE_RELAYS:
            drawRelaysPage();
            break;
        case PAGE_INPUTS:
            drawInputsPage();
            break;
        case PAGE_TOUCH_STATUS:
            drawTouchPage();
            break;
        case PAGE_TEMPERATURE:
            drawTemperaturePage();
            break;
        case PAGE_NETWORK:
            drawNetworkPage();
            break;
        case PAGE_WIFI_TOGGLE:
            drawWifiTogglePage();
            break;
        case PAGE_SYSTEM_INFO:
            drawSystemInfoPage();
            break;
        case PAGE_RELAY_GROUPS:
            drawRelayGroupsPage();
            break;
        case PAGE_GIT_INFO:
            drawGitInfoPage();
            break;
        case PAGE_ERROR_STATUS:
            drawErrorPage();
            break;
    }
    
    drawPageIndicator();
    display->display();
}

void MenuSystem::drawMainPage() {
    // Header
    display->setTextSize(1);
    display->setCursor(10, 0);
    display->println(F("WT32-KG Controller"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Netzwerk Status
    display->setCursor(0, 15);
    display->print(F("ETH: "));
    if(wifiIP.length() > 0) {
        display->print(wifiIP);
    } else {
        display->print(F("192.168.1.100")); // TODO: Echte IP
    }
    
    // WiFi Status Anzeige
    display->setCursor(0, 25);
    display->print(F("WiFi: "));
    if(wifiEnabled) {
        if(wifiConnected) {
            display->print(F("Verbunden"));
        } else {
            display->print(F("Nicht verbunden"));
        }
    } else {
        display->print(F("Deaktiviert"));
    }
    
    // Relais Status Summary
    display->setCursor(0, 35);
    int activeRelays = 0;
    if(relayStates) {
        for(int i = 0; i < 24; i++) {
            if(relayStates[i]) activeRelays++;
        }
    }
    display->printf("Relais: %d/24 EIN", activeRelays);
    
    // Input Status Summary
    display->setCursor(0, 43);
    int activeInputs = 0;
    if(inputStates) {
        for(int i = 0; i < 16; i++) {
            if(inputStates[i]) activeInputs++;
        }
    }
    display->printf("Inputs: %d/16 HIGH", activeInputs);
    
    // Temperature (wenn verfügbar)
    display->setCursor(0, 51);
    if(temperatureValues) {
        display->printf("Temp: %.1fC", temperatureValues[0]);
    } else {
        display->print(F("Temp: --.-C"));
    }
    
    // Footer
    display->setCursor(0, 56);
    display->print(F("* Menu  # Action"));
}

void MenuSystem::drawRelaysPage() {
    // Header
    display->setCursor(25, 0);
    display->println(F("RELAIS"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Zeige 4 Relais auf einmal
    int startIndex = scrollOffset;
    for(int i = 0; i < 4 && (startIndex + i) < 24; i++) {
        int relayIdx = startIndex + i;
        display->setCursor(0, 15 + i * 10);
        
        // Selection cursor
        if(relayIdx == currentSelection) {
            display->print(F(">"));
        } else {
            display->print(F(" "));
        }
        
        // Relais Name und Status
        display->printf("R%02d:", relayIdx);
        
        // Status
        if(relayStates && relayStates[relayIdx]) {
            display->print(F("EIN"));
        } else {
            display->print(F("AUS"));
        }
    }
    
    // Scrollbar
    drawScrollBar(24, 4);
    
    // Footer
    display->setCursor(0, 56);
    display->print(F("^v Scroll # Toggle"));
}

void MenuSystem::drawInputsPage() {
    // Header
    display->setCursor(20, 0);
    display->println(F("EINGAENGE"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Zeige 5 Eingänge auf einmal
    int startIndex = scrollOffset;
    for(int i = 0; i < 5 && (startIndex + i) < 16; i++) {
        int inputIdx = startIndex + i;
        display->setCursor(0, 13 + i * 9);
        
        // Pin Namen
        char pinName[10];
        if(inputIdx < 8) {
            sprintf(pinName, "IN%d (A%d)", inputIdx, inputIdx);
        } else {
            sprintf(pinName, "IN%d (B%d)", inputIdx, inputIdx - 8);
        }
        display->print(pinName);
        
        // Status
        display->setCursor(80, 13 + i * 9);
        if(inputStates && inputStates[inputIdx]) {
            display->print(F("HIGH"));
        } else {
            display->print(F("LOW"));
        }
    }
    
    // Scrollbar
    drawScrollBar(16, 5);
    
    // Footer
    display->setCursor(0, 56);
    display->print(F("^v Scroll * Back"));
}

void MenuSystem::drawTouchPage() {
    // Header
    display->setCursor(15, 0);
    display->println(F("TOUCH PANELS"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Touch Board Status
    display->setTextSize(1);
    display->setCursor(0, 15);
    display->println(F("Panel 1 (0x5A): OK"));
    display->println(F("  Tuer Garten EG"));
    
    display->setCursor(0, 33);
    display->println(F("Panel 2 (0x5C): OK"));
    display->println(F("  Saeule Garten"));
    
    display->setCursor(0, 51);
    display->println(F("Panel 3 (0x5D): OK"));
    
    // TODO: Echten Status der MPR121 Boards anzeigen
}

void MenuSystem::drawTemperaturePage() {
    // Header
    display->setCursor(15, 0);
    display->println(F("TEMPERATUR"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // DS18B20 Sensoren
    display->setCursor(0, 15);
    if(temperatureValues) {
        display->printf("Raum:     %.1fC\n", temperatureValues[0]);
        display->printf("Boden 1:  %.1fC\n", temperatureValues[1]);
        display->printf("Boden 2:  %.1fC\n", temperatureValues[2]);
        display->printf("Vorlauf:  %.1fC\n", temperatureValues[3]);
        display->printf("Ruecklauf:%.1fC", temperatureValues[4]);
    } else {
        display->println(F("Keine Sensoren"));
        display->println(F("gefunden"));
    }
}

void MenuSystem::drawNetworkPage() {
    // Header
    display->setCursor(25, 0);
    display->println(F("NETZWERK"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Ethernet Status
    display->setCursor(0, 15);
    display->println(F("Ethernet: OK"));
    if(wifiIP.length() > 0) {
        display->printf("IP: %s\n", wifiIP.c_str());
    } else {
        display->println(F("IP: 192.168.1.100")); // TODO: Echte IP
    }
    
    // WiFi Status (wenn aktiviert)
    display->setCursor(0, 35);
    display->printf("WiFi: %s\n", wifiEnabled ? "Aktiv" : "Aus");
    if(wifiEnabled && wifiSSID.length() > 0) {
        display->printf("SSID: %s", wifiSSID.c_str());
    }
    
    // MAC Adresse
    display->setCursor(0, 53);
    display->print(F("MAC: AA:BB:CC..."));
}

void MenuSystem::drawWifiTogglePage() {
    // Header
    display->setCursor(40, 0);
    display->println(F("WiFi"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Status
    display->setTextSize(1);
    display->setCursor(25, 20);
    display->print(F("Status: "));
    display->setTextSize(2);
    display->setCursor(20, 32);
    if(wifiEnabled) {
        display->print(F("EIN"));
    } else {
        display->print(F("AUS"));
    }
    
    // Verbindungsinfo (wenn aktiv)
    display->setTextSize(1);
    if(wifiEnabled) {
        display->setCursor(10, 48);
        if(wifiConnected) {
            display->print(F("Verbunden"));
        } else {
            display->print(F("Verbinde..."));
        }
    }
    
    // Footer
    display->setCursor(20, 56);
    display->print(F("# Umschalten"));
}

void MenuSystem::drawSystemInfoPage() {
    // Header
    display->setCursor(15, 0);
    display->println(F("SYSTEM INFO"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // System Details
    display->setCursor(0, 15);
    display->println(F("WT32-ETH01 ESP32"));
    display->println(F(""));
    display->println(F("I2C Geraete:"));
    display->println(F("  MCP23017 @ 0x20"));
    display->println(F("  PCA9535  @ 0x22-24"));
    display->println(F("  MPR121   @ 0x5A-5D"));
}

void MenuSystem::drawRelayGroupsPage() {
    // Header
    display->setCursor(10, 0);
    display->println(F("RELAIS GRUPPEN"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Gruppen
    display->setCursor(0, 15);
    if(currentSelection == 0) display->print(F(">"));
    else display->print(F(" "));
    display->println(F("Alle Lampen"));
    
    display->setCursor(0, 25);
    if(currentSelection == 1) display->print(F(">"));
    else display->print(F(" "));
    display->println(F("EG Lampen"));
    
    display->setCursor(0, 35);
    if(currentSelection == 2) display->print(F(">"));
    else display->print(F(" "));
    display->println(F("KG Lampen"));
    
    display->setCursor(0, 45);
    if(currentSelection == 3) display->print(F(">"));
    else display->print(F(" "));
    display->println(F("Alle AUS"));
    
    // Footer
    display->setCursor(0, 56);
    display->print(F("^v Select # Action"));
}

void MenuSystem::drawGitInfoPage() {
    // Header
    display->setCursor(20, 0);
    display->println(F("PROJEKT INFO"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // Project Name
    display->setTextSize(1);
    display->setCursor(15, 15);
    display->println(F("WT32-KG Controller"));
    
    // QR Code Placeholder (linke Seite)
    display->drawRect(5, 25, 35, 35, SSD1306_WHITE);
    display->setCursor(13, 37);
    display->setTextSize(1);
    display->print(F("QR"));
    
    // Repository Info (rechte Seite)
    display->setTextSize(1);
    display->setCursor(45, 26);
    display->println(F("github.com"));
    display->setCursor(45, 34);
    display->println(F("/heimbast"));
    display->setCursor(45, 42);
    display->println(F("ler/"));
    display->setCursor(45, 50);
    display->println(F("WT32_KG"));
    
    // Footer - Version Info
    display->setCursor(0, 58);
    display->print(F("v1.0  2025"));
}

void MenuSystem::drawErrorPage() {
    // Header
    display->setCursor(30, 0);
    display->println(F("STATUS"));
    display->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // System Status
    display->setCursor(0, 15);
    display->println(F("System: OK"));
    display->println(F("I2C: OK"));
    display->println(F("Ethernet: OK"));
    display->println(F("Relais: OK"));
    display->println(F("Touch: OK"));
}

void MenuSystem::drawPageIndicator() {
    // Zeige Seitennummer unten rechts
    display->setCursor(105, 56);
    display->printf("%d/11", (int)currentPage + 1);
}

void MenuSystem::drawScrollBar(int totalItems, int visibleItems) {
    // Scrollbar rechts
    if(totalItems <= visibleItems) return;
    
    int barHeight = (visibleItems * 46) / totalItems;
    int barPos = (scrollOffset * 46) / totalItems;
    
    display->drawRect(126, 12, 2, 46, SSD1306_WHITE);
    display->fillRect(126, 12 + barPos, 2, barHeight, SSD1306_WHITE);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         BUTTON HANDLERS                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void MenuSystem::handleButtonMenu() {
    // K1 (*) - Nächste Seite
    currentPage = (MenuPage)((currentPage + 1) % 11); // Jetzt 11 Seiten
    currentSelection = 0;
    scrollOffset = 0;
    needsRedraw = true;
    lastActivityTime = millis();
    
    Serial.printf("Menu: Page %d\n", currentPage);
}

void MenuSystem::handleButtonEnter() {
    // K2 (#) - Aktion ausführen
    lastActivityTime = millis();
    
    switch(currentPage) {
        case PAGE_RELAYS:
            // Toggle selected relay
            if(onRelayToggle) {
                onRelayToggle(currentSelection);
            }
            Serial.printf("Toggle Relay R%02d\n", currentSelection);
            break;
            
        case PAGE_RELAY_GROUPS:
            // Toggle group
            if(onGroupToggle) {
                onGroupToggle(currentSelection);
            }
            Serial.printf("Toggle Group %d\n", currentSelection);
            break;
        
        case PAGE_WIFI_TOGGLE:
            // Toggle WiFi
            wifiEnabled = !wifiEnabled;
            if(onWifiToggle) {
                onWifiToggle(wifiEnabled);
            }
            Serial.printf("WiFi %s\n", wifiEnabled ? "ENABLED" : "DISABLED");
            break;
            
        default:
            break;
    }
    
    needsRedraw = true;
}

void MenuSystem::handleButtonUp() {
    // K3 (↑) - Hoch scrollen
    lastActivityTime = millis();
    
    if(currentPage == PAGE_RELAYS) {
        if(currentSelection > 0) {
            currentSelection--;
            if(currentSelection < scrollOffset) {
                scrollOffset = currentSelection;
            }
        }
    } else if(currentPage == PAGE_INPUTS) {
        if(currentSelection > 0) {
            currentSelection--;
            if(currentSelection < scrollOffset) {
                scrollOffset = currentSelection;
            }
        }
    } else if(currentPage == PAGE_RELAY_GROUPS) {
        if(currentSelection > 0) {
            currentSelection--;
        }
    }
    
    needsRedraw = true;
    Serial.printf("Up: Selection %d\n", currentSelection);
}

void MenuSystem::handleButtonDown() {
    // K4 (↓) - Runter scrollen
    lastActivityTime = millis();
    
    if(currentPage == PAGE_RELAYS) {
        if(currentSelection < 23) {
            currentSelection++;
            if(currentSelection >= scrollOffset + 4) {
                scrollOffset = currentSelection - 3;
            }
        }
    } else if(currentPage == PAGE_INPUTS) {
        if(currentSelection < 15) {
            currentSelection++;
            if(currentSelection >= scrollOffset + 5) {
                scrollOffset = currentSelection - 4;
            }
        }
    } else if(currentPage == PAGE_RELAY_GROUPS) {
        if(currentSelection < 3) {
            currentSelection++;
        }
    }
    
    needsRedraw = true;
    Serial.printf("Down: Selection %d\n", currentSelection);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         GETTERS & SETTERS                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

MenuPage MenuSystem::getCurrentPage() {
    return currentPage;
}

int MenuSystem::getCurrentSelection() {
    return currentSelection;
}

void MenuSystem::setRelayStates(uint8_t* states) {
    relayStates = states;
}

void MenuSystem::setInputStates(uint8_t* states) {
    inputStates = states;
}

void MenuSystem::setTemperatureValues(float* temps) {
    temperatureValues = temps;
}

void MenuSystem::setWifiStatus(bool enabled, bool connected, String ssid, String ip) {
    wifiEnabled = enabled;
    wifiConnected = connected;
    wifiSSID = ssid;
    wifiIP = ip;
    needsRedraw = true;
}

bool MenuSystem::isWifiEnabled() {
    return wifiEnabled;
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                         GLOBAL FUNCTIONS                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void initMenuSystem(Adafruit_SSD1306* display) {
    menuSystem = new MenuSystem(display);
    menuSystem->init();
}

void updateMenuSystem() {
    if(menuSystem) {
        menuSystem->update();
    }
}
