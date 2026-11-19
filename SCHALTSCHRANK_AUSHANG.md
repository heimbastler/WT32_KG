# WT32-KG | Schaltschrank-Aushang

**Steuerung:** WT32-ETH01 · **Netzwerk:** Ethernet · **Stand:** 19.11.2026

> **Wichtiger Codeabgleich:** Relaisliste und Quellcode unterscheiden sich bei den Rollladenrichtungen: Laut Liste R00/R02 = runter, R01/R03 = hoch; im Code OPEN = R00/R02, CLOSE = R01/R03. Reale Fahrtrichtung und aufgespielte Firmware sind nicht geprüft. R12 ist laut Liste „Kronleuchter L2“ ohne Lampenanschluss; im Code standardmäßig „Reserve Wohnzimmer“, HA-Typ „light“, aktiviert. Gespeicherte Namen und HA-Einstellungen können abweichen. Zuordnung vor Bedienung fachgerecht prüfen lassen.

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

## Relaiszuordnung laut RELAIS.md

| Relais | Kabel | Dokumentierte Funktion |
| --- | --- | --- |
| R00 | `<047>` | Fensterjalousie runter |
| R01 | `<044>` | Fensterjalousie hoch |
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
| R13–R15 | — | Frei / nicht belegt |
| R16–R23 | — | In `RELAIS.md` nicht aufgeführt |

## Dokumentations- und Firmwareabgleich

| Relais | Liste laut `RELAIS.md` | Zuordnung im Quellcode |
| --- | --- | --- |
| R00 / R01 | R00 runter, R01 hoch | Cover 0: OPEN = R00, CLOSE = R01 |
| R02 / R03 | R02 runter, R03 hoch | Cover 1: OPEN = R02, CLOSE = R03 |
| R07 | Arbeitsleuchten | Küchenarbeitslampe (Standardname; andere Benennung) |
| R12 | Kronleuchter L2, ohne Lampenanschluss | Standard: Reserve Wohnzimmer; HA-Typ „light“, aktiviert |

Namen und HA-Einstellungen sind veränderbare Firmware-Standardwerte und können durch gespeicherte Werte ersetzt sein. Eine Namensänderung ändert nicht die feste Relaiszuordnung der Cover.

**Quelle der dokumentierten Zuordnung:** [RELAIS.md](RELAIS.md). **Quellcodeabgleich:** [src/main.cpp](src/main.cpp), insbesondere `coverRelayPairs`, `relayNames`, `relayHAType` und `relayHAEnabled`. Die Relais werden in Gruppen zu je acht über PCA9535 angesprochen: Board A `0x22` = R00–R07, Board B `0x23` = R08–R15, Board C `0x24` = R16–R23.
