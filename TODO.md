pop# WT32_KG – technische TODOs


            │
           ┌┴┐
           │ │ 4,7 kΩ (Pull-up)
           └┬┘
            ├───────────────────► zum Schalter (10 m verdrilltes Paar)
            │
           ┌┴┐
           │ │ 1 kΩ (optionaler Schutzwiderstand)
           └┬┘
            ├───────────────────► MCP23017 Pin (GPAx / GPBx)
            │
           ═╪═ 100 nF (Keramik)
            │
           GND ◄────────────────► Rückleiter vom Schalter & MCP23017 GND

- [x] **P1.0** Entstörung die eingelötet wurde:
A0-A7 + B3-B7 jeweils:
- 1KOhm zwischen Schraubklemme + MCP2317 INx
- 4.7KOhm zwischen Schraubklemme + Vcc (3V3)
- 100nF zwischen MCP2317 INx + GND

B0-B2 jeweils:
- 1KOhm
- 4.7KOhm
- KEIN 100nF


- [ ] adde Levelshifter in Readme.md
bidirektionaler 4-Kanal-Logikpegelwandler (Logic Level Converter) auf Basis von BSS138-MOSFETs.



- [x] **P1.1** `handleToggle()` in `src/main.cpp`: `r` auf den gültigen Bereich `0–23` prüfen, bevor `relayState[idx]` oder andere Arrays gelesen bzw. beschrieben werden. Fehlende, nichtnumerische und ungültige Werte mit HTTP 400 beantworten.
- [x] **P1.2** Arrayindizes aus HTTP- und MQTT-Eingaben einheitlich strikt parsen und validieren. Ungültige HTTP-Indizes mit 400 beantworten; ungültige MQTT-Kommandos verwerfen.
- [x] **P1.3** Regressionstest für `/toggle` mit fehlendem, negativem und zu großem `r` ergänzen.
- [x] **P1.4** Rekursionsfehler der Rollladensteuerung behoben: Der STOP-Zweig aktualisiert die Position mit `updateCoverPosition(coverIndex, false)`, ohne erneut einen Endlagen-STOP auszulösen. Reguläre Positionsupdates stoppen weiterhin automatisch bei 0/100 %. Firmware-Build erfolgreich; beide Rollos mit simulierten Relaiszuständen geprüft.

## Priorität 2: Eingangs- und Protokolldaten prüfen

- [x] **P2.1** MCP23017: GPB0–GPB7 für Interrupts aktivieren und `INTA`/`INTB` für die gemeinsame IRQ-Leitung spiegeln.
- [ ] **P2.2** ESP-NOW Empfang robust validieren: Nachrichtentyp, Nutzdatenlänge und erwartete Länge pro Nachrichtentyp prüfen, bevor Werte verwendet werden.
- [ ] **P2.3** ESP-NOW Client-Name auf gültige Länge und Terminierung prüfen; unbekannte Client-Typen und Nachrichtentypen kontrolliert behandeln.
- [x] **P2.4** MQTT-Eingaben validieren: Cover-/Relaisindex strikt parsen, Prozentwerte auf 0–100 und Port auf 1–65535 begrenzen, Broker-/Benutzer-/Passwortlängen prüfen und keine rohen MQTT-Payloads loggen.
- [x] **P2.5** MCP23017-Schalterfilterung integriert (2026-11-19): EG/KG-Kreuzschaltungen übernehmen erst eine mindestens 60 ms unverändert beobachtete Schalterkombination; Öffnen und Schließen toggeln das jeweilige Flurlicht einmal. Alle 16 Eingänge werden zusätzlich zu Interrupts alle 20 ms nachgelesen, soweit die Hauptschleife dies zulässt. Während der Boot-Verzögerung und beim erstmaligen Freigeben werden Zustände und Entprellkandidaten ohne Relaisaktion synchronisiert; Startphasen-Schaltvorgänge werden nicht nachgeholt. Küchen-Tasterlogik (50 ms, LOW → HIGH) und Rohzustände für Web-UI/MQTT bleiben erhalten. Firmware-Build und 164 Host-Tests mit `sh test/host/test_mcp_inputs.sh` erfolgreich; noch kein Upload oder Hardware-Nachweis gegen Relaisflattern. Bei langen Leitungen bleibt geeignete elektrische Beschaltung erforderlich, da länger anhaltende Störungen weiterhin als Schaltvorgang gelten können.

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

