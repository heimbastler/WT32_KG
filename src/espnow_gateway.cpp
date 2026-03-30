// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    ESP-NOW Gateway Implementation                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "espnow_gateway.h"

// ===== Globale Variablen =====
ESPNowClient espnowClients[MAX_ESPNOW_CLIENTS];
int espnowClientCount = 0;
bool espnowPairingMode = false;

// ===== Initialisierung =====
void initESPNowGateway() {
  Serial.println("\n=== ESP-NOW Gateway Initialisierung ===");
  
  // ⚠️ WICHTIG: WiFi nur für ESP-NOW, KEIN Station/AP Mode für Internet!
  // WiFi.mode(WIFI_STA) aktiviert WiFi-Radio für ESP-NOW
  // WiFi.disconnect() verhindert Verbindung zu Access Points
  // → Ethernet bleibt primäres Netzwerk-Interface für OTA/Webserver!
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.print("Gateway MAC: ");
  Serial.println(WiFi.macAddress());
  
  // ESP-NOW initialisieren
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW Init fehlgeschlagen!");
    return;
  }
  
  Serial.println("✅ ESP-NOW initialisiert");
  
  // Callback-Funktionen registrieren
  esp_now_register_recv_cb(espnowReceiveCallback);
  esp_now_register_send_cb(espnowSendCallback);
  
  // Client-Array initialisieren
  for (int i = 0; i < MAX_ESPNOW_CLIENTS; i++) {
    espnowClients[i].paired = false;
    espnowClients[i].online = false;
    espnowClients[i].type = CLIENT_TYPE_UNKNOWN;
    memset(espnowClients[i].name, 0, CLIENT_NAME_LENGTH);
  }
  
  Serial.println("✅ ESP-NOW Gateway bereit");
  Serial.println("==========================================\n");
}

// ===== Empfangs-Callback =====
void espnowReceiveCallback(const uint8_t *mac, const uint8_t *data, int len) {
  if (len < sizeof(ESPNowMessage)) {
    Serial.println("⚠️ ESP-NOW: Ungültige Nachrichtenlänge");
    return;
  }
  
  ESPNowMessage *msg = (ESPNowMessage *)data;
  int clientIdx = findClientByMAC(mac);
  
  Serial.printf("📨 ESP-NOW RX von %s - Type: %d\n", macToString(mac).c_str(), msg->type);
  
  switch (msg->type) {
    case MSG_PAIR_REQUEST:
      if (espnowPairingMode) {
        Serial.println("🔗 Pairing Request empfangen");
        // Client-Name aus Daten extrahieren
        char clientName[CLIENT_NAME_LENGTH];
        memcpy(clientName, msg->data, min(msg->dataLen, (uint8_t)(CLIENT_NAME_LENGTH - 1)));
        clientName[CLIENT_NAME_LENGTH - 1] = '\0';
        
        // Client hinzufügen
        if (addESPNowClient(mac, clientName, (ClientType)msg->clientId)) {
          // Pairing-Bestätigung senden
          ESPNowMessage response;
          response.type = MSG_PAIR_RESPONSE;
          response.clientId = 1; // Success
          response.dataLen = 0;
          
          esp_now_peer_info_t peerInfo;
          memset(&peerInfo, 0, sizeof(peerInfo));
          memcpy(peerInfo.peer_addr, mac, 6);
          peerInfo.channel = 0;
          peerInfo.encrypt = false;
          
          esp_now_add_peer(&peerInfo);
          esp_now_send(mac, (uint8_t *)&response, sizeof(response));
          
          Serial.println("✅ Client gepairt!");
        }
      } else {
        Serial.println("⚠️ Pairing-Modus nicht aktiv");
      }
      break;
      
    case MSG_DATA_UPDATE:
      if (clientIdx >= 0) {
        updateClientData(clientIdx, msg);
        espnowClients[clientIdx].lastSeen = millis();
        espnowClients[clientIdx].online = true;
      }
      break;
      
    case MSG_PING:
      if (clientIdx >= 0) {
        espnowClients[clientIdx].lastSeen = millis();
        espnowClients[clientIdx].online = true;
        
        // Pong zurücksenden
        ESPNowMessage pong;
        pong.type = MSG_PONG;
        pong.clientId = clientIdx;
        pong.dataLen = 0;
        esp_now_send(mac, (uint8_t *)&pong, sizeof(pong));
      }
      break;
      
    default:
      Serial.printf("⚠️ Unbekannter Nachrichtentyp: %d\n", msg->type);
      break;
  }
}

