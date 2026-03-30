# WT32_KG - Smart Home Controller

## 📋 Beschreibung
Ein intelligentes Hausautomatisierungssystem basierend auf dem **WT32-ETH01 (ESP32)** Mikrocontroller. Das System steuert verschiedene Haushaltsgeräte wie Lampen, Rollos und andere elektrische Verbraucher über I²C-Relaismodule und kapazitive Touch-Sensoren.

## ⚙️ Hardware

### 🖥️ Hauptcontroller: WT32-ETH01 (ESP32) – Version 1.4
<img src="pictures/WT32-ETH01_pinout_LL.png" alt="WT32-ETH01 Pinout" width="100%">

**Features:**
- Ethernet-Verbindung über LAN8720 PHY
- I²C Kommunikation (GPIO32/SCL, GPIO33/SDA)
- Integrierte WiFi-Funktionalität
- 240MHz Dual-Core Prozessor

**PINMAP WT32-ETH01**

```
                        ┌──────────────────────────┐
                        │      ╔═══════════╗       │
                        │      ║   ESP32   ║       │
                        │      ║   WROOM   ║       │
                        │      ║     S1    ║       │
                        │      ╚═══════════╝       │
                        ├──────────────────────────┤
                        │                          │
    EN (Programming)  1 │●                        ●│ 24  TXD (GPIO1)   - I/O, Prog & Debug
    GND (Masse)       2 │●                        ●│ 23  RXD (GPIO3)   - I/O, Prog & Debug  
    3V3 (+3.3V)       3 │●                        ●│ 22  IO0 (GPIO0)   - LAN REFCLK0
    EN (Programming)  4 │●                        ●│ 21  GND (Masse)
    CFG (GPIO32)      5 │●                        ●│ 20  IO39 (GPIO39) - Input only
    485_EN (GPIO33)   6 │●                        ●│ 19  IO36 (GPIO36) - Input only
    ---             7 │●                        ●│ 18  IO14 (GPIO14) - MTDO / Boot Debug Output
    TXD (GPIO17)      8 │●                        ●│ 17  IO15 (GPIO15) - PWM LED Dimmer
    GND (Masse)       9 │●                        ●│ 16  IO12 (GPIO12) - NO BOOT if HIGH
    3V3 (+3.3V)      10 │●                        ●│ 15  IO35 (GPIO35) - Input only
    GND (Masse)      11 │●                        ●│ 14  IO14 (GPIO14) - MTDO
    5V (+5V In)      12 │●                        ●│ 13  IO2  (GPIO2)  - NO PROG if HIGH
                        │                          │
                        ├───┐                  ┌───┤
                        │   │   ╔══════════╗   │   │
                        │   │   ║   RJ45   ║   │   │
                        │   │   ║ Ethernet ║   │   │
                        │   │   ╚══════════╝   │   │
                        └───┘                  └───┘
```

**Funktionsbeschreibung:**

| Pin | GPIO   | Typ                | Beschreibung                                    |
|-----|--------|--------------------|-------------------------------------------------|
| 1   | EN     | Input              | Enable/Reset (Programming)                      |
| 2   | GND    | Power              | Ground                                          |
| 3   | 3V3    | Power              | 3.3V Versorgungsspannung                        |
| 4   | EN     | Input              | Enable/Reset (Programming)                      |
| 5   | GPIO32 | I/O (**SCL**)      | **I²C Clock** (Pull-up extern 4.7kΩ)            |
| 6   | GPIO33 | I/O (**SDA**)      | **I²C Data** (Pull-up extern 4.7kΩ)             |
| 17  | GPIO35 | Input-only + **1-Wire** | **DS18B20 Temp-Sensor + 4.7kΩ Pull-up zu 3.3V**|
| 8   | GPIO17 | I/O                | Status LED OnBoard (aktiv HIGH)                 |
| 9   | GND    | Power              | Ground                                          |
| 10  | 3V3    | Power              | 3.3V Versorgungsspannung                        |
| 11  | GND    | Power              | Ground                                          |
| 12  | 5V     | Power              | 5V Eingang (VIN)                                |
| 13  | GPIO2  | I/O                | Boot Mode Control (LOW beim Programmieren)      |
| 14  | GPIO4  | I/O + **PWM**      | **AC Dimmer YYAC-3S (220V Kronleuchter)**       |
| 15  | GPIO35 | Input only         | ADC, kein Pull-up                               |
| 16  | GPIO12 | I/O ⚠️             | ⚠️ **KEIN Pull-up! Boot fail wenn HIGH!**      |
| 17  | GPIO15 | I/O + **PWM**      | **LED Dimmer HW-517 V0.0.1 (MOSFET)**           |
| 18  | GPIO14 | I/O                | MTDO / Boot Debug Output (optional verfügbar) |
| 19  | GPIO36 | Input only         | ADC, kein Pull-up                               |
| 20  | GPIO39 | Input only         | ADC, kein Pull-up                               |
| 21  | GND    | Power              | Ground                                          |
| 22  | GPIO0  | I/O                | LAN8720 REFCLK0 (Boot Mode beim Flash: LOW)     |
| 23  | GPIO3  | I/O                | UART RXD (Programming & Debug)                  |
| 24  | GPIO1  | I/O                | UART TXD (Programming & Debug)                  |

