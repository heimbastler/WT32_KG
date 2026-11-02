# WT32-KG | Schaltschrank-Aushang

**Steuerung:** WT32-ETH01 · **Netzwerk:** Ethernet · **Stand:** 01.11.2026

> **Wichtiger Codeabgleich:** Die vor Ort abgelesene Relaisliste stimmt bei den Rollläden nicht mit der aktuellen Firmware überein. R00/R01 und R02/R03 sind in der Firmware jeweils in umgekehrter Fahrtrichtung benannt und angesteuert. R12 ist laut Schaltschrankliste Kronleuchter L2 (nicht angeschlossen), im Code aber „Reserve Wohnzimmer“ und als Home-Assistant-Light aktiviert. Vor Bedienung dieser Ausgänge Firmware und reale Verdrahtung abgleichen.

## Weboberfläche

- **IP:** `http://192.168.178.195/`
- **mDNS:** `http://wt32-kg.local/` (sofern mDNS im lokalen Netz aufgelöst wird)
- **Repository / Dokumentation:** [github.com/heimbastler/WT32_KG](https://github.com/heimbastler/WT32_KG)

![QR-Code zum WT32-KG-Repository](https://api.qrserver.com/v1/create-qr-code/?size=320x320&data=https%3A%2F%2Fgithub.com%2Fheimbastler%2FWT32_KG)

## MQTT / Home Assistant

| Einstellung | Wert |
| --- | --- |
| MQTT | In der Weboberfläche unter **MQTT** aktivieren |
| Broker | `192.168.178.1` (Firmware-Standard; gespeicherten Wert prüfen) |
| Port | `1883` (Firmware-Standard) |
| Benutzer | ______________________________ |
| Passwort | ______________________________ |
| Transport | Ethernet / TCP |
| Base Topic | `wt32kg` |
| Home Assistant Discovery Prefix | `homeassistant` |

Broker, Port und Zugangsdaten können über die MQTT-Seite geändert werden und werden im ESP32 gespeichert. Das Passwort ist absichtlich nicht auf diesem Blatt hinterlegt.

## Relaiszuordnung laut Schaltschrank

| Relais | Kabel | Physische Funktion laut `RELAIS.md` |
| --- | --- | --- |
| R00 | `<044>` | Fensterjalousie runter |
| R01 | `<047>` | Fensterjalousie hoch |
| R02 | `<045>` | Türjalousie runter |
| R03 | `<046>` | Türjalousie hoch |
| R04 | `<043>` | Außenlampe Garten |
| R05 | `<019>` | Steinlampen / Stein |
| R06 | `<120>` | KG-Flur |
| R07 | `<025>` | Arbeitsleuchten |
| R08 | `<011>` | Küche |
| R09 | `<010>` | EG-Flur |
| R10 | `<009>` | Trägerlampen / Tisch |
| R11 | `<033>` | Kronleuchter |
| R12 | `<034>` | Kronleuchter L2, nicht an Lampe angeschlossen |
| R13–R15 | — | In `RELAIS.md` nicht zugeordnet |
| R16–R23 | — | In `RELAIS.md` nicht aufgeführt |

## Firmwareabweichungen

| Relais | Schaltschrankliste | Aktueller Code in `src/main.cpp` |
| --- | --- | --- |
| R00 / R01 | R00 runter, R01 hoch | R00 hoch, R01 runter; Cover 0 nutzt R00 als „open“ |
| R02 / R03 | R02 runter, R03 hoch | R02 hoch, R03 runter; Cover 1 nutzt R02 als „open“ |
| R07 | Arbeitsleuchten | Küchenarbeitslampe |
| R12 | Kronleuchter L2, nicht angeschlossen | Reserve Wohnzimmer; HA-Light standardmäßig aktiviert |

**Quelle der physischen Zuordnung:** [RELAIS.md](RELAIS.md), zuletzt vor Ort abgelesen. **Firmwareabgleich:** [src/main.cpp](src/main.cpp). Die Relais werden in Gruppen zu je acht über PCA9535 angesprochen: Board A `0x22` = R00–R07, Board B `0x23` = R08–R15, Board C `0x24` = R16–R23.
