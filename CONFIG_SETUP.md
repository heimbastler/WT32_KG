# Konfiguration - WT32-KG Smart Home Controller

## 🔐 Sensible Daten schützen

Dieses Projekt verwendet zwei Konfigurationsdateien für sensible Zugangsdaten:

### 1. `src/secure_config.h` (NICHT in Git!)

**Erstmalige Einrichtung:**

```bash
# Kopiere die Beispieldatei
cp src/secure_config.h.example src/secure_config.h

# Bearbeite die Datei mit deinen echten Zugangsdaten
nano src/secure_config.h
```

**Enthält:**
- WiFi SSID & Passwort
- Web-Interface Zugangsdaten
- MQTT Broker Credentials (optional)
- Telegram Bot Token (optional)
- OTA Update Passwort
- API Tokens

**⚠️ WICHTIG:** 
- `src/secure_config.h` ist in `.gitignore` eingetragen
- Diese Datei wird **NIEMALS** ins Git-Repository hochgeladen
- Nur `secure_config.h.example` mit Dummy-Werten committen!

### 2. `config.h.example` (Root-Verzeichnis)

Alternative/Zusätzliche Konfiguration für allgemeine Einstellungen.

```bash
# Falls verwendet:
cp config.h.example config.h
nano config.h
```

## 📋 Konfigurationsoptionen

### WiFi Einstellungen

```cpp
#define WIFI_SSID "MeinWLAN"
#define WIFI_PASSWORD "MeinPasswort"
```

**Backup WLAN:**
```cpp
#define WIFI_SSID_BACKUP "HandyHotspot"
#define WIFI_PASSWORD_BACKUP "HotspotPass"
```

**Access Point (Fallback):**
```cpp
#define AP_SSID "WT32-KG-Setup"
#define AP_PASSWORD "12345678"  // Min. 8 Zeichen
```

### Statische IP (Optional)

Standardmäßig verwendet das System DHCP. Für feste IP-Adresse:

```cpp
#define USE_STATIC_IP

#define STATIC_IP_ADDR "192.168.1.100"
#define GATEWAY_ADDR "192.168.1.1"
#define SUBNET_MASK "255.255.255.0"
```

### Web-Interface Zugang

```cpp
#define WEB_USERNAME "admin"
#define WEB_PASSWORD "MeinSicheresPasswort!"
```

**Zugriff über:** `http://192.168.1.100/` (oder deine IP)

### MQTT Integration (Home Assistant)

```cpp
#define ENABLE_MQTT  // Aktivieren

#define MQTT_SERVER "192.168.1.50"
#define MQTT_PORT 1883
#define MQTT_USER "homeassistant"
#define MQTT_PASSWORD "mqtt_passwort"
```

**Topics:**
- `home/wt32kg/status` - System Status
- `home/wt32kg/relay/[0-23]` - Relais Steuerung
- `home/wt32kg/input/[0-15]` - Digitale Eingänge
- `home/wt32kg/temperature/[0-4]` - Temperatursensoren

### OTA Updates (Over-The-Air)

```cpp
#define OTA_PASSWORD "update_passwort"
#define OTA_HOSTNAME "wt32-kg-controller"
```

**Arduino IDE OTA:**
1. Tools → Port → Network Ports → wt32-kg-controller
2. Upload mit OTA Passwort

### Telegram Benachrichtigungen

```cpp
#define ENABLE_TELEGRAM

#define TELEGRAM_BOT_TOKEN "123456:ABC..."
#define TELEGRAM_CHAT_ID "123456789"
```

**Bot erstellen:**
1. Telegram öffnen → @BotFather suchen
2. `/newbot` → Namen vergeben
3. Token kopieren
4. Bot anschreiben → `/start`
5. Chat ID holen: `https://api.telegram.org/bot<TOKEN>/getUpdates`

## 🛡️ Sicherheitsempfehlungen

### ✅ Must-Do

1. **Passwörter ändern**
   - Niemals Standard-Passwörter verwenden!
   - Mindestens 12 Zeichen, Groß-/Kleinbuchstaben, Zahlen, Sonderzeichen

2. **Netzwerk-Isolation**
   - WT32-KG nur im lokalen Netzwerk erreichbar
   - Firewall: Zugriff nur von vertrauenswürdigen IPs
   - Kein Port-Forwarding auf Internet!

3. **Backup erstellen**
   ```bash
   # Sichere deine Konfiguration
   cp src/secure_config.h ~/backups/secure_config.h.backup_$(date +%Y%m%d)
   ```

4. **Regelmäßige Updates**
   - Firmware aktuell halten
   - Sicherheits-Patches installieren

### ⚠️ Nicht vergessen

- `.gitignore` prüfen vor Git-Commit
- Keine Screenshots mit Passwörtern teilen
- Projekt-Dokumentation ohne sensible Daten
- Bei Gerät-Weitergabe: Factory Reset & neue Credentials

## 📁 Datei-Struktur

```
WT32_KG/
├── .gitignore                        # Schließt sensible Dateien aus
├── CONFIG_SETUP.md                   # Diese Datei
├── config.h.example                  # Allgemeine Config (Vorlage)
└── src/
    ├── secure_config.h.example       # Sensible Daten (Vorlage)
    ├── secure_config.h               # ⚠️ DEINE echten Daten (nicht in Git!)
    └── main.cpp                      # Nutzt secure_config.h
```

## 🔄 Update Prozess

Bei Git-Updates bleiben deine lokalen Credentials erhalten:

```bash
# Git Pull holt neue Features
git pull origin main

# Deine secure_config.h bleibt unberührt (wegen .gitignore)

# Prüfe ob neue Optionen in .example hinzugefügt wurden
diff src/secure_config.h src/secure_config.h.example

# Falls ja: manuell übernehmen
```

## 🆘 Troubleshooting

### WiFi verbindet nicht

1. Prüfe SSID & Passwort in `secure_config.h`
2. Serielle Konsole checken (115200 Baud)
3. Fallback AP sollte nach 30s starten: `WT32-KG-Setup`
4. Verbinde zu AP → `http://192.168.4.1/` → WiFi Config

### Web-Interface nicht erreichbar

1. IP-Adresse prüfen (Serial Output oder Router DHCP-Liste)
2. Ping testen: `ping 192.168.1.100`
3. Firewall/AV-Software deaktivieren (temporär)
4. Browser-Cache leeren

### OTA Update schlägt fehl

1. Passwort korrekt in Arduino IDE?
2. Genug freier Flash-Speicher? (Serial Monitor checken)
3. Netzwerkverbindung stabil?
4. Über USB-Serial flashen als Fallback

## 📞 Support

Bei Problemen:
1. Serial Monitor Output loggen (115200 Baud)
2. Fehlermeldungen dokumentieren
3. GitHub Issue erstellen (OHNE sensible Daten!)

---

**Letzte Aktualisierung:** 13. Dezember 2025