> **⚠️ HINWEIS:** Laut Diagramm sind **IO5 und IO35** im offiziellen Schaltplan **vertauscht**!  
> Oben ist die **korrigierte Belegung** aufgeführt.

**🔌 Zusätzliche GPIO-Zuordnungen (nicht auf 24-Pin-Header):**

| GPIO   | Funktion          | Beschreibung                                     |
|--------|-------------------|--------------------------------------------------|
| GPIO13 | **IRQ Input**     | **MCP23017 INTA/INTB** (Schalter/Taster Events) |
| GPIO16 | **IRQ Input**     | **MPR121 Wired-OR IRQ** (3x Touch Panels)       |

**Wichtige Boot-Pin-Einschränkungen:**
- **GPIO0**: Muss LOW sein beim Flash-Vorgang (Programmierung)
- **GPIO2**: Darf kein Pull-up beim Programmieren haben  
- **GPIO12**: ⚠️ **KRITISCH! Boot fail wenn HIGH beim Start - NIEMALS Pull-up verwenden!**
- **GPIO14**: MTDO - Boot Debug Output

**Warum GPIO35 statt GPIO12 für 1-Wire DS18B20?**
- DS18B20 benötigt **4.7kΩ Pull-up zu 3.3V** für 1-Wire Kommunikation
- **GPIO12** ist ein Boot-Mode-Pin: HIGH beim Boot = Boot failure!
- **GPIO35** ist ein Input-only Pin (ideal für 1-Wire Read-only) und hat keine Boot-Einschränkungen
- Alternativen: GPIO13, GPIO16, GPIO4, GPIO15 (alle sicher für Pull-up, aber Output-fähig)

**Interrupt-GPIO-Zuordnungen:**
- **GPIO13**: MCP23017 INTA/INTB → Triggert bei Schalter/Taster-Ereignissen (GPA0-7, GPB0-7)
- **GPIO16**: MPR121 Wired-OR → Triggert bei Touch auf einem der 3 Panels (kombinierter IRQ über Level Shifter)

### 🔌 Relais Boards: XL9535-K1V5
8 Kanal Erweiterungsrelais Modul 5V Netzteil I²C Kommunikation Optokoppler Isolation Board

here is a very good documentations about this type of boards:
https://github.com/mcauser/micropython-xl9535-kxv5-relay

<img src="pictures/Board XL9535-K8V5.png" alt="Relais Board XL9535-K8V5" width="30%">

#### Verkabelung: 3x PCA9535 Relais Boards (Daisy Chain)

**I²C-Adressen:**
- Board A: `0x22` (A1 gelötet, A0+A2 offen)
- Board B: `0x23` (A1+A0 gelötet, A2 offen)  
- Board C: `0x24` (A2 gelötet, A0+A1 offen)

### 📥 Input Expander: MCP23017 (1x 16-Bit GPIO)
16-Bit I/O Expander für digitale Eingänge (Schalter, Taster, Kreuzschaltungen) und Reserve-Ausgänge

<img src="pictures/CJMCU-2317_01.jpg" alt="MCP23017 Board - Vorderseite" width="20%">
<img src="pictures/CJMCU-2317_02.jpg" alt="MCP23017 Board - Rückseite" width="20%">
<img src="pictures/CJMCU-2317_03.jpg" alt="MCP23017 Board - Pinout" width="30%">

#### MCP23017 Pin-Belegung

**I²C-Adresse:** `0x20` (Standard - A2, A1, A0 alle auf GND)

**Port A (GPA0-GPA7) - Schalter/Taster:**
- `GPA0`: IR-Switch Küche EG11 - Linker Taster (Input mit Pull-Up)
- `GPA1`: IR-Switch Küche EG11 - Rechter Taster (Input mit Pull-Up)
- `GPA2`: Kreuzschaltung EG1 - Taster Treppe OG (Input mit Pull-Up)
- `GPA3`: Kreuzschaltung EG2 - Taster Eingang EG (Input mit Pull-Up)
- `GPA4`: Kreuzschaltung KG1 - Taster Tür Schlafzimmer (Input mit Pull-Up)
- `GPA5`: Kreuzschaltung KG2 - Taster Bad KG (Input mit Pull-Up)
- `GPA6`: Kreuzschaltung KG3 - Taster Treppe KG-EG (Input mit Pull-Up)
- `GPA7`: Reserve (früher MPR121 IRQ, jetzt auf GPIO16)

