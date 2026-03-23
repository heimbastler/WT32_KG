
#include "Adafruit_MPR121.h"
#include <Adafruit_MCP23X17.h>

// ╔══════════════════════════════════════════════════════════════════════════╗
// ║                   MPR121 IRQ KONFIGURATION                               ║
// ╚══════════════════════════════════════════════════════════════════════════╝
// 
// OPTION 1: Direkter GPIO16 Interrupt (VORBEREITET - bevorzugt)
// --------------------------------------------------------------
// - Alle 3 MPR121 IRQ-Leitungen via Wired-OR + Level Shifter → GPIO16
// - Interrupt-Handler in main.cpp: mpr121ISR()
// - Flag: mpr121InterruptFlag
// - Vorteil: Schnelle Reaktion, keine MCP23017 GPA7 belegt
//
// OPTION 2: Via MCP23017 GPA7 (LEGACY - veraltet)
// ------------------------------------------------
// - MPR121 IRQs → GPA7 → ESP32 via MCP23017 Interrupt
// - Nachteil: Umweg über zusätzlichen I²C-Chip
//
// AKTUELL: Polling-Modus (keine Hardware angeschlossen)
// ======================================================

// Externe Referenzen zu Hardware-Objekten (definiert in main.cpp)
extern Adafruit_MPR121 cap1;
extern Adafruit_MPR121 cap2; 
extern Adafruit_MPR121 cap3;
extern Adafruit_MCP23X17 mcpIn;

// Externe Referenzen zu Relay-Funktionen (definiert in main.cpp)
extern void toggleFensterrolloUp();
extern void toggleFensterrolloDown();
extern void toggleTuerrolloUp();
extern void toggleTuerrolloDown();
extern void toggleAussenlampeGarten();
extern void toggleSteinlampe();
extern void toggleKGFlurlampe();
extern void toggleKuechenarbeitslampe();
extern void toggleKuechenlampe();
extern void toggleEGFlurlampe();
extern void toggleTraegerlampen();
extern void toggleKronleuchter();
extern void dimKronleuchter(bool dimUp);
extern void toggleLamps();

// Lokale Funktionsprototypen
void checkMPR(Adafruit_MPR121 &sensor, const char *name, int sensorIdx);
void handleKronleuchterTouchEvent(bool isTouched);

// ===================================================
// Touch Panel Belegung (3x MPR121)
// ===================================================

 /*
 TouchBoard 2: Säule Garten EG (I2C 0x5C)
 #############################################
 ##                     ##                  ##
 ##    toggleLamps      ##    toggleLamps   ##
 ##       case 9        ##      case 11     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##     Steinlampe      ##   Kronleuchter   ##
 ##       case 6        ##      case 10     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##    Kuechenlampe     ##    EGFlurlampe   ##
 ##       case 2        ##      case 0      ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ## Kuechenarbeitslampe ##   Traegerlampen  ##
 ##       case 3        ##      case 1      ##
 ##                     ##                  ##
 #############################################



TouchBoard 3: Säule Strasse EG (I2C 0x5D)
 #############################################
 ##                     ##                  ##
 ##    toggleLamps      ##    toggleLamps   ##
 ##       case 9        ##      case 10     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##     Steinlampe      ##   Kronleuchter   ##
 ##       case 6        ##      case 11     ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ##    Kuechenlampe     ##    EGFlurlampe   ##
 ##       case 3        ##      case 0      ##
 ##                     ##                  ##
 #############################################
 ##                     ##                  ##
 ## Kuechenarbeitslampe ##   Traegerlampen  ##
 ##       case 2        ##      case 1      ##
 ##                     ##                  ##
 #############################################



TouchBoard 1: Tür Garten EG (I2C 0x5A)
 ##########################################################################################
 ##                     ##                   ##                     ##                   ##
 ##    Kronleuchter     ##    Traegerlampen  ##    TuerrolloUp      ##   FensterrolloUp  ##
 ##       case 2        ##      case 5       ##       case 6        ##      case 9       ##
 ##                     ##                   ##                     ##                   ##
 ##########################################################################################
 ##                     ##                   ##                     ##                   ##
 ##    EGFlurlampe      ## AussenlampeGarten ##   TuerrolloDown     ## FensterrolloDown  ##
 ##       case 1        ##      case 0       ##       case 11       ##      case 10      ##
 ##                     ##                   ##                     ##                   ##
 ##########################################################################################
 



*/

// ======================================================
// Touch Board Initialisierung
// ======================================================
void initTouchBoards() {
  // MPR121 Sensoren starten
  cap1.begin(0x5A);
  cap2.begin(0x5C);
  cap3.begin(0x5D);
  
  Serial.println("TouchBoards initialisiert");
}

