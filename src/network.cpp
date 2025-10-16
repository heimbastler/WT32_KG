#include <WiFi.h>
#include <WebServer.h>
#include <ETH.h>
#include "config.h"

// Externe Referenzen
extern WebServer server;

// WiFi Status
bool wifiConnected = false;
bool apModeActive = false;
unsigned long apStartTime = 0;

// Funktionsprototypen
void initNetworking();
void handleWiFiEvents();
void startAccessPoint();
void stopAccessPoint();
String getNetworkStatus();
void handleWifiConfig();
void handleWifiScan();
void handleWifiConnect();

// ======================================================
// Network Initialisierung
// ======================================================
void initNetworking() {
  Serial.println("=== Network Setup ===");
  
  // Hostname setzen
  WiFi.setHostname(HOSTNAME);
  
  // Ethernet initialisieren (bevorzugt)
  ETH.begin();
  delay(2000);
  
  // Prüfen ob Ethernet verbunden ist
  if (ETH.linkUp()) {
    Serial.println("Ethernet Kabel erkannt");
    
    #ifdef ETH_USE_STATIC_IP
    ETH.config(ETH_STATIC_IP, ETH_GATEWAY, ETH_SUBNET);
    Serial.println("Ethernet: Statische IP konfiguriert");
    #endif
    
    return; // Ethernet funktioniert, kein WiFi nötig
  }
  
  Serial.println("Kein Ethernet - starte WiFi");
  
  // WiFi versuchen
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Verbinde zu WiFi: ");
  Serial.println(WIFI_SSID);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_CONNECT_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    
    #ifdef USE_STATIC_IP
    WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS1, DNS2);
    #endif
    
    Serial.println("WiFi verbunden!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi Verbindung fehlgeschlagen - starte Access Point");
    startAccessPoint();
  }
}

// ======================================================
// Access Point starten
// ======================================================
void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  apModeActive = true;
  apStartTime = millis();
  
  IPAddress apIP = WiFi.softAPIP();
  Serial.println("=== ACCESS POINT AKTIV ===");
  Serial.println("SSID: " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASSWORD));
  Serial.print("IP: ");
  Serial.println(apIP);
  Serial.println("Gehe zu: http://" + apIP.toString());
  
  // Zusätzliche Routen für WiFi-Konfiguration
  server.on("/wifi", handleWifiConfig);
  server.on("/wifi/scan", handleWifiScan);
  server.on("/wifi/connect", HTTP_POST, handleWifiConnect);
}

// ======================================================
// Access Point stoppen
// ======================================================
void stopAccessPoint() {
  if (apModeActive) {
    WiFi.softAPdisconnect(true);
    apModeActive = false;
    Serial.println("Access Point gestoppt");
  }
}

// ======================================================
// WiFi Events überwachen
// ======================================================
void handleWiFiEvents() {
  // AP-Timeout prüfen
  if (apModeActive && millis() - apStartTime > AP_TIMEOUT) {
    Serial.println("AP-Timeout erreicht - neustart");
    ESP.restart();
  }
  
  // WiFi Reconnect falls nötig
  if (wifiConnected && WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Verbindung verloren - versuche Reconnect");
    wifiConnected = false;
    WiFi.reconnect();
  }
}

// ======================================================
// Network Status abfragen
// ======================================================
String getNetworkStatus() {
  String status = "<h3>Netzwerk Status</h3>";
  
  if (ETH.linkUp()) {
    status += "<p><strong>Ethernet:</strong> Verbunden<br>";
    status += "IP: " + ETH.localIP().toString() + "<br>";
    status += "MAC: " + ETH.macAddress() + "</p>";
  } else {
    status += "<p><strong>Ethernet:</strong> Nicht verbunden</p>";
  }
  
  if (wifiConnected) {
    status += "<p><strong>WiFi:</strong> Verbunden zu " + WiFi.SSID() + "<br>";
    status += "IP: " + WiFi.localIP().toString() + "<br>";
    status += "Signal: " + String(WiFi.RSSI()) + " dBm</p>";
  } else if (apModeActive) {
    status += "<p><strong>WiFi:</strong> Access Point Modus<br>";
    status += "SSID: " + String(AP_SSID) + "<br>";
    status += "IP: " + WiFi.softAPIP().toString() + "<br>";
    status += "Clients: " + String(WiFi.softAPgetStationNum()) + "</p>";
  } else {
    status += "<p><strong>WiFi:</strong> Nicht verbunden</p>";
  }
  
  return status;
}

// ======================================================
// WiFi Konfigurationsseite
// ======================================================
void handleWifiConfig() {
  String html = "<html><head><title>WiFi Konfiguration</title></head><body>";
  html += "<h2>WT32-KG WiFi Setup</h2>";
  
  html += getNetworkStatus();
  
  html += "<h3>WiFi Netzwerke scannen</h3>";
  html += "<button onclick=\"location.href='/wifi/scan'\">Netzwerke scannen</button><br><br>";
  
  html += "<h3>Manuelle WiFi Konfiguration</h3>";
  html += "<form action='/wifi/connect' method='post'>";
  html += "SSID: <input type='text' name='ssid' required><br><br>";
  html += "Passwort: <input type='password' name='password'><br><br>";
  html += "<input type='submit' value='Verbinden'>";
  html += "</form>";
  
  html += "<br><a href='/'>← Zurück zur Hauptseite</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// ======================================================
// WiFi Netzwerke scannen
// ======================================================
void handleWifiScan() {
  String html = "<html><head><title>WiFi Scan</title></head><body>";
  html += "<h2>Verfügbare WiFi Netzwerke</h2>";
  
  int networks = WiFi.scanNetworks();
  
  if (networks == 0) {
    html += "<p>Keine Netzwerke gefunden</p>";
  } else {
    html += "<table border='1'>";
    html += "<tr><th>SSID</th><th>Signal</th><th>Verschlüsselung</th><th>Aktion</th></tr>";
    
    for (int i = 0; i < networks; i++) {
      html += "<tr>";
      html += "<td>" + WiFi.SSID(i) + "</td>";
      html += "<td>" + String(WiFi.RSSI(i)) + " dBm</td>";
      html += "<td>" + String(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Offen" : "Verschlüsselt") + "</td>";
      html += "<td>";
      html += "<form action='/wifi/connect' method='post' style='margin:0;'>";
      html += "<input type='hidden' name='ssid' value='" + WiFi.SSID(i) + "'>";
      html += "<input type='password' name='password' placeholder='Passwort'>";
      html += "<input type='submit' value='Verbinden'>";
      html += "</form>";
      html += "</td>";
      html += "</tr>";
    }
    html += "</table>";
  }
  
  html += "<br><a href='/wifi'>← Zurück</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// ======================================================
// WiFi Verbindung herstellen
// ======================================================
void handleWifiConnect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  String html = "<html><head><title>WiFi Verbindung</title></head><body>";
  html += "<h2>Verbinde zu: " + ssid + "</h2>";
  
  WiFi.begin(ssid.c_str(), password.c_str());
  
  html += "<p>Verbindung wird hergestellt...</p>";
  html += "<script>setTimeout(function(){ window.location.href='/wifi'; }, 5000);</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
  
  // Im Hintergrund verbinden
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) {
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    stopAccessPoint();
    Serial.println("WiFi erfolgreich konfiguriert!");
  }
}