**Port B (GPB0-GPB7) - Reserve:**
- `GPB0-GPB7`: Reserve für zukünftige Eingänge/Ausgänge

**MCP23017 Interrupt:**
- `INTA/INTB`: Verbunden mit **GPIO13** (ESP32) - triggert bei Änderung auf GPA0-7 oder GPB0-7

⚠️ **WICHTIG - Level Shifter erforderlich für MPR121 Touchboards!**
Die 3 MPR121 Touchboards arbeiten mit **5V I²C und 5V IRQ Logik**, der ESP32 mit **3.3V Logik**.
- **Alle 3 MPR121 IRQ-Leitungen werden mit Wired-OR kombiniert** → **GPIO16** (ESP32) über Level Shifter
- **Ein bidirektionaler Level Shifter ist ZWINGEND erforderlich** (z.B. TXB0108, PCA9306) für:
  - **SCL**: 3.3V → 5V (Output WT32 → Input Touchboards)
  - **SDA**: bidirektional 3.3V ↔ 5V
  - **IRQ (Wired-OR)**: 5V → 3.3V (Output Touchboards → Input GPIO16 ESP32)
- **OHNE Level Shifter**: 5V-Signale beschädigen die 3.3V-Eingänge des ESP32!

**Spezifikationen:**
- 16 digitale Ein-/Ausgänge mit individueller Konfiguration
- I²C Interface (GPIO32/SCL, GPIO33/SDA)
- 3.3V Logik-Versorgung über I²C Bus
- Interner Pull-Up für Input-Pins verfügbar
- Interrupt-Fähig für Edge-Detection

---

### 🌡️ AC Dimmer: YYAC-3S (1x 220V PWM AC-Dimmer)
PWM-Dimmer für 220V AC-Lasten (Kronleuchter, Halogen, LED-Trafos mit Phasenanschnitt)

<img src="pictures/AC Dimmer YYAC-3S.jpg" alt="YYAC-3S AC Dimmer Modul" width="30%">

#### AC Dimmer GPIO-Belegung

**GPIO-Zuordnung:**
- `GPIO4`: PWM-Signal (0-255 → 0-100% Helligkeit)
- `VCC 5V`: Stromversorgung Modul
- `GND`: Ground
- **AC-Eingang**: 220V AC 50Hz (muss extern verdrahtet werden!)
- **AC-Ausgang**: Zur Last (Kronleuchter, etc.)

