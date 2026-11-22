# WT32_KG – Web-UI zum Übernehmen

Stand: 22.11.2026. Die Web-UI läuft auf dem ESP32 mit Arduino `WebServer` (Port 80). HTML, CSS und JavaScript werden in [src/main.cpp](src/main.cpp) erzeugt; kein separates Frontend-Build und keine zusätzliche Dateisystem-Partition sind erforderlich.

## Reiter

| Reiter | Route | Kurzbeschreibung |
| --- | --- | --- |
| Home | `/home` | Relais schalten, LED-/AC-Dimmer einstellen, zwei Rollläden öffnen/schließen/stoppen/resetten. Live-Status, MCP-Eingänge `IN0 (A0)` bis `IN15 (B7)`, Temperatur und MPR-Freigabe. |
| ESP-NOW | `/espnow` | Pairing aktivieren/deaktivieren, Clients mit Typ und Online-Status auflisten, Details anzeigen. Blaue How-to-Box erklärt Einrichtung und Einschränkungen. |
| MQTT | `/mqtt` | MQTT aktivieren, Broker/Port/Zugangsdaten speichern und Neustart auslösen. Verbindungsstatus und Hinweise zur Home-Assistant-Discovery. |
| Info | `/info` | Ethernet-/WiFi-MAC, IP, Hostname, Linkdaten, NTP-Zeit, Uptime und freien Heap anzeigen; Neustart mit Bestätigung. |
| Edit | `/edit` | Namen, HA-Typen und Freigaben für Relais, Cover, Eingänge und Sondergeräte speichern. Hinweis zur nächtlichen Kalibrierung. |

`/` leitet auf `/home` weiter. Die Reiter sind eigene Seiten, keine Single-Page-App.

## Wichtigste Features

- **Gemeinsames Dark-Mode-Layout:** Navigation, Statusfarben, Karten, Buttons und blaue Infoboxen über gemeinsamen Header/CSS.
- **Speicherschonendes Rendering:** Home und Edit streamen statische Inhalte direkt aus Flash (`PROGMEM`, `sendContent_P()`), dynamische Inhalte in kleinen Blöcken. HTTP/1.1 chunked mit genau einem Abschluss; HTTP/1.0 verbindungsbegrenzt.
- **Gebündeltes Live-Polling:** `/home_state` liefert Relais, Cover und alle Eingänge. Nur auf sichtbarer Home-Seite; keine überlappenden Abfragen, regulär 1 s nach Abschluss, 5-s-Timeout und Fehler-Backoff 2/4/8 s.
- **Gezielte DOM-Updates:** Nur sichtbare und tatsächlich geänderte Texte, Farben und Zustandsklassen werden aktualisiert.
- **Dimmer ohne Request-Flut:** Sofortige Prozentanzeige, 250-ms-Debounce, abschließender Wert bei `change`, ein laufender Request je Dimmer, neuester Wert wird nachgesendet. Timeout, einmaliger Retry und Fehlermeldung.
- **Relais-/Cover-AJAX:** Bedienung ohne vollständiges Neuladen; Cover-Aktionen OPEN/CLOSE/STOP/RESET und anschließende Statusaktualisierung.
- **Persistente Konfiguration:** Namen und Freigaben über `Preferences`; unveränderte Werte vermeiden unnötige NVS-Schreibvorgänge.
- **Kontextgerechtes Escaping:** Gemeinsame HTML-/JSON-Helfer erhalten Sonderzeichen und UTF-8; gespeicherte Rohwerte bleiben unverändert.
- **Eingabeprüfung:** Index-/Bereichsprüfung, AJAX-Dimmer 0–100 %, MQTT-Port 1–65535. Namen sind serverseitig auf 1–30 **Bytes** begrenzt.
- **Regressionstests:** Tatsächliche Handler-Ausgabe, HTTP-Framing, String-Grenzen, emittiertes JavaScript, Polling, Dimmer und Speichern werden mit Hosttests geprüft.

## Für Copy-and-paste in ein anderes Projekt

Die UI ist **kein eigenständiges Modul**: Nur die HTML-Funktionen zu kopieren reicht nicht, weil Handler auf globale Zustände und Hardware-/MQTT-Funktionen zugreifen.