- [x] **P5.1** Alle 22 Web-Routen, Parameter, Antworten, Seiteneffekte und UI-Aufrufer erfasst (siehe Bestandsaufnahme unten). Firmware und Schnittstellen unverändert; diese Schnittstellen erhalten oder bei erforderlicher Änderung mit allen Aufrufern aktualisieren.
- [x] **P5.2** `handleHome()` und `getHTMLHeader()` optimiert: gemeinsamer statischer HTML-/CSS-/JavaScript-Header als `PROGMEM`-Block mit längenbasierter Übernahme und passend reservierter Kapazität; statische Home-Abschnitte als zusammengefasste Flash-Literale (`F()`). Cover-, Relais- und Eingangsnamen auf Home mit gemeinsamem HTML-Escaping ausgegeben. Keine zusätzliche Dateisystem-Partition. Direkte Flash-Ausgabe und begrenztes Streaming sind in P5.3 umgesetzt.
- [x] **P5.3** `handleHome()` auf Streaming umgestellt: Header und statische Abschnitte direkt aus Flash per `send_P()`/`sendContent_P()`, dynamische Werte in kleinen Abschnitten und Eingänge zeilenweise. Keine vollständige Home-Antwort mehr als `String` im Heap. HTTP/1.1 verwendet Chunked Transfer mit genau einem Abschluss; HTTP/1.0 verwendet die verbindungsbegrenzte Antwort ohne falsche `Content-Length`.
- [x] **P5.4** Statusabfragen gebündelt: `/home_state` liefert zusätzlich die 16 Eingänge; `/inputs` bleibt unverändert verfügbar. Home fragt erst nach DOM-Bereitschaft und nur bei sichtbarem Tab ab, regulär 1.000 ms nach Abschluss der vorherigen Anfrage. Single-Flight mit zusammengefasster Nachforderung, 5-s-Timeout und Fehler-Backoff von 2/4/8 s. Nur sichtbare Elemente mit tatsächlich geänderten Texten, Farben oder Zustandsklassen werden aktualisiert.
- [x] **P5.5** Beide Dimmer mit 250-ms-Debounce und sofortigem lokalem Prozentwert; `change` fordert den Endwert unmittelbar an. Je Dimmer nur ein Request, neuester wartender Wert wird nachgesendet. AJAX-Antwort ohne Redirect, 5-s-Timeout, einmaliger verzögerter Retry und sichtbare Fehlermeldung. Legacy-Weiterleitungen bleiben erhalten.
- [x] **P5.6** `handleEdit()` verwendet den gemeinsamen gestreamten Flash-Header, direkte Flash-Ausgabe für statisches Markup/Save-JavaScript und kleine dynamische Abschnitte statt großer Seiten-/Fünfergruppen-Strings. Dropdown-Konstanten liegen im Flash; Optionen werden in begrenzten Blöcken gesendet. Edit-Ausgabe vor/nach diesem Schritt bytegleich geprüft.
- [x] **P5.7** Namen und freie Konfigurationstexte in Home/Edit/MQTT/ESP-NOW kontextgerecht HTML-escaped. Save-Antworten und MQTT-Discovery-Namensfelder zusätzlich korrekt JSON-escaped. Rohwerte, NVS-Formate, Längenlimits, HA-Typen und Enable-Felder bleiben erhalten; Sonderzeichen, UTF-8 und Speichern durchgängig getestet.
- [ ] **P5.8** Vor und nach der Umsetzung die Performance messbar vergleichen: Ladezeit und Antwortgröße für `/home` und `/edit` aufzeichnen; vorhandene Heap-Diagnose aus P4.2 verwenden. Ziel: geringere Antwortzeit und weniger temporäre Heap-Belegung, ohne Funktionsverlust.
- [ ] **P5.9** Nach jeder Teiländerung bauen und relevante Funktionen testen: Web-UI öffnen, Relais schalten, Cover OPEN/CLOSE/STOP/RESET bedienen, Inputs aktualisieren, Dimmer bedienen, Namen und HA-Konfiguration speichern sowie MQTT-Zustände prüfen. Bestehende Routen und NVS-Daten dürfen nicht unbeabsichtigt geändert oder gelöscht werden.

### P5.1: Bestandsaufnahme der Web-Schnittstellen

Stand: 2026-11-05, vor P5.2–P5.9. Quellen: Routenregistrierung und Handler in [src/main.cpp](src/main.cpp), Client-Detailansicht in [src/espnow_gateway.cpp](src/espnow_gateway.cpp).

#### Registrierung und Seiten

Der `WebServer` läuft auf Port 80 und wird in `loop()` über `server.handleClient()` bedient. Alle 22 Routen sind ohne Methodenbeschränkung registriert (`server.on(path, handler)`, Arduino-WebServer: `HTTP_ANY`). Die vorhandene UI verwendet ausschließlich GET, auch zum Speichern und Schalten; Parameter werden als Query-String übergeben. Eine Umstellung auf POST wäre eine gesonderte Schnittstellenänderung. Es gibt keinen projektspezifischen `onNotFound`-Handler.

| Route | Handler | Aktuelle Funktion und Antwort | Aufrufer |
| --- | --- | --- | --- |
| `/` | `handleRoot()` | HTTP 303, `Location: /home` | Direkter Einstieg; Weiterleitung der Dimmer-Endpunkte |
| `/home` | `handleHome()` | HTTP 200 HTML: MPR121-Schalter, LED-/AC-Slider, zwei Covers mit OPEN/CLOSE/STOP/RESET, Relais R04–R23, 16 Eingänge | Tab-Navigation, `/` |
| `/edit` | `handleEdit()` | HTTP 200 HTML: Namen und HA-Konfiguration für Special Devices, Covers, Relais und Eingänge; bereits Chunked Transfer (`CONTENT_LENGTH_UNKNOWN`, `sendContent()`, Abschluss mit leerem Chunk) | Tab-Navigation |
| `/mqtt` | `handleMQTT()` | HTTP 200 HTML: Verbindungsstatus, MQTT-Konfiguration und HA-Hinweise. „Verbindung testen“ zeigt nur einen Hinweis per `alert()`, kein eigener HTTP-Test | Tab-Navigation |
| `/info` | `handleInfo()` | HTTP 200 HTML: Ethernet-/WiFi-MAC-/System-/Zeit-/Heap-Daten, Neustart-Button, Repository-Link und externes QR-Code-Bild | Tab-Navigation; Meta-Refresh der Neustart-Antwort |
| `/espnow` | `handleESPNow()` | HTTP 200 HTML: Client-Liste, Pairing- und Gateway-Status, Detail-Links | Tab-Navigation, Pairing-Rückkehr, Client-Rücklink und Entfernen-Weiterleitung |
| `/client` | `handleClientDetail()` | `id`: strikt validierter Client-Index, kleiner als `MAX_ESPNOW_CLIENTS` und aktuelle Client-Anzahl. HTTP 200 HTML mit Name, MAC, Typ, Status, letztem Kontakt und typspezifischen Werten; ungültig/fehlend: HTTP 400 `text/plain` | Detail-Link aus `/espnow`; HTML-Helfer `getClientDetailHTML()` liefert Rück- und Entfernen-Link |