**Betriebsarten:**
- **PWM-Steuerung**: `setACDimmerBrightness(0-255)` in [src/main.cpp](src/main.cpp#L1195)
- **Relative Änderung**: ±% Helligkeit über Web-Interface
- **Min/Max Grenzen**: Keine extremen PWM-Werte wenn Strom zurückgeregelt wird

**Wichtige Hinweise:**
- ⚠️ **GEFAHR 220V!** Nur von geschultem Personal installieren
- 🔌 Netzteil-Versorgung separat (nicht vom WT32!)
- 📊 PWM-Frequenz: 5kHz (Modul-spezifisch)

**Spezifikationen:**
- AC 50Hz 220V ~ 1000W max.
- PWM Phasenanschnitt-Steuerung
- 3.3V PWM-Signal (ESP32 kompatibel)

---

### 💡 LED Dimmer: HW-517 V0.0.1 (1x MOSFET PWM-Dimmer)
MOSFET-basierter PWM-Dimmer für 12-24V DC LED-Stripes, LED-Trafos und andere DC-Lasten

<img src="pictures/HW-517 V0.0.1.jpg" alt="HW-517 V0.0.1 LED Dimmer Modul" width="30%">

#### LED Dimmer GPIO-Belegung

**GPIO-Zuordnung:**
- `GPIO15`: PWM-Signal (0-255 → 0-100% Helligkeit)
- `VCC 5V`: Stromversorgung Modul (oder 3.3V je nach Vers.)
- `GND`: Ground
- **Eingang IN+**: Zu 12-24V DC Stromversorgung (+)
- **Eingang IN-**: Zu 12-24V DC Stromversorgung (-/GND)
- **Ausgang OUT+**: Zur LED-Last (+)
- **Ausgang OUT-**: Zur LED-Last (-)

**Betriebsarten:**
- **PWM-Steuerung**: `setLEDDimmerBrightness(0-255)` in [src/main.cpp](src/main.cpp#L1173)
- **Relative Änderung**: ±% Helligkeit über Web-Interface
- **Sanfte Übergänge**: Rampen-Steuerung für angenehmes Dimmen

**Wichtige Hinweise:**
- 🔌 Separate 12/24V DC Stromversorgung erforderlich
- ⚠️ MOSFET wird warm! Ggf. Kühlkörper montieren bei >5A Last
- 📊 PWM-Frequenz: 5kHz (MOSFETs mögen höhere Frequenzen)
- Reverse-Polarity-Protection teilweise eingebaut (Datenblatt prüfen!)

**Spezifikationen:**
- DC 12-24V max. 10-30A (je nach Modul-Version)
- PWM-Dimming 0-100%
- MOSFET-Ausgang (IRF740 o.ä.)
- 3.3V PWM-Signal (ESP32 kompatibel)

---

**Komplettes Anschlussdiagramm WT32-ETH01 System:**

```
╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
║                               WT32-ETH01 (ESP32) Smart Home Controller mit Level Shifter                                       ║
║                                                    Version 1.4                                                                 ║
╚════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

                                          ┌─────────────────────────────────────┐
                                          │         WT32-ETH01 (ESP32)          │
                                          │                                     │
                                          │  GPIO32/SCL ──┬─ [4.7kΩ] ── 3.3V    │
                                          │  GPIO33/SDA ──┼─ [4.7kΩ] ── 3.3V    │
                                          │ GPIO35  ──────┼─ 1-Wire Temp (Input) │
                                          │  GPIO13 ──────┼─ MCP23017 IRQ       │◄─────┐
                                          │  GPIO16 ──────┼─ MPR121 IRQ         │◄─────┼──────┐
                                          │  GPIO15 ──────┼─ LED Dimmer HW-517 (PWM)  │
                                          │  GPIO17 ──────┼─ Status LED         │      │      │
                                          │  GND ─────────┼─ Common Ground      │      │      │
                                          │  3.3V ────────┼─ Logic Power (3.3V) │      │      │
                                          └───────┬───────┴──────────────────────┘      │      │
                                                  │                                     │      │
                                           ┌──────┴──────────┐                         │      │
                                           │  I²C: SCL 3.3V  │                         │      │
                                           │  I²C: SDA 3.3V  │                         │      │
                                           └──────────┬──────┘                         │      │
                                                      │                                │      │
                        ┌─────────────────────────────┼────────────────────────────┐   │      │
                        │                             │                            │   │      │
                        │                             │            ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓│
                        │                             │            ┃  LEVEL SHIFTER (TXB0108 oder ähnl.)    ┃│
                        │                             │            ┃  ┌────────────────────────────────┐    ┃│
                        │                             │            ┃  │  3.3V Side  ↔  5V Side         │    ┃│
                        │                             │            ┃  ├────────────────────────────────┤    ┃│
                        │                             │            ┃  │  SCL_out ──┐  ┌─── SCL_in      │    ┃│
                        │                             │            ┃  │  SDA  ◄────┼──┼─►  SDA         │    ┃│
                        │                             │            ┃  │  IRQ_in ◄──┼──┼─── IRQ_out     │    ┃│
                        │                             │            ┃  │     (5→3.3V)│  │    (Wired-OR)  │    ┃│
                        │                             │            ┃  │  GND ───────┴──┴─── GND         │    ┃│
                        │                             │            ┃  └────────────────────────────────┘    ┃│
                        │                             │            ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛│
                        │                             │                             │                       │
                        │                             │                ┌────────────────────────────┐       │
                        │                             │                │            │               │       │
                        │                             │                │        5V I²C Bus          │       │
                        │                             │                │            │               │       │
                        │                             │                │            │               │       │
                                                      │                │            │               │       │
┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓                   │    ┏━━━━━━━━━━━━━━━━┓  ┏━━━━━━━━━━━━━━━┓  ┏━━━━━━━━━━━━━━━┓│
┃        MCP23017 INPUT           ┃                   │    │ MPR121 TOUCH   ┃  │ MPR121 TOUCH  ┃  ┃ MPR121 TOUCH  ┃│
┃    (0x20) - 3.3V Logic          ┃                   │    │ Panel 1 (0x5A) ┃  │ Panel 2(0x5C) ┃  ┃ Panel 3(0x5D) ┃│
┃                                 ┃                   │    │ 5V Logic       ┃  │ 5V Logic      ┃  ┃ 5V Logic      ┃│
┃  Port A (GPA0-GPA7):            ┃                   │    │ SCL/SDA/GND    ┃  │ SCL/SDA/GND   ┃  ┃ SCL/SDA/GND   ┃│
┃  ├─ GPA0: IR-Switch Links       ┃                   │    │ IRQ (Pin X)─►──────►IRQ (Pin X)─►─────►IRQ (Pin X)─►────┘
┃  ├─ GPA1: IR-Switch Rechts      ┃                   │    │ 12x Touch Pads ┃  │ 12x Pads      ┃  ┃ 12x Pads      ┃
┃  ├─ GPA2: Kreuz EG1             ┃                   │    │                ┃  │               ┃  ┃               ┃
┃  ├─ GPA3: Kreuz EG2             ┃                   │    │ Tür Garten EG  ┃  │ Säule Garten  ┃  ┃ Säule Straße  ┃
┃  ├─ GPA4: Kreuz KG1             ┃                   │    │                ┃  │ EG            ┃  ┃ EG            ┃
┃  ├─ GPA5: Kreuz KG2             ┃                   │    └────────────────┘  └───────────────┘  └───────────────┘
┃  ├─ GPA6: Kreuz KG3             ┃                   │
┃  └─ GPA7: Reserve               ┃                   │
┃                                 ┃                   │
┃  Port B (GPB0-GPB7) - Reserve:  ┃                   │
┃  ├─ GPB0-GPB7: N/C              ┃                   │
┃                                 ┃                   │
┃  INTA/INTB ─────────────────────┼───────────────────┘
┃  (IRQ bei Änderung GPA/GPB)     ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
      │                                               │
      ├─ Kabel EG11                                   │
      ├─ Kabel EG10                                   │
      ├─ Kabel EG1                                    │
      ├─ Kabel KG1                                    │
      └─ weitere Schalter                             │

┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓  ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃      PCA9535 RELAIS (Board A)   ┃  ┃   PCA9535 RELAIS (Board B)      ┃  ┃   PCA9535 RELAIS (Board C)      ┃
┃    (0x22) - 3.3V I²C Logic      ┃  ┃ (0x23) - 3.3V I²C Logic         ┃  ┃ (0x24) - 3.3V I²C Logic         ┃
┃                                 ┃  ┃                                 ┃  ┃                                 ┃
┃  Ausgänge: R00-R07              ┃  ┃  Ausgänge: R08-R15              ┃  ┃  Ausgänge: R16-R23              ┃
┃  ├─ P0-P7: 8x Relais Ausgänge   ┃  ┃  ├─ P0-P7: 8x Relais Ausgänge   ┃  ┃  ├─ P0-P7: 8x Relais Ausgänge   ┃
┃  └─ 5V Extern (100mA)           ┃  ┃  └─ 5V Extern (100mA)           ┃  ┃  └─ 5V Extern (100mA)           ┃
┃                                 ┃  ┃                                 ┃  ┃                                 ┃
┃  → 230V Relais Verbraucher      ┃  ┃  → 230V Relais Verbraucher      ┃  ┃  → 230V Relais Verbraucher      ┃
┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛  ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                           LAN8720 PHY - RESERVIERTE GPIO PINS                                                    │
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                                                  │
│  GPIO18: ETH_MDIO       ║  GPIO19: ETH_TXD0      ║  GPIO21: ETH_CLK_OUT  ║  GPIO22: ETH_RXD0                                     │
│  GPIO23: ETH_MDC        ║  GPIO25: ETH_TX_EN     ║  GPIO26: ETH_RX_ER    ║  GPIO27: ETH_CRS_DV                                   │
│                                                                                                                                  │
│       DIESE PINS DÜRFEN NICHT FÜR ANDERE FUNKTIONEN GENUTZT WERDEN!                                                              │
│                                                                                                                                  │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

**I²C-Adressbelegung:**
```
┌─────────────┬─────────┬──────────────────────────────────────┐
│   Adresse   │  Typ    │              Beschreibung            │
├─────────────┼─────────┼──────────────────────────────────────┤
│    0x20     │ MCP23017│ I/O Expander (Schalter/Reserve)      │
│    0x22     │ PCA9535 │ Relais Board A (R00-R07)             │
│    0x23     │ PCA9535 │ Relais Board B (R08-R15)             │
│    0x24     │ PCA9535 │ Relais Board C (R16-R23)             │
│    0x5A     │ MPR121  │ TouchPanel 1 (Tür Garten EG)         │
│    0x5C     │ MPR121  │ TouchPanel 2 (Säule Garten EG)       │
│    0x5D     │ MPR121  │ TouchPanel 3 (Säule Straße EG)       │
└─────────────┴─────────┴──────────────────────────────────────┘
```

**WT32-ETH01 Pinbelegung:**
```
┌─────────┬──────────────┬────────────────────────────────────────────────────────────┐
│   Pin   │   Funktion   │               Beschreibung                                 │
├─────────┼──────────────┼────────────────────────────────────────────────────────────┤
│ GPIO04  │ PWM          │ AC Dimmer YYAC-3S 220V (GPIO4)                             │
│ GPIO13  │ IRQ Input    │ MCP23017 INTA/INTB Interrupt (Schalter/Taster)            │
│ GPIO14  │ MTDO         │ Boot Debug Output (optional verfügbar)                     │
│ GPIO15  │ PWM          │ LED Dimmer HW-517 V0.0.1 MOSFET (GPIO15)                  │
│ GPIO16  │ IRQ Input    │ MPR121 Wired-OR Interrupt (3x Touch Panels via Level Shift)│
│ GPIO17  │ Status LED   │ OnBoard LED (aktiv HIGH)                                   │
│ GPIO35  │ 1-Wire       │ DS18B20 Temperatursensor (Input-only, 4.7kΩ Pull-up zu 3V) │
│ GPIO32  │ I²C SCL      │ Clock für alle I²C Geräte + 4.7kΩ PU                       │
│ GPIO33  │ I²C SDA      │ Daten für alle I²C Geräte + 4.7kΩ PU                       │
├─────────┼──────────────┼────────────────────────────────────────────────────────────┤
│ GPIO12  │ ⚠️ RESERVE   │ ⚠️ KEIN Pull-up erlaubt! Boot fail wenn HIGH beim Start!  │                     │
│ GPIO23  │ ETH_MDC      │ LAN8720 PHY (reserviert - nicht frei!)                     │
│ GPIO25  │ ETH_TX_EN    │ LAN8720 PHY (reserviert - nicht frei!)                     │
│ GPIO26  │ ETH_RX_ER    │ LAN8720 PHY (reserviert - nicht frei!)                     │
│ GPIO27  │ ETH_CRS_DV   │ LAN8720 PHY (reserviert - nicht frei!)                     │
├─────────┼──────────────┼────────────────────────────────────────────────────────────┤
│ GPIO01  │ TX0/UART0    │ Serial Debug (Flash/Upload - zum PC)                       │
│ GPIO03  │ RX0/UART0    │ Serial Debug (Flash/Upload - vom PC)                       │
├─────────┼──────────────┼────────────────────────────────────────────────────────────┤
│ GPIO35  │ Input Only   │ ADC - kein Pull-up möglich, nur Eingang                    │
│ GPIO36  │ Input Only   │ ADC - kein Pull-up möglich, nur Eingang                    │
│ GPIO39  │ Input Only   │ ADC - kein Pull-up möglich, nur Eingang                    │
├─────────┼──────────────┼────────────────────────────────────────────────────────────┤
│ GND     │ Masse        │ Gemeinsame Masse für alle Geräte                           │
│ 3.3V    │ Logic        │ Pullup-Versorgung, kein Board-Power                        │
│ 5V      │ Power Input  │ Hauptstromversorgung (VIN)                                 │
│ ETH     │ Netzwerk     │ LAN8720 PHY → RJ45 Ethernet (8 Pins oben)                  │
└─────────┴──────────────┴────────────────────────────────────────────────────────────┘
```

**⚠️ KRITISCHE Boot-Pin-Einschränkungen:**
```
┌──────────┬────────────────────────────────────────────────────────────────────┐
│   GPIO   │                        Boot-Verhalten                              │
├──────────┼────────────────────────────────────────────────────────────────────┤
│ GPIO0    │ MUSS LOW sein beim Flash! (Boot Mode Selection)                   │
│ GPIO2    │ Darf keinen externen Pull-up beim Programmieren haben              │
│ GPIO12   │ ⚠️ BOOT FAIL wenn HIGH beim Start! KEIN Pull-up erlaubt!          │
│ GPIO14   │ Gibt Boot-Debug-Log aus (MTDO), kann stören                        │
└──────────┴────────────────────────────────────────────────────────────────────┘
```

**✅ Sichere GPIOs für Pull-up-Anwendungen (z.B. 1-Wire DS18B20):**
- **GPIO35** ✓ (aktuell verwendet für 1-Wire - Input-only)
- **GPIO13** ✓ (aktuell verwendet für MCP23017 IRQ)
- **GPIO16** ✓ (aktuell verwendet für MPR121 Wired-OR IRQ)
- **GPIO4** ✓ (aktuell PWM, aber sicher bei Bedarf)
- **GPIO14** ✓ (aktuell PWM, aber sicher bei Bedarf)

**Wichtige Hinweise:**
- **Bi-direktionaler Level Shifter ERFORDERLICH** (z.B. TXB0108, PCA9306):
  - SCL: 3.3V Output (WT32) → 5V Input (Touchboards)
  - SDA: Bidirektional 3.3V ↔ 5V
  - IRQ: 5V Output (Wired-OR der 3 Touchboards) → 3.3V Input (**GPIO16** ESP32)
  - MCP23017 INTA/INTB: Direkt zu **GPIO13** ESP32 (3.3V Logik, kein Level Shifter nötig)
- **Separate 5V Versorgung** für jedes PCA9535 Relais Board (je ~100mA)
- **5V Versorgung** für alle MPR121 Touchboards (über Level Shifter Versorgung oder separate 5V Rail)
- **4.7kΩ Pullup-Widerstände** am WT32-ETH01 Seite (SDA/SCL zu 3.3V) - auf 3.3V Seite des Level Shifters
- **5V Pullup-Widerstände** auf dem Level Shifter 5V-Seite (für Touchboard-Versorgung)
- **Gemeinsame Masse (GND)** zwischen WT32-ETH01, Level Shifter und allen Boards zwingend erforderlich
- **Kabel:** Shielded 0.25mm² für I²C Signalleitungen (SCL/SDA/IRQ)
- **TouchPanels** benötigen 5V Versorgung + I²C Anschluss

**UART Adapter Verbindung (zum Debuggen/Flashen):**
- **GPIO01 (TX0)** → CP2102/TTL Adapter RXD Pin
- **GPIO03 (RX0)** → CP2102/TTL Adapter TXD Pin
- **GND** → CP2102/TTL Adapter GND Pin
- **3.3V** → CP2102/TTL Adapter 3.3V Pin (optional, für Stromversorgung)
- ⚠️ **GPIO01/GPIO03 sind Boot-Strapping-Pins** - Vorsicht beim Anschluss!
- **Alternative:** UART2 (GPIO16/GPIO17) nicht nutzbar - GPIO17 ist Status LED

**Relais-Nummerierung:**
- Board A (0x22): R00-R07 (Pin 0-7)
- Board B (0x23): R08-R15 (Pin 0-7)
- Board C (0x24): R16-R23 (Pin 0-7)

### 👆 Touch Interface
Kapazitive Touch-Sensoren für intuitive Bedienung (mit **5V I²C und IRQ Logik**)

<img src="pictures/TouchPad 2+3.png" alt="TouchPad Säule" width="30%">

**Wichtige Hinweise zu den MPR121 Touchboards:**
- **I²C Logik: 5V Level** (kompatibel mit den Relais-Board 5V Versorgung)
- **IRQ Signal: 5V Level** (aktiv LOW bei Touch-Erkennung)
- **Alle 3 Boards werden auf EINER I²C-Adressenleitung montiert** (Daisy-Chain I²C)
- **Alle 3 IRQ-Ausgänge werden zu einer Wired-OR Leitung kombiniert** → **GPIO16** des ESP32 über Level Shifter
- ⚠️ **Level Shifter zwingend erforderlich**: 5V Signal darf nicht direkt auf 3.3V-Eingänge des ESP32 gelegt werden!

**Die 3 IRQ-Leitungen können zusammengefasst werden weil:**
- Alle IRQ-Ausgänge sind aktiv LOW (Open-Drain)
- Wired-OR: Wenn EINE der 3 Touchboards einen Touch erkennt, wird GPIO16 auf LOW gezogen
- Der Interrupt-Handler liest dann, welches Board den Interrupt ausgelöst hat, über die I²C-Adresse
- Resultat: Nur 1 physikalische IRQ-Leitung statt 3 erforderlich

**Interrupt-Architektur:**
- **GPIO13**: MCP23017 INTA/INTB → Schalter/Taster Events (3.3V Logik)
- **GPIO16**: MPR121 Wired-OR → Touch Panel Events (5V→3.3V via Level Shifter)

## 🎛️ System-Funktionen

### 🏠 Beleuchtungssteuerung
- **[Außenlampe Garten](src/main.cpp#L298)** - Gartenbeleuchtung (R04)
- **[Steinlampe](src/main.cpp#L305)** - Dekorative Beleuchtung (R05)
- **[KG Flurlampe](src/main.cpp#L312)** - Kellergeschoss Flurbeleuchtung (R06)
- **[Küchenarbeitslampe](src/main.cpp#L319)** - Arbeitsplatzbeleuchtung Küche (R07)
- **[Küchenlampe](src/main.cpp#L326)** - Hauptbeleuchtung Küche (R08)
- **[EG Flurlampe](src/main.cpp#L333)** - Erdgeschoss Flurbeleuchtung (R09)
- **[Trägerlampen](src/main.cpp#L340)** - Strukturbeleuchtung (R10)
- **[Wohnzimmerlampe 1](src/main.cpp#L347) & [2](src/main.cpp#L354)** - Wohnzimmerbeleuchtung (R11, R12)

### 💡 PWM Dimmer (LED & AC)
- **[LED Dimmer Kellertreppe](src/main.cpp#L348)** - GPIO14, MOSFET-gesteuert, 0-100% Helligkeit
- **[AC Dimmer Kronleuchter](src/main.cpp#L349)** - GPIO4, 220V AC-Dimmer, 0-100% Helligkeit (R11)
  - Kurzer Touch: EIN/AUS toggle
  - Langer Touch: Kontinuierliches Dimmen up/down

### 🎚️ Rollladensteuerung
- **[Fensterrollo hoch](src/main.cpp#L261) / [runter](src/main.cpp#L270)** - Automatische Auf/Ab-Steuerung mit Zeitbegrenzung (60s)
- **[Türrollo hoch](src/main.cpp#L279) / [runter](src/main.cpp#L288)** - Automatische Auf/Ab-Steuerung mit Zeitbegrenzung (60s)

### 🌐 Web-Interface
- **[HTTP Webserver](src/main.cpp#L174)** auf Port 80
- **[Echtzeit-Steuerung](src/main.cpp#L217)** aller Relais über Browser
- **[Status-Anzeige](src/main.cpp#L224)** aller Ein-/Ausgänge
- **[Toggle-Funktion](src/main.cpp#L241)** per Klick

### 👆 Touch-Bedienung
- **[3x MPR121 Sensoren](src/main.cpp#L62)** für kapazitive Touch-Eingabe
- **[TouchBoard 1](src/main.cpp#L398)**: Tür Garten EG - Fenster-/Türrollo, Wohnzimmer, Gartenlampe
- **[TouchBoard 2](src/main.cpp#L410)**: Säule Garten EG - Küchen-/Trägerlampen, Gruppensteuerung
- **[TouchBoard 3](src/main.cpp#L422)**: Säule Straße EG - Duplikat der Säule Garten
- **[Touch-Handler Funktion](src/main.cpp#L387)**: Zentrale Touch-Verarbeitung

### ⚡ Hardware-Interface
- **[3x PCA9535](src/main.cpp#L57)** I²C GPIO-Expander für 24 Relais-Ausgänge
- **[1x MCP23017](src/main.cpp#L52)** I²C GPIO-Expander für 16 digitale Ein-/Ausgänge
- **[I²C Bus](src/main.cpp#L47)** auf GPIO32 (SCL) und GPIO33 (SDA)
- **[AC Dimmer YYAC-3S](src/main.cpp#L1195)** - GPIO4 PWM, 220V AC bis 1000W für Kronleuchter
- **[LED Dimmer HW-517 V0.0.1](src/main.cpp#L1173)** - GPIO15 PWM, 12-24V DC bis 30A für LED-Stripes

### 🔧 Technische Features
- **[Ethernet-Kommunikation](src/main.cpp#L132)** für stabile Netzwerkverbindung
- **[Interrupt-basierte](src/main.cpp#L193)** Touch-Erkennung
- **[Zeitgesteuerte Relais](src/main.cpp#L199)** für Rollläden mit automatischer Abschaltung
- **[Gruppensteuerung](src/main.cpp#L374)** mehrerer Lampen gleichzeitig
- **[Fail-Safe Initialisierung](src/main.cpp#L155)** aller Relais im AUS-Zustand

## 🌐 Webserver-Bedienung
Das System bietet eine intuitive Web-Oberfläche zur Steuerung aller angeschlossenen Geräte:

**Zugriff:** `http://[IP-Adresse-des-WT32]/`

**Funktionen:**
- **[Einzelsteuerung](src/main.cpp#L241)** aller 24 Relais
- **[Live-Status-Anzeige](src/main.cpp#L224)** (EIN/AUS)
- **[Eingangsstatus](src/main.cpp#L232)** der 16 digitalen Eingänge
- **[Toggle-Funktion](src/main.cpp#L241)** per Klick

### 📋 Relais-Zuordnung
- **[Relaisnamen Array](src/main.cpp#L70)** - Übersicht aller 24 Relais mit Beschreibung

---

**Referenz WebGUI Design:**  
https://werner.rothschopf.net/microcontroller/202401_esp32_wt32_eth01_en.htm




## 🔧 Flashen

Manuelles Flashen
🔌 **Verkabelung**

| WT32-ETH01 | CP2102 | Beschreibung |
|------------|--------|--------------|
| 3V3        | 3.3V   | Stromversorgung |
| GND        | GND    | Masse |
| TX0        | RXD    | Daten zum PC |
| RX0        | TXD    | Daten vom PC |
| EN         | DTS    | als Reset genutzt (muss man an der Seite anlöten / siehe Bild) |
| IO0        | GND    | (nur während Flashen) Bootmodus aktivieren |

Ablauf manuell:
1. IO0 mit GND verbinden → Bootmodus aktiv

2. EN kurz auf GND tippen → Reset auslösen

3. Flash starten (z. B. mit esptool oder ESPHome-Flasher)

4. Nach dem Flashen: IO0 wieder trennen, Power Cycle für Neustart

<img src="pictures/wt32_flash.png" alt="WT32 Flash Setup" width="60%">

<img src="pictures/UART_CP2102.png" alt="CP2102 UART Adapter" width="60%">