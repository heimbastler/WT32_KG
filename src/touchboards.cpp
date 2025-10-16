
#include "Adafruit_MPR121.h"
#include <PCF8574.h>

#define MPR_IRQ_PCF_PIN 0   // z.B. P0 vom pcfIn1 als IRQ Eingang

// Externe Referenzen zu Hardware-Objekten (definiert in main.cpp)
extern Adafruit_MPR121 cap1;
extern Adafruit_MPR121 cap2; 
extern Adafruit_MPR121 cap3;
extern PCF8574 pcfIn1;

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
extern void toggleWohnzimmerlampe1();
extern void toggleLamps();

// Lokaler Funktionsprototyp
void checkMPR(Adafruit_MPR121 &sensor, const char *name, int sensorIdx);

// ===================================================
// Touch Panel Belegung (3x MPR121)
// ===================================================

 /*
 TouchBoard 2: Säule Garten EG (I2C 0x5C)
 TouchBoard 3: Säule Strasse EG (I2C 0x5D)

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




TouchBoard 1: Tür Garten EG (I2C 0x5A)
 
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 ##                     ##                   ##
 ##    ??               ##    ??             ##
 ##       case          ##      case         ##
 ##                     ##                   ##
 ##############################################
 

case 2: toggleWohnzimmerlampe1(); break;    // oben 1te von links
case 5: toggleTraegerlampen(); break;       // oben 2te von links
case 1: toggleEGFlurlampe(); break;         // unten 1te von links
case 0: toggleAussenlampeGarten(); break;   // unten 2te von links
case 9: toggleFensterrolloUp(); break;      // Taster für Fensterrollo up
case 10: toggleFensterrolloDown(); break;   // Taster für Fensterrollo down
case 6: toggleTuerrolloUp(); break;         // push button for Türrollo up
case 11: toggleTuerrolloDown(); break;      // push button for Türrollo down

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
  // IRQ über PCF erkennen
  bool mprIRQ = (pcfIn1.digitalRead(MPR_IRQ_PCF_PIN) == LOW);
  if (mprIRQ) {
    checkMPR(cap1, "MPR121 #1", 0);
    checkMPR(cap2, "MPR121 #2", 1);
    checkMPR(cap3, "MPR121 #3", 2);
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