HTML-Antworten verwenden `text/html; charset=UTF-8`. Außer `/edit` bauen die Inhaltsseiten vollständige HTML-Strings auf. `getHTMLHeader()` liefert gemeinsame Styles, JavaScript und Tabs für `/home`, `/edit`, `/mqtt`, `/info`, `/espnow` und `/client`; `getHTMLFooter()` schließt die Seite ab.

#### Status- und Schaltendpunkte

JSON-Antworten verwenden `application/json`. Zahlenzustände sind 0/1, keine JSON-Booleans, sofern unten nicht anders angegeben.

| Route | Handler / Parameter | Antwort und Seiteneffekt | UI-Aufrufer |
| --- | --- | --- | --- |
| `/home_state` | `handleHomeState()`, keine Parameter | HTTP 200: `{"relays":[24 Zustände],"covers":[2 Objekte]}`. Jedes Cover enthält `position` (ganzzahlig), `openRelay`, `closeRelay`, `resetPending` (Boolean). Nur Status lesen | `updateHomeState()` im gemeinsamen Header; zusätzlich nach Cover-Aktion |
| `/inputs` | `handleInputs()`, keine Parameter | HTTP 200: `{"inputs":[16 Zustände]}` aus `inputState`, Reihenfolge Index 0–15 | `updateInputs()` im gemeinsamen Header |
| `/toggle` | `handleToggle()`, Pflicht: `r=0..23`, strikt geparst | HTTP 200: `{"relay":Index,"state":0/1}`; für R00–R03 zusätzlich `pair` und `pairState`. R00/R01: Fensterrollo, R02/R03: Türrollo mit Start/Stopp und gegenseitiger Verriegelung; R04–R23: Relais umschalten. MQTT-Zustände werden veröffentlicht. Fehlender/ungültiger Index: HTTP 400 JSON mit `success:false,error` | `toggleRelay(r,btnElement)` im Header; Home verwendet für Covers die separate `/cover`-API |
| `/cover` | `handleCover()`, Pflicht: `idx=0..1`, `action=OPEN/CLOSE/STOP/RESET`; Aktion wird in Großbuchstaben umgewandelt | HTTP 200: `success:true,cover,action,openRelay,closeRelay`. Steuert Cover über `setCoverState()` inklusive MQTT; RESET startet die Fahrt zur oberen Referenz. Fehlende/ungültige Parameter: HTTP 400 JSON mit `success:false,error` | `coverAction()` für beide Covers auf `/home`, danach `/home_state` |
| `/toggle_mpr121` | `handleToggleMPR121()`, keine Parameter | Touch-Auswertung umschalten, Zustand per Preferences speichern und per MQTT veröffentlichen. HTTP 200: `{"enabled":true/false}` | `toggleMPR121()` auf `/home` |
| `/led` | `handleLEDDimmer()`, optional `brightness` | Vorhandenen Wert mit `toInt()` lesen, bei 0–100 in PWM 0–255 umrechnen und LED-Dimmer setzen; fehlend/außerhalb: keine Änderung. Immer HTTP 303, `Location: /`; keine JSON-Antwort und kein MQTT-Publish im Handler | `updateLED()` bei jedem Slider-`input` auf `/home`; `fetch()` folgt Redirects standardmäßig bis `/home` |
| `/kronleuchter` | `handleACDimmer()`, optional `brightness` | Gleiche Prozent-/PWM-Verarbeitung und Weiterleitung wie `/led`, aber AC-Dimmer; R11 bleibt unabhängig. Keine strikte numerische Prüfung (auch `/led`), kein MQTT-Publish im Handler | `updateKronleuchter()` bei jedem Slider-`input` auf `/home`; Redirect-Kette bis `/home` |

#### Konfiguration speichern

Alle vier Geräte-Konfigurationsrouten verlangen Index und `name`. Indizes werden strikt geparst; `String::length()` des Namens muss 1–30 sein (Bytes, nicht zwingend Unicode-Zeichen). Fehlende/ungültige Werte ergeben HTTP 400 JSON mit `success:false,error`. `enabled` ist nur bei exakt `"1"` wahr, fehlend ist falsch. Erfolg: HTTP 200 JSON. Die Handler speichern über die oben dokumentierten Preferences-Helfer; bei aktivem und verbundenem MQTT stoßen sie Discovery erneut an. HA-Enable-Felder sind nicht mit dem Laufzeitschalter `/toggle_mpr121` gleichzusetzen.

