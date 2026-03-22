// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    ESP-NOW Gateway für WT32-ETH01                           ║
// ║                   Bridge zwischen ESP-NOW und Ethernet                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#ifndef ESPNOW_GATEWAY_H
#define ESPNOW_GATEWAY_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// ===== Client-Datenstruktur =====
#define MAX_ESPNOW_CLIENTS 10
#define CLIENT_NAME_LENGTH 32

enum ClientType {
  CLIENT_TYPE_UNKNOWN = 0,
  CLIENT_TYPE_SENSOR = 1,      // Sensor-Node (Temperatur, Luftfeuchte, etc.)
  CLIENT_TYPE_SWITCH = 2,      // Schalter/Button-Node
  CLIENT_TYPE_RELAY = 3,       // Relais-Steuerung
  CLIENT_TYPE_DIMMER = 4,      // LED-Dimmer
  CLIENT_TYPE_CUSTOM = 5       // Custom Device
};

struct ESPNowClient {
  uint8_t mac[6];              // MAC-Adresse des Clients
  char name[CLIENT_NAME_LENGTH]; // Name des Clients (z.B. "Wohnzimmer Sensor")
  ClientType type;             // Typ des Clients
  bool paired;                 // Ist Client gepairt?
  unsigned long lastSeen;      // Letzter Kontakt (millis)
  bool online;                 // Ist Client online? (< 60s seit lastSeen)
  
  // Client-spezifische Daten (flexible Verwendung)
  float sensorValue1;          // z.B. Temperatur
  float sensorValue2;          // z.B. Luftfeuchte
  bool switchState;            // z.B. Button-Status
  uint8_t dimmerLevel;         // z.B. Helligkeit 0-255
  char customData[64];         // Freie Daten als String
};

// ===== ESP-NOW Nachrichten-Struktur =====
#define ESPNOW_MSG_MAX_LEN 250

enum MessageType {
  MSG_PAIR_REQUEST = 1,        // Client möchte sich pairen
  MSG_PAIR_RESPONSE = 2,       // Gateway bestätigt Pairing
  MSG_DATA_UPDATE = 3,         // Client sendet Daten-Update
  MSG_COMMAND = 4,             // Gateway sendet Kommando an Client
  MSG_PING = 5,                // Keep-Alive Ping
  MSG_PONG = 6                 // Keep-Alive Antwort
};

struct ESPNowMessage {
  uint8_t type;                // MessageType
  uint8_t clientId;            // Client Identifier
  uint8_t dataLen;             // Länge der Nutzdaten
  uint8_t data[ESPNOW_MSG_MAX_LEN]; // Nutzdaten
};

// ===== Globale Variablen =====
extern ESPNowClient espnowClients[MAX_ESPNOW_CLIENTS];
extern int espnowClientCount;
extern bool espnowPairingMode;  // Pairing-Modus aktiv?

// ===== Funktionsprototypen =====
void initESPNowGateway();
void espnowReceiveCallback(const uint8_t *mac, const uint8_t *data, int len);
void espnowSendCallback(const uint8_t *mac, esp_now_send_status_t status);
bool addESPNowClient(const uint8_t *mac, const char *name, ClientType type);
bool removeESPNowClient(const uint8_t *mac);
int findClientByMAC(const uint8_t *mac);
void updateClientData(int clientIndex, const ESPNowMessage *msg);
bool sendCommandToClient(int clientIndex, const uint8_t *data, uint8_t dataLen);
void checkClientTimeouts();
void enablePairingMode(bool enable);
String getClientListHTML();
String getClientDetailHTML(int clientIndex);

// ===== Hilfsfunktionen =====
String macToString(const uint8_t *mac);
void stringToMAC(const String &macStr, uint8_t *mac);

#endif // ESPNOW_GATEWAY_H