1. **Darstellung übernehmen:** Aus [src/main.cpp](src/main.cpp) die `HTML_HEADER_*`-/Footer-Konstanten, `getHTMLNavigation()`, Header-/Footer-Helfer und Seitenhandler übernehmen. Aktive Navigation, CSS-Klassen und JavaScript-Funktionen zusammenhalten.
2. **Helfer übernehmen:** [include/html_utils.h](include/html_utils.h) für Escaping und [include/index_validation.h](include/index_validation.h) für strikte Integer-/Indexprüfung.
3. **API vollständig verdrahten:** Die untenstehenden Routen mit ihren Handlern registrieren. DOM-IDs, JavaScript-Aufrufer, Parameter und Antwortfelder müssen zu den Handlern passen.
4. **Backend bereitstellen:** Relais-/Eingangszustände, Cover-Struktur, Namen/Freigaben, Dimmerfunktionen, Speicherfunktionen und MQTT-Anbindung implementieren oder gezielt an das Zielprojekt anpassen. Nicht benötigte Funktionen einschließlich UI-Aufrufern entfernen.
5. **Optionale Integration:** ESP-NOW benötigt [src/espnow_gateway.h](src/espnow_gateway.h) und [src/espnow_gateway.cpp](src/espnow_gateway.cpp); Netzwerkinitialisierung siehe [src/network.cpp](src/network.cpp). Bibliotheken und Build-Umgebung stehen in [platformio.ini](platformio.ini).
6. **Projektwerte ändern:** Titel, Hostname/IP, MQTT-Themen, Geräteanzahl, GPIOs/I²C-Adressen und Cover-Relaispaare anpassen. Die lokale `src/config.h` ist nicht versioniert und muss im Zielprojekt bereitgestellt werden. Zugangsdaten nicht aus einer bestehenden Installation übernehmen oder veröffentlichen.
7. **Prüfen:** Erst bauen und Hosttests ausführen, dann Browser-/Hardwaretests mit der tatsächlichen Zielkonfiguration. Keine Ausgangszuordnung ungeprüft übernehmen.

### API-Vertrag

| Bereich | Routen / zentrale Parameter |
| --- | --- |
| Status | `/home_state`; `/inputs` bleibt als Legacy-Statusroute verfügbar |
| Relais | `/toggle?r=INDEX` |
| Cover | `/cover?idx=INDEX&action=OPEN\|CLOSE\|STOP\|RESET` |
| Dimmer | `/led?brightness=0..100&ajax=1`, `/kronleuchter?brightness=0..100&ajax=1`; ohne AJAX bleibt die Legacy-Weiterleitung erhalten |
| Speichern | `/savename?relay=INDEX`, `/savecover?cover=INDEX`, `/saveinput?input=INDEX`, `/savespecial?device=INDEX`; weitere Felder siehe jeweilige Save-Funktion |
| MQTT | `/savemqtt` mit Aktivierung, Broker, Port und Zugangsdaten |
| System | `/restart`, `/toggle_mpr121` |
| ESP-NOW | `/pairing`, `/client?id=INDEX`, `/remove_client?id=INDEX` |

`/home_state` liefert `relays[24]`, `inputs[16]` und `covers[2]` mit `position`, `openRelay`, `closeRelay`, `resetPending`. `openRelay`/`closeRelay` sind hier Zustandswerte, keine Relaisindizes. Die vollständige historische Routen-/Parameteraufnahme steht unter P5.1 in [TODO_ARCHIV.md](TODO_ARCHIV.md); spätere Änderungen sind dort unter P5.3–P5.7 dokumentiert.

### Copy-and-paste-Kurzbeschreibung

```text
ESP32-Arduino-Web-UI mit fünf Reitern: Home, ESP-NOW, MQTT, Info, Edit.
Gemeinsames Dark-Mode-CSS und blaue Infoboxen; Home/Edit streamen aus Flash.
Home: Relais, zwei Dimmer, zwei Cover, 16 MCP-Eingänge und Temperatur.
Sichtbarkeitsabhängiges Single-Flight-Polling, Timeout und Fehler-Backoff.
Dimmer: sofortige Anzeige, Debounce und serialisierte Endwertübertragung.
Edit: Namen, HA-Typen und Freigaben persistent über Preferences speichern.
HTML-/JSON-Escaping, strikte Index-/Bereichsprüfung und Host-Regressions.
Zur Übernahme UI, API, Zustände und Backend gemeinsam anpassen;
kein eigenständiges Frontend-Modul und keine zusätzliche FS-Partition.
```

## Grenzen des aktuellen Stands

- MQTT/ESP-NOW/Info bauen weiterhin vollständige Seiten als `String`; nicht alle Seiten sind gestreamt.
- Der Button „Verbindung testen“ auf MQTT zeigt derzeit nur einen Hinweis, er startet keinen eigenen Verbindungstest.
- MPR-Freigabe in der UI aktiviert keine fehlende Hardwareauswertung: Touch-Initialisierung und eigentliche Touch-Auswertung sind derzeit deaktiviert.
- ESP-NOW-Pairing endet nicht automatisch; Clients liegen nur im RAM.
- Rollladenpositionen sind Zeitabschätzungen. Verdrahtung, Fahrtrichtung und echte Endlagen müssen separat geprüft werden.
- Reale Ladezeit-/ESP32-Heap-Messungen und vollständige Hardwaretests bleiben offen (P5.8/P5.9).

## Tests

Im Projektverzeichnis:

```sh
pio run -e wt32-eth01
sh test/host/test_home_polling.sh
sh test/host/test_mcp_inputs.sh
```

Die Webtests benötigen die installierte Arduino-WebServer-Bibliothek, einen C++-Compiler und für JavaScript den im Testskript verwendeten macOS-JavaScriptCore-Runner. Bei Übernahme auf andere Systeme den Runner anpassen.