| Route | Handler / Parameter | Erfolgsantwort | UI-Aufrufer auf `/edit` |
| --- | --- | --- | --- |
| `/savename` | `handleSaveName()`: `relay=0..23`, `name`, optional `type`, `enabled`. Typen: `none/switch/light/cover/fan`; fehlend oder unbekannt: `switch` | `success:true,relay,name,type,enabled` (Boolean) | `saveConfig()` |
| `/savecover` | `handleSaveCover()`: `cover=0..1`, `name`, optional `enabled` | `success:true,cover,name,enabled` (Boolean) | `saveCoverConfig()` |
| `/saveinput` | `handleSaveInput()`: `input=0..15`, `name`, optional `type`, `enabled`. Typen: `none/switch/door/window/motion/occupancy/opening/garage_door`; fehlend oder unbekannt: `switch` | `success:true,input,name,type,enabled` (Boolean) | `saveInputConfig()` |
| `/savespecial` | `handleSaveSpecial()`: `device=0..2` (MPR121, LED Dimmer, AC Dimmer), `name`, optional `enabled` | `success:true,device,name,deviceType,enabled` (Boolean) | `saveSpecialConfig()` |

Die Save-Funktionen trimmen Namen im Browser, lehnen leere Namen ab und URL-kodieren sie mit `encodeURIComponent()`. Die UI wertet `success` aus; Fehler werden per Alert gemeldet. Die Handler setzen Namen derzeit direkt in JSON ein; korrektes Escaping ist damit auch bei späteren Änderungen der Ausgabe zu beachten, nicht nur in HTML.

| Route | Handler / Parameter | Antwort und Seiteneffekt | Aufrufer |
| --- | --- | --- | --- |
| `/savemqtt` | `handleSaveMQTT()`: optional `enabled`, `broker`, `port`, `user`, `pass`. Fehlende Text-/Portfelder behalten den bisherigen Wert; `enabled` ist nur bei `"1"` wahr. Broker max. 63 Bytes, User/Passwort max. 31 Bytes, Port strikt 1–65535; bei aktiviertem MQTT muss Broker nicht leer sein | Ungültig: HTTP 400 JSON `success:false,error`, keine Übernahme. Erfolg: Preferences speichern, HTTP 200 `{"success":true,"restart":true}`, nach 1 s `ESP.restart()` | `saveMQTTConfig(event)` auf `/mqtt`: verhindert Formularnavigation, trimmt Broker/User/Passwort, URL-kodiert Textfelder, lädt nach Erfolg nach 3 s neu |
| `/restart` | `handleRestart()`, keine Parameter | HTTP 200 HTML mit Meta-Refresh nach 10 s zu `/info`; nach 1 s Neustart | `restartESP()` auf `/info` nach Bestätigung; `fetch()` zeigt das gelieferte HTML nicht an, UI lädt nach 10 s neu |
| `/pairing` | `handlePairing()`, keine Parameter | Pairing-Modus umschalten, HTTP 200 HTML mit Meta-Refresh nach 2 s zu `/espnow`. Die Seite nennt ein 60-s-Pairing-Fenster; der HTTP-Handler selbst setzt keinen Ablauf-Timer | Pairing-Link auf `/espnow`; auch im vorhandenen Helfer `getClientListHTML()` |
| `/remove_client` | `handleRemoveClient()`, Pflicht: `id`, gleiche Prüfung wie `/client` | Client anhand seiner MAC über `removeESPNowClient()` entfernen, HTTP 303 `Location: /espnow`; ungültig/fehlend: HTTP 400 `text/plain` | Entfernen-Link in `getClientDetailHTML()` auf `/client` |

#### Polling, Abhängigkeiten und Erhaltungsregeln

- Der gemeinsame Header startet `/inputs` alle 500 ms und `/home_state` alle 1000 ms, auch auf anderen Tabs und auf der Client-Detailseite ohne passende DOM-Elemente. `/home_state` wird zusätzlich sofort beim Ausführen des Header-Skripts im `<head>` abgefragt; `/inputs` hat keinen sofortigen Initialaufruf. Cover-Aktionen lösen weitere Home-Statusabfragen aus. Es gibt bisher keinen In-Flight-Schutz oder Backoff; Fehler landen in der Browser-Konsole.
- `/inputs` aktualisiert `input-0-status` bis `input-15-status` (OPEN/CLOSED und Farbe). `/home_state` aktualisiert `coverN-position`, `coverN-status`, `coverN-open`, `coverN-close` sowie `relay-4` bis `relay-23` (Klasse/Text). Diese IDs und die oben genannten JSON-Felder müssen bei P5.2–P5.7 zusammen mit ihren Aufrufern erhalten bleiben.
- `toggleRelay()` setzt zunächst optimistisch die Anzeige, gleicht sie mit der JSON-Antwort ab und versucht bei Fetch-/JSON-Fehlern die Anzeige zurückzusetzen. Cover-Fehler zeigen zusätzlich einen Alert. MPR121- und Dimmer-Fetches haben derzeit keine eigene Fehlerbehandlung; Save- und Statusaufrufe prüfen den HTTP-Status nicht ausdrücklich.
- Dimmer-Slider senden derzeit ungedrosselt; die 303-Kette erzeugt pro Aufruf zusätzlich einen Abruf von `/` und `/home`. Das ist für P5.5 und spätere Performance-Messungen relevant.
- Geräte-Namen, HA-Typen und Enable-Felder, NVS-Namespaces/-Schlüssel, MQTT-Discovery und Schaltlogik bleiben bei den Web-Optimierungen unverändert. Änderungen an Route, Parametern, Response-Feldern, Redirects oder DOM-IDs erfordern die gleichzeitige Anpassung aller oben genannten Aufrufer.

