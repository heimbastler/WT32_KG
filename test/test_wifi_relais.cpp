// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║                    WiFi AP RELAIS CONTROLLER TEST                           ║
// ║                              Version 1.0                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
//
// Erstellt einen WiFi Access Point "WT32-Relais-Test"
// Öffnet Web-Interface mit 24 Relais-Buttons
// Einfaches Testen aller PCA9535 Relais über Smartphone

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_PCA9535.h>

// ---------- WiFi AP Setup ----------
const char* ap_ssid = "WT32-Relais-Test";
const char* ap_password = "12345678";  // Mindestens 8 Zeichen für WPA2

// ---------- I2C Setup ----------
#define SDA_PIN 33 // GPIO33
#define SCL_PIN 32 // GPIO32

// ---------- PCA9535 Relais Boards ----------
Adafruit_PCA9535 pcaRel1; // Board A: 0x22
Adafruit_PCA9535 pcaRel2; // Board B: 0x23  
Adafruit_PCA9535 pcaRel3; // Board C: 0x24

// ---------- Webserver ----------
WebServer server(80);

// ---------- Relais Status Array (24 Relais) ----------
bool relayState[24] = {false}; // false = AUS, true = EIN

// ---------- HTML Web-Interface ----------
String getWebPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>WT32 Relais Test</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }";
  html += "h1 { color: #2c3e50; text-align: center; }";
  html += ".board { background: white; padding: 20px; margin: 10px 0; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }";
  html += ".board h2 { color: #34495e; margin-top: 0; border-bottom: 2px solid #3498db; padding-bottom: 10px; }";
  html += ".relay-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); gap: 10px; }";
  html += ".relay-btn { padding: 15px; border: none; border-radius: 5px; font-size: 14px; font-weight: bold; cursor: pointer; transition: all 0.3s; }";
  html += ".relay-off { background: #e74c3c; color: white; }";
  html += ".relay-on { background: #27ae60; color: white; }";
  html += ".relay-btn:hover { transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.2); }";
  html += ".status { text-align: center; margin: 20px 0; padding: 15px; background: #3498db; color: white; border-radius: 5px; }";
  html += ".footer { text-align: center; margin-top: 30px; color: #7f8c8d; font-size: 12px; }";
  html += "</style></head><body>";
  
  html += "<h1>🏠 WT32 Relais Controller</h1>";
  html += "<div class='status'>WiFi AP: <strong>" + String(ap_ssid) + "</strong> | IP: <strong>" + WiFi.softAPIP().toString() + "</strong></div>";
  
  // Board A (Relais 0-7)
  html += "<div class='board'>";
  html += "<h2>🔸 Board A (0x22) - Relais R00-R07</h2>";
  html += "<div class='relay-grid'>";
  for (int i = 0; i < 8; i++) {
    String btnClass = relayState[i] ? "relay-on" : "relay-off";
    String btnText = relayState[i] ? "EIN" : "AUS";
    html += "<button class='relay-btn " + btnClass + "' onclick='toggleRelay(" + String(i) + ")'>R" + String(i, DEC);
    if (i < 10) html += "0";
    html += String(i) + "<br>" + btnText + "</button>";
  }
  html += "</div></div>";
  
  // Board B (Relais 8-15)
  html += "<div class='board'>";
  html += "<h2>🔸 Board B (0x23) - Relais R08-R15</h2>";
  html += "<div class='relay-grid'>";
  for (int i = 8; i < 16; i++) {
    String btnClass = relayState[i] ? "relay-on" : "relay-off";
    String btnText = relayState[i] ? "EIN" : "AUS";
    html += "<button class='relay-btn " + btnClass + "' onclick='toggleRelay(" + String(i) + ")'>R" + String(i) + "<br>" + btnText + "</button>";
  }
  html += "</div></div>";
  
  // Board C (Relais 16-23)
  html += "<div class='board'>";
  html += "<h2>🔸 Board C (0x24) - Relais R16-R23</h2>";
  html += "<div class='relay-grid'>";
  for (int i = 16; i < 24; i++) {
    String btnClass = relayState[i] ? "relay-on" : "relay-off";
    String btnText = relayState[i] ? "EIN" : "AUS";
    html += "<button class='relay-btn " + btnClass + "' onclick='toggleRelay(" + String(i) + ")'>R" + String(i) + "<br>" + btnText + "</button>";
  }
  html += "</div></div>";
  
  html += "<div class='footer'>WT32-ETH01 Smart Home Controller | Test-Modus</div>";
  
  // JavaScript für AJAX Toggle
  html += "<script>";
  html += "function toggleRelay(relay) {";
  html += "  fetch('/toggle?relay=' + relay)";
  html += "    .then(response => response.text())";
  html += "    .then(data => { location.reload(); })";
  html += "    .catch(error => console.log('Error:', error));";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  return html;
}