// ===== Sende-Callback =====
void espnowSendCallback(const uint8_t *mac, esp_now_send_status_t status) {
  Serial.printf("📤 ESP-NOW TX zu %s: %s\n", 
                macToString(mac).c_str(), 
                status == ESP_NOW_SEND_SUCCESS ? "OK" : "FEHLER");
}

// ===== Client hinzufügen =====
bool addESPNowClient(const uint8_t *mac, const char *name, ClientType type) {
  if (espnowClientCount >= MAX_ESPNOW_CLIENTS) {
    Serial.println("❌ Maximale Anzahl Clients erreicht!");
    return false;
  }
  
  // Prüfen ob Client bereits existiert
  if (findClientByMAC(mac) >= 0) {
    Serial.println("⚠️ Client bereits gepairt");
    return false;
  }
  
  // Neuen Client hinzufügen
  ESPNowClient *client = &espnowClients[espnowClientCount];
  memcpy(client->mac, mac, 6);
  strncpy(client->name, name, CLIENT_NAME_LENGTH - 1);
  client->name[CLIENT_NAME_LENGTH - 1] = '\0';
  client->type = type;
  client->paired = true;
  client->online = true;
  client->lastSeen = millis();
  client->sensorValue1 = 0.0;
  client->sensorValue2 = 0.0;
  client->switchState = false;
  client->dimmerLevel = 0;
  memset(client->customData, 0, sizeof(client->customData));
  
  espnowClientCount++;
  
  Serial.printf("✅ Client hinzugefügt: %s (%s)\n", name, macToString(mac).c_str());
  return true;
}

// ===== Client entfernen =====
bool removeESPNowClient(const uint8_t *mac) {
  int idx = findClientByMAC(mac);
  if (idx < 0) return false;
  
  // ESP-NOW Peer entfernen
  esp_now_del_peer(mac);
  
  // Client aus Array entfernen (aufrücken)
  for (int i = idx; i < espnowClientCount - 1; i++) {
    espnowClients[i] = espnowClients[i + 1];
  }
  espnowClientCount--;
  
  Serial.printf("🗑️ Client entfernt: %s\n", macToString(mac).c_str());
  return true;
}

// ===== Client suchen =====
int findClientByMAC(const uint8_t *mac) {
  for (int i = 0; i < espnowClientCount; i++) {
    if (memcmp(espnowClients[i].mac, mac, 6) == 0) {
      return i;
    }
  }
  return -1;
}

// ===== Client-Daten aktualisieren =====
void updateClientData(int clientIndex, const ESPNowMessage *msg) {
  if (clientIndex < 0 || clientIndex >= espnowClientCount) return;
  
  ESPNowClient *client = &espnowClients[clientIndex];
  
  // Daten je nach Client-Typ interpretieren
  switch (client->type) {
    case CLIENT_TYPE_SENSOR:
      if (msg->dataLen >= 8) {
        memcpy(&client->sensorValue1, msg->data, 4);
        memcpy(&client->sensorValue2, msg->data + 4, 4);
        Serial.printf("📊 Sensor-Daten: %.2f, %.2f\n", client->sensorValue1, client->sensorValue2);
      }
      break;
      
    case CLIENT_TYPE_SWITCH:
      if (msg->dataLen >= 1) {
        client->switchState = msg->data[0] > 0;
        Serial.printf("🔘 Switch: %s\n", client->switchState ? "ON" : "OFF");
      }
      break;
      
    case CLIENT_TYPE_DIMMER:
      if (msg->dataLen >= 1) {
        client->dimmerLevel = msg->data[0];
        Serial.printf("💡 Dimmer: %d%%\n", (client->dimmerLevel * 100) / 255);
      }
      break;
      
    default:
      // Custom Data als String speichern
      if (msg->dataLen > 0) {
        memcpy(client->customData, msg->data, min(msg->dataLen, (uint8_t)63));
        client->customData[63] = '\0';
      }
      break;
  }
}

// ===== Kommando an Client senden =====
bool sendCommandToClient(int clientIndex, const uint8_t *data, uint8_t dataLen) {
  if (clientIndex < 0 || clientIndex >= espnowClientCount) return false;
  if (!espnowClients[clientIndex].online) return false;
  
  ESPNowMessage msg;
  msg.type = MSG_COMMAND;
  msg.clientId = clientIndex;
  msg.dataLen = min(dataLen, (uint8_t)ESPNOW_MSG_MAX_LEN);
  memcpy(msg.data, data, msg.dataLen);
  
  esp_err_t result = esp_now_send(espnowClients[clientIndex].mac, (uint8_t *)&msg, sizeof(msg));
  return (result == ESP_OK);
}

