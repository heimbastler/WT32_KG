# WT32_KG – technische TODOs

Die Punkte sollen einzeln umgesetzt und nach jedem Schritt gebaut und geprüft werden.
Für spätere Änderungen kann direkt eine ID genannt werden, zum Beispiel `P2.1`.

## Priorität 1: Speicherzugriffe absichern

- [x] **P1.1** `handleToggle()` in `src/main.cpp`: `r` auf den gültigen Bereich `0–23` prüfen, bevor `relayState[idx]` oder andere Arrays gelesen bzw. beschrieben werden. Fehlende, nichtnumerische und ungültige Werte mit HTTP 400 beantworten.
- [x] **P1.2** Arrayindizes aus HTTP- und MQTT-Eingaben einheitlich strikt parsen und validieren. Ungültige HTTP-Indizes mit 400 beantworten; ungültige MQTT-Kommandos verwerfen.
- [x] **P1.3** Regressionstest für `/toggle` mit fehlendem, negativem und zu großem `r` ergänzen.

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

## Validierung pro Änderung

- [ ] **V1** `pio run -e wt32-eth01`
- [ ] **V2** Relevante Unity-Tests mit `pio test -e test_relais` ausführen, sofern sie den geänderten Codepfad abdecken.
- [ ] **V3** Nach Upload Reset-Grund und Laufzeitverhalten über den seriellen Monitor prüfen.
