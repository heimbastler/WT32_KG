# WT32_KG – technische TODOs

Die Punkte sollen einzeln umgesetzt und nach jedem Schritt gebaut und geprüft werden.
Für spätere Änderungen kann direkt eine ID genannt werden, zum Beispiel `P2.1`.

## Priorität 1: Speicherzugriffe absichern

- [x] **P1.1** `handleToggle()` in `src/main.cpp`: `r` auf den gültigen Bereich `0–23` prüfen, bevor `relayState[idx]` oder andere Arrays gelesen bzw. beschrieben werden. Fehlende, nichtnumerische und ungültige Werte mit HTTP 400 beantworten.
- [x] **P1.2** Arrayindizes aus HTTP- und MQTT-Eingaben einheitlich strikt parsen und validieren. Ungültige HTTP-Indizes mit 400 beantworten; ungültige MQTT-Kommandos verwerfen.
- [x] **P1.3** Regressionstest für `/toggle` mit fehlendem, negativem und zu großem `r` ergänzen.
- [x] **P1.4** Rekursionsfehler der Rollladensteuerung behoben: Der STOP-Zweig aktualisiert die Position mit `updateCoverPosition(coverIndex, false)`, ohne erneut einen Endlagen-STOP auszulösen. Reguläre Positionsupdates stoppen weiterhin automatisch bei 0/100 %. Firmware-Build erfolgreich; beide Rollos mit simulierten Relaiszuständen geprüft.

## Priorität 2: Eingangs- und Protokolldaten prüfen

- [x] **P2.1** MCP23017: GPB0–GPB7 für Interrupts aktivieren und `INTA`/`INTB` für die gemeinsame IRQ-Leitung spiegeln.
- [ ] **P2.2** ESP-NOW Empfang robust validieren: Nachrichtentyp, Nutzdatenlänge und erwartete Länge pro Nachrichtentyp prüfen, bevor Werte verwendet werden.
- [ ] **P2.3** ESP-NOW Client-Name auf gültige Länge und Terminierung prüfen; unbekannte Client-Typen und Nachrichtentypen kontrolliert behandeln.
- [x] **P2.4** MQTT-Eingaben validieren: Cover-/Relaisindex strikt parsen, Prozentwerte auf 0–100 und Port auf 1–65535 begrenzen, Broker-/Benutzer-/Passwortlängen prüfen und keine rohen MQTT-Payloads loggen.

## Priorität 3: Blockierende Laufzeit verkürzen

- [x] **P3.1** Nächtliche Rollladen-Kalibrierung auf `millis()`-Zustandsphasen umstellen; keine langen `delay()`-Aufrufe mehr in diesem Ablauf.
- [x] **P3.2** DS18B20-Messung nicht blockierend durchführen. DallasTemperature-Konversion mit `setWaitForConversion(false)` starten und den Messwert nach der passenden Konversionszeit abholen.
- [x] **P3.3** `delay(50)` in der IR-Tasterverarbeitung durch zeitbasierte Entprellung ersetzen.
- [x] **P3.4** MQTT Discovery und initiale State-Veröffentlichung kooperativ und zeitlich gestaffelt abarbeiten, statt während der gesamten Veröffentlichung zu warten.
- [x] **P3.5** Netzwerk-Setup auf maximal 10 Sekunden begrenzt; Fehler bei fehlendem Link oder fehlender IP werden getrennt protokolliert.

## Priorität 4: Reset- und Speicherdiagnose

- [x] **P4.1** Beim Start den Reset-Grund (`esp_reset_reason()`) samt lesbarem Namen und Zahlenwert protokollieren, damit Watchdog-, Brownout- und Software-Resets unterscheidbar sind.
- [x] **P4.2** Freien Heap und größten freien Heap-Block einmal pro Minute protokollieren und Minimalwerte seit Boot mitführen, um Fragmentierung zu erkennen.
- [x] **P4.3** Große HTML-Antworten geprüft und für Header/Home vorab String-Kapazität reserviert; Heap-Wirkung nach Upload mit P4.2-Messwerten kontrollieren.
- [x] **P4.4** Alle `Preferences`-Schreibstellen dokumentiert; unveränderte Werte lösen keinen NVS-Write aus. Es gibt keine Konfigurationsschreibvorgänge in der Loop.
- [ ] **P4.5** Brownout-/Spannungsversorgung bei realen Neustarts prüfen, insbesondere beim Schalten von Relais und Lasten.

### Preferences-Schreibstellen

| Funktion in `src/main.cpp` | Namespace | Gespeicherte Werte |
| --- | --- | --- |
| `saveRelayName()` | `relay-names` | `R0`–`R23` |
| `saveRelayHAConfig()` | `relay-ha-config` | `type_N`, `en_N` |
| `saveCoverConfig()` | `cover-config` | `cname_N`, `cen_N` |
| `saveInputConfig()` | `input-config` | `iname_N`, `itype_N`, `ien_N` |
| `saveSpecialDeviceConfig()` | `special-dev` | MPR121-, LED- und AC-Dimmername/Enable |
| `saveMPR121State()` | `mpr121-config` | `enabled` |
| `saveMQTTConfig()` | `mqtt-config` | `enabled`, `broker`, `port`, `user`, `password` |

## Priorität 5: Web-UI schneller und speicherschonender machen

Die Weboberfläche funktioniert derzeit, aber `src/main.cpp` baut große HTML-Antworten mit vielen `String`-Konkatenationen. Ziel ist, die Ladezeit und Heap-Last zu verringern, ohne Bedienfunktionen, URL-Routen, MQTT-Discovery, Konfigurationsspeicherung oder Relaisverhalten zu verändern.