#### Vorhandene Tests und Prüfgrenzen

- [test/test_main.cpp](test/test_main.cpp) enthält Unity-Tests für Indexvalidierung (`test_toggle_index_validation`) und Hardware-Relais-/I2C-Tests. Der Indextest prüft den Parser, nicht HTTP-Status, JSON oder die Produktionshandler.
- [test/test_wifi_relais.cpp](test/test_wifi_relais.cpp) ist eine separate WiFi-Testfirmware mit eigenen Routen `/` und `/toggle?relay=...` sowie `text/plain`-Antwort. Sie ist kein Integrationstest der Produktionsroute `/toggle?r=...`.
- [web_preview.html](web_preview.html) ist eine statische Vorschau, kein Test der produktiven Handler. [src/original_backup.cpp](src/original_backup.cpp) ist laut [platformio.ini](platformio.ini) vom Build ausgeschlossen.
- Es gibt keine automatisierten HTTP-/Browser-Regressionstests für die hier erfassten Produktionsrouten. Diese Bestandsaufnahme ist statisch aus dem Quellcode erstellt; Live-Antworten, Ladezeiten, Heap-Wirkung und Hardware-/MQTT-Verhalten sind damit nicht gemessen. P5.8/P5.9 und V1–V3 bleiben offen. P5.1 ändert ausschließlich Dokumentation.

### P5.2: Umsetzung und Prüfung

- [src/main.cpp](src/main.cpp): Der statische Header wird nicht mehr aus vielen einzelnen Anhängeoperationen zusammengesetzt, sondern einmal aus einem Flash-Block kopiert. Die aktive Tab-Markierung bleibt dynamisch. Zusammenhängende statische Home-Markup-/Scriptabschnitte werden ebenfalls jeweils als ein Flash-Block angehängt. Die vollständige Antwort bleibt bis P5.3 ein `String`; eine gemessene Verbesserung der temporären Heap-Belegung oder Ladezeit wird hier noch nicht behauptet.
- [include/html_utils.h](include/html_utils.h): Gemeinsamer Escaping-Helfer für `&`, `<`, `>`, doppelte und einfache Anführungszeichen, mit vorab berechneter Ausgabelänge. Home verwendet ihn für beide Cover-Namen, alle dargestellten Relais-Namen und alle Eingangsnamen. UTF-8 bleibt erhalten. Escaping auf weiteren Seiten bleibt P5.7.
- Firmware-Build `pio run -e wt32-eth01` erfolgreich. Vorher/nachher: Flash 1.070.425 → 1.067.937 Bytes (2.488 Bytes weniger), statischer RAM unverändert 55.016 Bytes. Vorhandene OneWire-Warnungen bleiben unverändert.
- Host-Vergleich der tatsächlichen alten/neuen Header-/Home-Funktionen mit einem `String`-/Server-Testdouble: Header für sechs Tab-Werte und Footer bytegleich; Home für 32 Zustandskombinationen bytegleich. Bei Namen mit HTML-Sonderzeichen entspricht die Ausgabe exakt dem bisherigen Markup mit escapten Namen. Routenregistrierung sowie Code außerhalb Header/Home (einschließlich MQTT, NVS und Schaltendpunkten) unverändert.
- Sechs Escaping-Fälle aus dem neuen Unity-Test `test_html_escaping()` in [test/test_main.cpp](test/test_main.cpp) im Host-Test erfolgreich ausgeführt: leerer Text, normaler Name, alle fünf Sonderzeichen, Script-Markup, vorhandene Entity und UTF-8. Zusätzlich maximale Expansion eines 30-Byte-Namens geprüft.
- Ein zusätzlicher Host-Lauf mit ASan/UBSan blieb ohne Ergebnis hängen und wurde abgebrochen; der reguläre Host-Lauf wurde danach erneut erfolgreich ausgeführt. Daraus wird keine Sanitizer-Freigabe abgeleitet.
- `pio test -e test_relais --without-uploading --without-testing` kompiliert die Tests, scheitert jedoch beim Linken: Die vorhandenen Testdateien [test/test_main.cpp](test/test_main.cpp) und [test/test_wifi_relais.cpp](test/test_wifi_relais.cpp) definieren beide `setup()`/`loop()`. Dieser bestehende Testaufbau wurde nicht verändert. Kein Upload und keine Hardware-Schaltversuche durchgeführt; Live-UI-/MQTT-Prüfung sowie Heap-/Ladezeitmessung bleiben offen.

### P5.3: Home-Streaming und Transferprüfung