// ===== Client-Timeouts prüfen =====
void checkClientTimeouts() {
  unsigned long now = millis();
  for (int i = 0; i < espnowClientCount; i++) {
    if (espnowClients[i].paired) {
      // Client als offline markieren wenn > 60 Sekunden keine Nachricht
      if (now - espnowClients[i].lastSeen > 60000) {
        if (espnowClients[i].online) {
          espnowClients[i].online = false;
          Serial.printf("⚠️ Client offline: %s\n", espnowClients[i].name);
        }
      }
    }
  }
}

// ===== Pairing-Modus =====
void enablePairingMode(bool enable) {
  espnowPairingMode = enable;
  Serial.printf("🔗 Pairing-Modus: %s\n", enable ? "AKTIV" : "INAKTIV");
}

// ===== HTML-Generierung für Client-Liste =====
String getClientListHTML() {
  String html = "<h3>📡 ESP-NOW Clients</h3>";
  html += "<div style='margin:10px 0;'>";
  html += "<a href='/pairing' class='btn btn-neutral' style='min-width:150px;'>🔗 Pairing-Modus</a>";
  html += "</div>";
  
  if (espnowClientCount == 0) {
    html += "<p>Keine Clients gepairt. Aktivieren Sie den Pairing-Modus und starten Sie einen Client.</p>";
  } else {
    html += "<table><tr><th>Name</th><th>Typ</th><th>Status</th><th>Aktion</th></tr>";
    for (int i = 0; i < espnowClientCount; i++) {
      ESPNowClient *c = &espnowClients[i];
      html += "<tr><td>" + String(c->name) + "</td>";
      html += "<td>";
      switch (c->type) {
        case CLIENT_TYPE_SENSOR: html += "Sensor"; break;
        case CLIENT_TYPE_SWITCH: html += "Schalter"; break;
        case CLIENT_TYPE_RELAY: html += "Relais"; break;
        case CLIENT_TYPE_DIMMER: html += "Dimmer"; break;
        default: html += "Custom"; break;
      }
      html += "</td><td>";
      html += c->online ? "🟢 Online" : "🔴 Offline";
      html += "</td><td>";
      html += "<a href='/client?id=" + String(i) + "' class='btn btn-neutral' style='min-width:80px;'>Details</a>";
      html += "</td></tr>";
    }
    html += "</table>";
  }
  
  return html;
}

// ===== HTML-Generierung für Client-Details =====
String getClientDetailHTML(int clientIndex) {
  if (clientIndex < 0 || clientIndex >= espnowClientCount) {
    return "<p>❌ Client nicht gefunden</p>";
  }
  
  ESPNowClient *c = &espnowClients[clientIndex];
  String html = "<h3>📱 Client: " + String(c->name) + "</h3>";
  html += "<div class='card'>";
  html += "<p><b>MAC:</b> " + macToString(c->mac) + "</p>";
  html += "<p><b>Typ:</b> ";
  switch (c->type) {
    case CLIENT_TYPE_SENSOR: html += "Sensor"; break;
    case CLIENT_TYPE_SWITCH: html += "Schalter"; break;
    case CLIENT_TYPE_RELAY: html += "Relais"; break;
    case CLIENT_TYPE_DIMMER: html += "Dimmer"; break;
    default: html += "Custom"; break;
  }
  html += "</p>";
  html += "<p><b>Status:</b> " + String(c->online ? "🟢 Online" : "🔴 Offline") + "</p>";
  html += "<p><b>Letzter Kontakt:</b> " + String((millis() - c->lastSeen) / 1000) + "s</p>";
  
  // Typ-spezifische Daten
  switch (c->type) {
    case CLIENT_TYPE_SENSOR:
      html += "<p><b>Wert 1:</b> " + String(c->sensorValue1, 2) + "</p>";
      html += "<p><b>Wert 2:</b> " + String(c->sensorValue2, 2) + "</p>";
      break;
    case CLIENT_TYPE_SWITCH:
      html += "<p><b>Status:</b> " + String(c->switchState ? "EIN" : "AUS") + "</p>";
      break;
    case CLIENT_TYPE_DIMMER:
      html += "<p><b>Helligkeit:</b> " + String((c->dimmerLevel * 100) / 255) + "%</p>";
      break;
  }
  
  html += "</div>";
  html += "<div style='margin-top:15px;'>";
  html += "<a href='/espnow' class='btn btn-neutral'>◀ Zurück</a>";
  html += " <a href='/remove_client?id=" + String(clientIndex) + "' class='btn btn-off'>🗑️ Entfernen</a>";
  html += "</div>";
  
  return html;
}

// ===== Hilfsfunktionen =====
String macToString(const uint8_t *mac) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

void stringToMAC(const String &macStr, uint8_t *mac) {
  sscanf(macStr.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
         &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
}