// ---------- HTTP Handlers ----------
void handleRoot() {
  server.send(200, "text/html", getWebPage());
}

void handleToggle() {
  if (server.hasArg("relay")) {
    int relayNum = server.arg("relay").toInt();
    
    if (relayNum >= 0 && relayNum < 24) {
      // Toggle Relais Status
      relayState[relayNum] = !relayState[relayNum];
      
      // Hardware schalten
      int boardIndex = relayNum / 8;  // 0, 1, oder 2
      int pinIndex = relayNum % 8;    // 0-7
      
      bool pinState = relayState[relayNum] ? HIGH : LOW; // Relais sind HIGH-aktiv (invertiert)
      
      switch (boardIndex) {
        case 0: // Board A (0x22)
          pcaRel1.digitalWrite(pinIndex, pinState);
          break;
        case 1: // Board B (0x23)
          pcaRel2.digitalWrite(pinIndex, pinState);
          break;
        case 2: // Board C (0x24)
          pcaRel3.digitalWrite(pinIndex, pinState);
          break;
      }
      
      Serial.printf("Relais R%02d → %s (Board %c, Pin %d)\n", 
                    relayNum, 
                    relayState[relayNum] ? "EIN" : "AUS",
                    'A' + boardIndex,
                    pinIndex);
    }
  }
  
  server.send(200, "text/plain", "OK");
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║    WT32 WiFi AP Relais Test         ║");
  Serial.println("╚══════════════════════════════════════╝");
  
  // I2C initialisieren
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);
  Serial.println("✅ I2C Bus initialisiert (SDA: GPIO33, SCL: GPIO32)");
  
  // PCA9535 Boards initialisieren
  Serial.println("\n📡 Initialisiere PCA9535 Relais Boards...");
  
  if (pcaRel1.begin(0x22)) {
    Serial.println("✅ Board A (0x22) gefunden");
  } else {
    Serial.println("❌ Board A (0x22) NICHT gefunden!");
  }
  
  if (pcaRel2.begin(0x23)) {
    Serial.println("✅ Board B (0x23) gefunden");
  } else {
    Serial.println("❌ Board B (0x23) NICHT gefunden!");
  }
  
  if (pcaRel3.begin(0x24)) {
    Serial.println("✅ Board C (0x24) gefunden");
  } else {
    Serial.println("❌ Board C (0x24) NICHT gefunden!");
  }
  
  // Alle Relais als OUTPUT und AUS
  for (int i = 0; i < 8; i++) {
    pcaRel1.pinMode(i, OUTPUT);
    pcaRel1.digitalWrite(i, LOW); // LOW = Relais AUS (invertiert)
    pcaRel2.pinMode(i, OUTPUT);
    pcaRel2.digitalWrite(i, LOW); // LOW = Relais AUS (invertiert)
    pcaRel3.pinMode(i, OUTPUT);
    pcaRel3.digitalWrite(i, LOW); // LOW = Relais AUS (invertiert)
  }
  Serial.println("🔌 Alle 24 Relais auf AUS gesetzt");
  
  // WiFi Access Point starten
  Serial.println("\n📶 Starte WiFi Access Point...");
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.println("✅ WiFi AP gestartet!");
  Serial.println("📱 SSID: " + String(ap_ssid));
  Serial.println("🔑 Passwort: " + String(ap_password));
  Serial.println("🌐 IP Adresse: " + IP.toString());
  Serial.println("🔗 Web-Interface: http://" + IP.toString());
  
  // HTTP Server Routen
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  
  // Webserver starten
  server.begin();
  Serial.println("🚀 Webserver gestartet!");
  
  Serial.println("\n╔══════════════════════════════════════╗");
  Serial.println("║         BEREIT ZUM TESTEN!          ║");
  Serial.println("║                                      ║");
  Serial.println("║ 1. Mit Handy zu WiFi verbinden:     ║");
  Serial.println("║    SSID: WT32-Relais-Test           ║");
  Serial.println("║    Passwort: 12345678                ║");
  Serial.println("║                                      ║");
  Serial.println("║ 2. Browser öffnen und eingeben:     ║");
  Serial.printf("║    http://%-23s ║\n", IP.toString().c_str());
  Serial.println("║                                      ║");
  Serial.println("║ 3. Relais mit Buttons testen! 🎯   ║");
  Serial.println("╚══════════════════════════════════════╝");
}

// ---------- Loop ----------
void loop() {
  server.handleClient();
  
  // Zeige WiFi Status alle 30 Sekunden
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 30000) {
    lastStatus = millis();
    Serial.printf("📊 Verbundene Clients: %d | Uptime: %lu min\n", 
                  WiFi.softAPgetStationNum(), 
                  millis() / 60000);
  }
  
  delay(10);
}