- [src/main.cpp](src/main.cpp): Der Flash-Header, sein Abschluss und der Footer sind gemeinsam genutzte Konstanten. `getHTMLHeader()` bleibt für die übrigen Seiten verfügbar und verwendet dieselbe Tab-Navigation wie die gestreamte Startseite. Home sendet statische Daten direkt aus Flash; nur Navigation, dynamische Markup-Abschnitte, einzelne Relaisbuttons, Eingangszeilen und der kleine Temperaturabschnitt entstehen noch als temporäre Strings. HTML, DOM-IDs, Escaping und Bedienfunktionen bleiben unverändert.
- Der Handler setzt `CONTENT_LENGTH_UNKNOWN`, startet die Antwort mit der Drei-Argument-Variante von `send_P()` und beendet sie ausdrücklich mit `sendContent("")`. Die installierte WebServer-Bibliothek erzeugt für HTTP/1.1 Chunk-Header und genau einen Null-Chunk; ihr anschließendes `_finalizeResponse()` schreibt keinen zweiten Abschluss. Bei HTTP/1.0 entfällt Chunked Transfer; `Connection: close` begrenzt die Antwort.
- Reproduzierbarer Host-Test ohne Upload: `sh test/host/test_home_streaming.sh`. Der Test extrahiert die tatsächlichen Home-/Header-Funktionen und die HTTP-Formatierungsfunktionen der lokal installierten Arduino-WebServer-Bibliothek. Nur Transport und Arduino-`String` werden auf dem Host nachgebildet. Voraussetzung: C++11-Compiler und das lokal installierte PlatformIO-ESP32-Arduino-Framework.
- 72 Antworten geprüft: je 36 Fälle für HTTP/1.0 und HTTP/1.1 mit wechselnden Relais-/Eingangs-/Cover-/Dimmer-/Touch-Zuständen, Temperatur einschließlich Fehlerfall sowie 30-Byte-Namen aus HTML-Sonderzeichen. HTTP-Header, Chunk-Längen, Nutzdaten, Footer und genau ein Transferabschluss geprüft. Der Test begrenzt erzeugte String-Längen und angeforderte Reservierungen jeweils auf maximal 1.024 Bytes; beobachtet wurden 391 Bytes Länge und 512 Bytes Reservierung. Das sind Host-Prüfwerte, keine Messung des realen ESP32-Heap-Peaks.
- Zusätzlich mit einer vor P5.3 gesicherten Quellkopie ausgeführt: `sh test/host/test_home_streaming.sh /pfad/zur/main-vor-p5-3.cpp`. Alle 72 dekodierten Antworten sowie Header für sechs Tab-Werte und Footer waren bytegleich zum P5.2-Stand. Routen, Konfigurationsspeicherung und Schaltlogik wurden nicht geändert.
- Firmware-Build `pio run -e wt32-eth01` erfolgreich: Flash 1.068.469 Bytes, statischer RAM unverändert 55.016 Bytes. Kein Upload und keine Tests an realen Lasten; Browser-/MQTT-Funktionsprüfung und reale Ladezeit-/Heap-Messung bleiben P5.8/P5.9 vorbehalten.

### P5.4: Gebündeltes und begrenztes Status-Polling

- [src/main.cpp](src/main.cpp): `/home_state` behält `relays` und `covers` unverändert bei und ergänzt `inputs` mit den 16 numerischen 0/1-Zuständen in derselben Reihenfolge wie `/inputs`. Der JSON-String reserviert 512 Bytes. Die bestehende `/inputs`-Route bleibt für ältere Clients unverändert; die aktuelle Web-UI fragt sie nicht mehr separat ab.
- Der gemeinsame Header startet keine Polling-Anfragen mehr auf Edit/MQTT/Info/ESP-NOW/Client-Seiten. Auf Home beginnt die erste Anfrage nach `DOMContentLoaded`, nicht mehr vor dem Aufbau der Bedienelemente. Im ausgeblendeten Browser-Tab entfallen neue Anfragen; ein noch laufender Request darf abschließen, verändert dann aber keine DOM-Werte und startet keinen Timer. Beim Sichtbarwerden erfolgt eine frische Statusabfrage.
- Statt zwei `setInterval()`-Timern gibt es eine selbstplanende `setTimeout()`-Schleife: normalerweise mindestens 1.000 ms Abstand ab Abschluss einer Anfrage, zuzüglich Antwortzeit. Damit sinkt das reguläre Statusaufkommen von ungefähr drei auf höchstens eine Anfrage pro Sekunde. Eingänge werden bewusst nicht mehr alle 500 ms, sondern zusammen mit Relais und Covers aktualisiert. Aktionsanfragen bleiben unabhängig bedienbar.
- Es läuft höchstens eine Statusanfrage einschließlich JSON-Verarbeitung. Mehrere Aktualisierungswünsche währenddessen werden zu genau einer anschließenden frischen Anfrage zusammengefasst, insbesondere nach Cover-Aktionen. Diese Nachforderung umgeht bei Fehlern nicht den Backoff.
- `fetch()` verwendet `cache: 'no-store'` und einen `AbortController` mit 5.000-ms-Timeout. HTTP-Fehler, ungültiges JSON, ungültige Datenformen und Netzwerk-/Timeoutfehler werden in der Browser-Konsole protokolliert. Wiederholungen warten 2, dann 4, dann maximal 8 Sekunden; nach Erfolg gilt wieder der normale Abstand. Die UI setzt moderne Fetch-/Promise-/AbortController-Unterstützung voraus.
- Text, Eingangsfarbe und Relaisklassen werden nur geschrieben, wenn sie sich vom tatsächlichen DOM unterscheiden. Versteckte Elemente ohne Layout-Rechteck werden ausgelassen und beim nächsten sichtbaren Statusabgleich nachgeführt. Es gibt keinen Zustands-Cache, der die Korrektur optimistischer oder veralteter Anzeigen verhindern könnte. Cover-Status und OPEN/CLOSE/STOP/RESET-Bedienung bleiben erhalten.
- Reproduzierbarer Test auf macOS: `sh test/host/test_home_polling.sh`. Der bestehende C++-Host-Test erzeugt dabei Fixtures aus dem tatsächlich kompilierten Header-JavaScript und den tatsächlichen Handlern `/home_state` und `/inputs`; JavaScriptCore führt den ausgegebenen Code mit kontrolliertem DOM, Fetch und Zeitgebern aus. Geprüft: API-Felder/-Werte und Legacy-Eingänge, DOM-Bereitschaft, 1.000-ms-Abstand, Single-Flight/Nachforderung, unveränderte/versteckte DOM-Werte, Tab-Sichtbarkeit, HTTP-/JSON-/Schema-/Netzwerkfehler, 2/4/8-s-Backoff, 5-s-Abbruch, Wiederanlauf sowie Refresh nach Cover-Aktion. Alle Prüfungen erfolgreich.
- Die 72 Home-Streaming-/HTTP-Transferfälle aus P5.3 bestehen weiterhin. Firmware-Build `pio run -e wt32-eth01` erfolgreich: Flash 1.070.261 Bytes, statischer RAM unverändert 55.016 Bytes. Kein Upload und keine Live-Prüfung mit ESP32/Browser/MQTT; reale Zeit-/Heap-Messungen und Hardware-Funktionstest bleiben P5.8/P5.9.

