# WT32_KG – offene Aufgaben

Stand: 22.11.2026. Hier stehen nur offene Arbeiten und Prüfungen. Abgeschlossene Schritte, frühere Messergebnisse und die historische Web-API-Bestandsaufnahme stehen in [TODO_ARCHIV.md](TODO_ARCHIV.md). Aktuelle Bedienungs-/Übernahmehinweise: [WEB_UI.md](WEB_UI.md); Hardware: [README.md](README.md).

## Firmware

- [ ] **P2.2** ESP-NOW-Empfang vollständig validieren: Nachrichtentyp, zulässige Nutzdatenlänge und erwartete Länge je Nachrichtentyp prüfen, bevor Daten verwendet werden. Fehlerfälle mit Hosttests abdecken.
- [ ] **P2.3** ESP-NOW-Namen sicher initialisieren und terminieren; gültige Länge und Client-Typen prüfen. Kurze Namen, fehlende Terminierung und unbekannte Typen kontrolliert behandeln und testen.
- [ ] **P2.6** MPR121-Touch-Auswertung bei gewünschter Inbetriebnahme aktivieren: Hardwareinitialisierung und Fehlerbehandlung sowie funktionierende Ereignisauswertung ergänzen. Direkte IRQ-Leitung ist an GPIO36 verdrahtet; derzeit sind Initialisierung/Interruptregistrierung auskommentiert und `handleTouchEvents()` ohne aktive Auswertung. GPIO36 hat keinen internen Pull-up. Anschließend alle drei Panels und ihre Relaiszuordnung testen.

## Hardwareangaben klären

- [ ] **P2.7** Küchen-Eingänge GPA0/GPA1: tatsächlichen Anschluss und gewünschten Einsatz klären; vorhandene Tasterlogik allein bestätigt keine angeschlossene Hardware.
- [ ] **P2.8** Level-Shifter: Transistortyp, Belegung von Kanal 2 und genaue Zuordnung SCL/SDA auf Kanal 3/4 klären. Bestätigt sind 5 V ↔ 3,3 V und Kanal 1 für MPR-IRQ.
- [ ] **P4.5** Bei realen Neustarts Versorgung/Brownouts unter Last prüfen und Reset-Grund protokollieren, besonders beim Schalten von Relais und Verbrauchern. Die bestätigte 5-V-Versorgung aus dem Hutschienen-Netzteil ersetzt diese Laufzeitprüfung nicht.

## Messungen und Funktionsprüfung

- [ ] **P5.8** Reale Antwortgröße, Ladezeit und ESP32-Heap für `/home` und `/edit` messen; freien Heap, größten freien Block und Minimalwerte aus der vorhandenen Diagnose nutzen. Vorher-/Nachher-Vergleich nur mit vergleichbaren Firmwareständen und Bedingungen. Host-String-Grenzen sind kein realer Heap-Nachweis.
- [ ] **P5.9 / V3** Nach Upload vollständige Browser-/Hardware-/MQTT-Prüfung: Relais, Cover OPEN/CLOSE/STOP/RESET, Eingänge, Dimmer-Endwerte, Namens-/HA-Speicherung einschließlich Neustart und MQTT-Zustände. Reset-Grund und Laufzeitverhalten seriell beobachten. Reale Cover-Fahrtrichtungen gegen dokumentierte Verdrahtung prüfen; gespeicherte NVS-Daten erhalten. Die erfolgreiche Entstörungsprüfung allein deckt diese Funktionen nicht ab.
- [ ] **V2** Unity-Testaufbau reparieren: doppelte `setup()`-/`loop()`-Definitionen der beiden Testprogramme verhindern das Linken. Testprogramme sauber trennen und anschließend relevante Unity-Tests ausführen; Hosttests bestehen bereits.

## Bei jeder Firmwareänderung

Firmware bauen und passende Hosttests ausführen; keine Hardwareprüfung allein aus einem erfolgreichen Build ableiten:

```sh
pio run -e wt32-eth01
sh test/host/test_home_polling.sh
sh test/host/test_mcp_inputs.sh
```

Der bisherige Build-Punkt V1 ist erledigt, bleibt aber eine wiederkehrende Prüfung. Level-Shifter mit Foto und die eingelötete Eingangsbeschaltung sind dokumentiert; die Entstörungswirkung wurde laut Betreiber erfolgreich geprüft.