// ======================================================
// Touch Events verarbeiten
// ======================================================
void handleTouchEvents() {
  // ═══════════════════════════════════════════════════════════════
  // WENN MPR121 AKTIVIERT: Wähle eine der folgenden Optionen:
  // ═══════════════════════════════════════════════════════════════
  
  // OPTION 1: Direkter GPIO16 Interrupt (EMPFOHLEN - in main.cpp aktivieren)
  // -------------------------------------------------------------------------
  // Diese Funktion wird nur aufgerufen wenn mpr121InterruptFlag gesetzt ist
  // Kein IRQ-Check nötig, direkt alle 3 Boards prüfen:
  //
  // checkMPR(cap1, "MPR121 #1", 0);
  // checkMPR(cap2, "MPR121 #2", 1);
  // checkMPR(cap3, "MPR121 #3", 2);
  
  // OPTION 2: Legacy - Via MCP23017 GPA7 (NICHT EMPFOHLEN)
  // -------------------------------------------------------
  // #define MPR_IRQ_MCP_PIN 7  // Oben definieren
  // bool mprIRQ = (mcpIn.digitalRead(MPR_IRQ_MCP_PIN) == LOW);
  // if (mprIRQ) {
  //   checkMPR(cap1, "MPR121 #1", 0);
  //   checkMPR(cap2, "MPR121 #2", 1);
  //   checkMPR(cap3, "MPR121 #3", 2);
  // }
  
  // AKTUELL: Polling-Modus (Hardware nicht angeschlossen)
  // ------------------------------------------------------
  // Funktion tut nichts, da keine MPR121 Hardware vorhanden
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
          if (i == 2) {
            // Spezielle Behandlung für Kronleuchter (case 2)
            handleKronleuchterTouchEvent(isTouched);
          } else if (isTouched) {
            switch (i) {
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
          if (i == 10) {
            // Spezielle Behandlung für Kronleuchter (case 10)
            handleKronleuchterTouchEvent(isTouched);
          } else if (isTouched) {
            switch (i) {
              case 9: toggleLamps(); break;               // oben links
              case 11: toggleLamps(); break;              // oben rechts
              case 1: toggleTraegerlampen(); break;       // unten rechts
              case 2: toggleKuechenlampe(); break;        // 3te links
              case 3: toggleKuechenarbeitslampe(); break; // unten links
              case 0: toggleEGFlurlampe(); break;         // 3te rechts
              case 6: toggleSteinlampe(); break;          // 2te links
            }
          }
        } else if (sensorIdx == 2) { // TouchBoard 3: Säule Strasse EG
          if (i == 11) {
            // Spezielle Behandlung für Kronleuchter (case 11)
            handleKronleuchterTouchEvent(isTouched);
          } else if (isTouched) {
            switch (i) {
              case 9: toggleLamps(); break;               // oben links
              case 10: toggleLamps(); break;              // oben rechts
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

// ======================================================
// Kronleuchter Touch Handler (mit Touch-Zeit-Erkennung)
// ======================================================
void handleKronleuchterTouchEvent(bool isTouched) {
  static unsigned long touchStartTime = 0;
  static bool longPressActive = false;
  
  extern unsigned long lastTouchTime;
  extern const unsigned long touchDebounceTime;
  extern bool kronleuchterDimmingUp;
  extern uint8_t kronleuchterBrightness;
  
  unsigned long currentTime = millis();
  
  if (isTouched) {
    // Touch PRESSED
    if (touchStartTime == 0) {
      touchStartTime = currentTime;
      longPressActive = false;
      Serial.println("🔆 Kronleuchter Touch START");
    }
    
    // Prüfe auf langen Druck (nach 800ms)
    unsigned long touchDuration = currentTime - touchStartTime;
    if (touchDuration > 800 && !longPressActive) {
      longPressActive = true;
      Serial.println("🔆 Kronleuchter LANG-TOUCH → Dimmen startet");
      
      // Dimm-Richtung bestimmen
      if (kronleuchterBrightness >= 240) {
        kronleuchterDimmingUp = false; // Runter dimmen
        Serial.println("🔆 Dimm-Richtung: RUNTER");
      } else if (kronleuchterBrightness <= 15) {
        kronleuchterDimmingUp = true;  // Hoch dimmen  
        Serial.println("🔆 Dimm-Richtung: HOCH");
      }
      // Sonst aktuelle Richtung beibehalten
      
      dimKronleuchter(kronleuchterDimmingUp);
    } else if (longPressActive && (touchDuration % 300) < 50) {
      // Kontinuierliches Dimmen alle 300ms
      dimKronleuchter(kronleuchterDimmingUp);
    }
    
  } else {
    // Touch RELEASED
    if (touchStartTime > 0) {
      unsigned long touchDuration = currentTime - touchStartTime;
      
      if (!longPressActive && touchDuration < 800) {
        // Kurzer Touch → Toggle (AUS → 50% → AUS)
        toggleKronleuchter();
        Serial.println("🔆 Kronleuchter KURZ-TOUCH → Toggle");
      } else if (longPressActive) {
        Serial.println("🔆 Kronleuchter LANG-TOUCH beendet");
      }
      
      // Reset für nächsten Touch
      touchStartTime = 0;
      longPressActive = false;
      lastTouchTime = currentTime;
    }
  }
}