### P5.5–P5.7: Dimmer, Edit-Streaming und Escaping

#### P5.5: Dimmer-Anfragen begrenzen

- [src/main.cpp](src/main.cpp): Ein gemeinsamer `createDimmerSender()` bedient LED- und AC-Dimmer unabhängig voneinander. `input` aktualisiert die eigene Prozentanzeige sofort und bündelt Änderungen über 250 ms. `change` fordert den Endwert ohne zusätzliche Wartezeit an; während eines laufenden Requests bleibt nur der neueste Wert vorgemerkt. Ein alter Response überschreibt die lokale Anzeige nicht. Auch der Endwert 0 wird gesendet.
- Die UI ruft `/led?brightness=N&ajax=1` beziehungsweise `/kronleuchter?brightness=N&ajax=1` auf. Dieser additive Modus validiert den Prozentwert strikt auf 0–100 und antwortet mit HTTP 200 `{"success":true}` oder HTTP 400 JSON. Dadurch entfallen die bisherigen Folgeabrufe über `/` nach `/home`. Ohne `ajax=1` bleiben Verarbeitung und HTTP-303-Weiterleitung der Legacy-Endpunkte unverändert; PWM-Umrechnung und Unabhängigkeit von R11 bleiben erhalten.
- Requests werden pro Gerät serialisiert, nach 5 Sekunden abgebrochen und bei einem Fehler einmal nach 1 Sekunde wiederholt, sofern kein neuerer Wert wartet. Fehler erscheinen in der Konsole; nach ausgeschöpftem Retry zeigt die UI einen Alert. Dieselbe Eingabe kann danach erneut angefordert werden. Eine erfolgreiche Bestätigung verhindert unnötiges erneutes Senden desselben alleinigen `change`-Werts.

#### P5.6: Edit-Seite streamen

- Home und Edit verwenden `sendHTMLHeader()` mit denselben Flash-Konstanten und derselben Tab-Navigation. Edit baut weder den vollständigen Header noch große Karten-/Fünfergruppen-/JavaScript-Strings im Heap auf. Statische Abschnitte werden direkt per `sendContent_P()` ausgegeben; dynamische Felder und einzelne Dropdown-Listen bleiben klein. Der Footer und der einmalige Transferabschluss werden explizit gesendet.
- Die Typ-/Beschriftungslisten für Relais und Eingänge sind konstante Flash-Daten statt pro Eintrag neu erzeugter String-Arrays. Dropdown-Optionen werden mit 512 beziehungsweise 768 Bytes reservierter Kapazität aufgebaut.
- Vor dem anschließenden Escaping-Schritt gegen eine gesicherte P5.5-Quellkopie verglichen: alle 16 Edit-Antworten (HTTP/1.0 und HTTP/1.1, acht HA-Konfigurationen) sowie die 72 Home-Fälle bytegleich. Alle HA-Typen, Enable-Zustände, Save-Funktionen und ursprünglichen Leername-Prüfungen blieben erhalten. Für die Edit-Normalfälle wurden im Host-Test maximal 360 Bytes String-Länge und 768 Bytes Reservierung beobachtet; dies ist keine reale ESP32-Heap-Messung.

#### P5.7: Textwerte korrekt ausgeben