- [ ] **P5.1** Vor Änderungen alle Web-Routen und deren aktuelle Funktion erfassen: insbesondere `/home`, `/edit`, `/mqtt`, `/info`, `/espnow`, `/home_state`, `/inputs` sowie die Konfigurations- und Schaltendpunkte. Diese Schnittstellen erhalten oder bei erforderlicher Änderung mit allen Aufrufern aktualisieren.
- [ ] **P5.2** `handleHome()` und `getHTMLHeader()` in `src/main.cpp` optimieren. Statische HTML-, CSS- und JavaScript-Anteile nicht bei jeder Anfrage über große `String`-Konkatenationen neu aufbauen. Bevorzugt statische Inhalte als `PROGMEM`-Daten mit `server.send_P()`/`server.sendContent_P()` ausgeben; dynamische Werte gezielt und korrekt HTML-escapen. Keine zusätzliche Dateisystem-Partition voraussetzen, sofern sie nicht bereits konfiguriert ist.
- [ ] **P5.3** `handleHome()` schrittweise streamen: Header, statische Abschnitte und dynamische Relais-/Cover-/Eingangsdaten in überschaubaren Blöcken senden, statt eine komplette, sehr große `String`-Antwort gleichzeitig im Heap zu halten. `Content-Length` beziehungsweise Chunked Transfer konsistent und korrekt abschließen.
- [ ] **P5.4** In der Startseite doppelte beziehungsweise unnötig häufige Statusabfragen reduzieren. Aktuell gibt es getrennte periodische Abfragen für `/inputs` und `/home_state`; die UI soll Statusdaten möglichst gebündelt abfragen und nur sichtbare beziehungsweise tatsächlich geänderte DOM-Werte aktualisieren. Polling-Intervall angemessen wählen, keine überlappenden `fetch()`-Aufrufe zulassen und Fehler ohne ungebremste Wiederholungen behandeln.
- [ ] **P5.5** Slider-Handler für LED- und AC-Dimmer drosseln: nicht für jedes `input`-Ereignis eine Netzwerk-Anfrage auslösen. Einen geeigneten Throttle/Debounce oder ein `change`-Ereignis verwenden, dabei weiterhin unmittelbares lokales Feedback geben und den Endwert zuverlässig an den ESP senden.
- [ ] **P5.6** `handleEdit()` als eigenen Optimierungsschritt überprüfen. Die Konfigurationsseite enthält viele dynamische Felder und Chunk-Ausgaben. Statische Markup- und JavaScript-Bestandteile auslagern beziehungsweise als Flash-Konstanten senden; dynamische Listen in begrenzten Blöcken streamen. Speichern-Buttons und alle vorhandenen Validierungen müssen weiter funktionieren.
- [ ] **P5.7** Große dynamische Benutzereingaben beim Einfügen in HTML escapen, insbesondere gespeicherte Relais-, Cover-, Eingangs- und Gerätenamen. Eingabegrenzen und bisherige Speicherformate beibehalten; gültige Namen sollen korrekt angezeigt werden.
- [ ] **P5.8** Vor und nach der Umsetzung die Performance messbar vergleichen: Ladezeit und Antwortgröße für `/home` und `/edit` aufzeichnen; vorhandene Heap-Diagnose aus P4.2 verwenden. Ziel: geringere Antwortzeit und weniger temporäre Heap-Belegung, ohne Funktionsverlust.
- [ ] **P5.9** Nach jeder Teiländerung bauen und relevante Funktionen testen: Web-UI öffnen, Relais schalten, Cover OPEN/CLOSE/STOP/RESET bedienen, Inputs aktualisieren, Dimmer bedienen, Namen und HA-Konfiguration speichern sowie MQTT-Zustände prüfen. Bestehende Routen und NVS-Daten dürfen nicht unbeabsichtigt geändert oder gelöscht werden.

### Umsetzungs-Prompt für eine KI

> Optimiere die Weboberfläche des ESP32-Projekts `heimbastler/WT32_KG` gemäß P5.1–P5.9 in `TODO.md`. Untersuche zuerst `src/main.cpp`, identifiziere die tatsächlichen Web-Routen, HTML-Handler, Polling-Aufrufe und vorhandenen Tests. Ändere die Implementierung in kleinen, nachvollziehbaren Schritten. Reduziere große dynamische `String`-Konkatenationen und unnötige HTTP-Abfragen; verwende für statische HTML/CSS/JavaScript-Anteile nach Möglichkeit `PROGMEM` und die passenden `WebServer`-Sendefunktionen, ohne eine nicht vorhandene Dateisystem-Partition einzuführen. Stream dynamische Inhalte in begrenzten Blöcken und escapen gespeicherte Texte für HTML. Vermeide parallele Polling-Requests und drossele Dimmer-Slider-Anfragen. Bewahre alle existierenden Routen, UI-Funktionen, Konfigurationsspeicherung, MQTT-Verhalten und Relais-/Cover-Logik. Ändere keine Hardware-Zuordnungen und keine gespeicherten NVS-Schlüssel oder -Formate. Ergänze oder aktualisiere Tests, führe den Firmware-Build und passende Tests aus und berichte abschließend konkrete Änderungen, Messergebnisse sowie nicht verifizierte Punkte. Markiere in `TODO.md` nur die Arbeitspunkte als erledigt, die tatsächlich umgesetzt und geprüft wurden.

## Validierung pro Änderung

- [ ] **V1** `pio run -e wt32-eth01`
- [ ] **V2** Relevante Unity-Tests mit `pio test -e test_relais` ausführen, sofern sie den geänderten Codepfad abdecken.
- [ ] **V3** Nach Upload Reset-Grund und Laufzeitverhalten über den seriellen Monitor prüfen.