- Der vorhandene Helfer in [include/html_utils.h](include/html_utils.h) escaped alle Relais-/Cover-/Eingangs-/Special-Device-Namen in Edit, die schon geschützten Home-Namen, Broker/User/Passwort im MQTT-Formular sowie ESP-NOW-Namen in der Gateway-Liste und beiden HTML-Helfern in [src/espnow_gateway.cpp](src/espnow_gateway.cpp). Hostname und angezeigte MQTT-Topics werden ebenfalls als Text escaped. Gespeicherte Namen werden nicht durch HTML-Entities ersetzt.
- `escapeJSONString()` behandelt Anführungszeichen, Backslash und sämtliche Steuerbytes U+0000–U+001F; UTF-8 bleibt unverändert. Alle vier Save-Antworten geben damit gültiges JSON mit dem ursprünglichen Namen zurück. Die eng gekoppelten neun dynamischen Namensfelder der MQTT-Discovery verwenden denselben Helfer, damit gültige Namen mit Anführungszeichen nicht die Discovery-Nachricht beschädigen. Topics, Discovery-Schema, NVS-Namensräume/-Schlüssel und Speichergrenzen wurden nicht umgestellt.
- Alle 41 Edit-Namensfelder behalten `maxlength=30`. Die Servergrenze bleibt 1–30 Bytes laut `String::length()`, nicht 30 beliebige Unicode-Zeichen. Host-Prüfungen akzeptieren 30 Bytes, lehnen 31 Bytes ab und bestätigen unveränderte gespeicherte Rohwerte bei ungültigen Folgeeingaben. Bereits eingegebene Entity-Texte werden einmal escaped, nicht als Markup interpretiert.

#### Validierung

- `pio run -e wt32-eth01` nach jedem Schritt erfolgreich: P5.5 Flash 1.072.325 Bytes, P5.6 1.069.409 Bytes, P5.7 1.071.017 Bytes; statischer RAM jeweils 55.016 Bytes.
- `sh test/host/test_home_polling.sh` erfolgreich: 72 Home-Transferfälle, 16 Edit-Konfigurationen, gebündeltes Polling, Dimmer-Debounce/Serialisierung/Endwerte/Retry/Timeout, Legacy- und AJAX-Dimmerantworten, alle 41 Edit-Namensfelder und ihre Limits, MQTT-Formularattribute, drei ESP-NOW-Ansichten sowie HTML-/JSON-/UTF-8- und Steuerbyte-Roundtrips. Auch die tatsächlich ausgegebenen vier Edit-Save-Funktionen wurden mit den tatsächlichen Handler-JSON-Antworten ausgeführt: URL-kodierte Rohwerte, Erfolgsmeldungen und Leername-Prüfungen funktionieren.
- Die Host-Streamingtests begrenzen String-Längen und angeforderte Reservierungen für Home und Edit weiterhin auf jeweils höchstens 1.024 Bytes, auch mit maximal expandierenden Namen. Netzwerktransport, DOM, Zeitgeber und Speicherung sind Testdoubles; echte Flash-/NVS-/MQTT-Interaktion und Browserdarstellung am Gerät sind damit nicht geprüft.
- Die ergänzten Unity-Fälle in [test/test_main.cpp](test/test_main.cpp) kompilieren. `pio test -e test_relais --without-uploading --without-testing` scheitert weiterhin erst am bestehenden Linkkonflikt der doppelten `setup()`-/`loop()`-Definitionen. Der Host-Compiler meldet zusätzlich einen bereits vorhandenen, nicht vollständigen ESP-NOW-Clienttyp-Switch als Warnung; diese fachfremde Logik wurde nicht geändert.
- Kein Upload und keine Tests an realen Lasten durchgeführt. P5.8/P5.9 bleiben für reale Performance-, Browser-, NVS- und MQTT-Prüfungen offen.

### Umsetzungs-Prompt für eine KI

> Optimiere die Weboberfläche des ESP32-Projekts `heimbastler/WT32_KG` gemäß P5.1–P5.9 in `TODO.md`. Untersuche zuerst `src/main.cpp`, identifiziere die tatsächlichen Web-Routen, HTML-Handler, Polling-Aufrufe und vorhandenen Tests. Ändere die Implementierung in kleinen, nachvollziehbaren Schritten. Reduziere große dynamische `String`-Konkatenationen und unnötige HTTP-Abfragen; verwende für statische HTML/CSS/JavaScript-Anteile nach Möglichkeit `PROGMEM` und die passenden `WebServer`-Sendefunktionen, ohne eine nicht vorhandene Dateisystem-Partition einzuführen. Stream dynamische Inhalte in begrenzten Blöcken und escapen gespeicherte Texte für HTML. Vermeide parallele Polling-Requests und drossele Dimmer-Slider-Anfragen. Bewahre alle existierenden Routen, UI-Funktionen, Konfigurationsspeicherung, MQTT-Verhalten und Relais-/Cover-Logik. Ändere keine Hardware-Zuordnungen und keine gespeicherten NVS-Schlüssel oder -Formate. Ergänze oder aktualisiere Tests, führe den Firmware-Build und passende Tests aus und berichte abschließend konkrete Änderungen, Messergebnisse sowie nicht verifizierte Punkte. Markiere in `TODO.md` nur die Arbeitspunkte als erledigt, die tatsächlich umgesetzt und geprüft wurden.

## Validierung pro Änderung

- [ ] **V1** `pio run -e wt32-eth01`
- [ ] **V2** Relevante Unity-Tests mit `pio test -e test_relais` ausführen, sofern sie den geänderten Codepfad abdecken.
- [ ] **V3** Nach Upload Reset-Grund und Laufzeitverhalten über den seriellen Monitor prüfen.
