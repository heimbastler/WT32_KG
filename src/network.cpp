// #include <WiFi.h>  // WiFi deaktiviert - nur Ethernet
#include <WebServer.h>
#include <ETH.h>
#include "config.h"

// Externe Referenzen
extern WebServer server;

// Ethernet Status
bool ethConnected = false;

// Funktionsprototypen
void initNetworking();
String getNetworkStatus();

// ======================================================
// Network Initialisierung (Ethernet Only)
// ⚠️  ETHERNET GPIO KONFIGURATION - NIEMALS ÄNDERN! ⚠️
// ⚠️  WiFi wird NUR für ESP-NOW verwendet (siehe espnow_gateway.cpp)
// ⚠️  KEIN WiFi-STA oder WiFi-AP für Internet-Verbindung!
// ======================================================
void initNetworking() {
  Serial.println("=== Network Setup (Ethernet Only) ===");
  Serial.println("    WiFi wird später NUR für ESP-NOW initialisiert!");
  
  // ✅ GEPRÜFT & FUNKTIONIEREND (30.03.2026)
  // ETH.begin() ohne Parameter verwendet WT32-ETH01 Board-Defaults
  // Hardware-reservierte GPIOs: 0, 18, 19, 21, 22, 23, 25, 26, 27
  // 🔴 NIEMALS mit manuellen GPIO-Parametern aufrufen!
  ETH.begin();

#ifdef ETH_USE_STATIC_IP
  // IP-Konfiguration auch ohne aktiven Link setzen, damit späteres Einstecken funktioniert.
  IPAddress dns1(8, 8, 8, 8);
  IPAddress dns2(192, 168, 178, 1);
  ETH.config(ETH_STATIC_IP, ETH_GATEWAY, ETH_SUBNET, dns1, dns2);
  Serial.println("📌 Statische Ethernet-Konfiguration angefordert");
#else
  Serial.println("📡 Ethernet wartet auf Link und DHCP-Adresse");
#endif

  const unsigned long networkStartupTimeoutMs = 10000;
  const unsigned long startupStartedAt = millis();
  while (millis() - startupStartedAt < networkStartupTimeoutMs &&
         (!ETH.linkUp() || ETH.localIP() == IPAddress(0, 0, 0, 0))) {
    delay(100);
  }

  const bool linkUp = ETH.linkUp();
  const IPAddress localIP = ETH.localIP();
  ethConnected = linkUp && localIP != IPAddress(0, 0, 0, 0);

  if (!linkUp) {
    Serial.println("❌ Ethernet-Link nach 10 Sekunden nicht verfügbar; fahre offline fort.");
  } else if (localIP == IPAddress(0, 0, 0, 0)) {
#ifdef ETH_USE_STATIC_IP
    Serial.println("❌ Ethernet-Link steht, aber die statische IP wurde nicht aktiviert.");
    Serial.println("   IP-Konfiguration und mögliche Adresskonflikte prüfen.");
#else
    Serial.println("❌ Ethernet-Link steht, aber DHCP lieferte innerhalb von 10 Sekunden keine IP.");
#endif
  } else {
#ifdef ETH_USE_STATIC_IP
    Serial.println("✅ Statische Ethernet-IP aktiv");
#else
    Serial.println("✅ DHCP IP-Adresse erhalten");
#endif
    Serial.print("   IP: ");
    Serial.println(localIP);
    Serial.print("   MAC: ");
    Serial.println(ETH.macAddress());
    Serial.print("   Hostname: ");
    Serial.println(HOSTNAME);
    Serial.println("╔═══════════════════════════════════════╗");
    Serial.print("║  WEB-INTERFACE: http://");
    Serial.print(localIP);
    Serial.println("  ║");
    Serial.println("╚═══════════════════════════════════════╝");
  }
}

// ======================================================
// Network Status abfragen
// ======================================================
String getNetworkStatus() {
  String status = "<h3>🌐 Netzwerk Status</h3>";
  status += "<div style='background:#f0f0f0; padding:15px; border-radius:5px;'>";
  
  if (ETH.linkUp()) {
    status += "<p><strong>✅ Ethernet:</strong> Verbunden<br>";
    status += "<strong>IP-Adresse:</strong> " + ETH.localIP().toString() + "<br>";
    status += "<strong>MAC-Adresse:</strong> " + ETH.macAddress() + "<br>";
    status += "<strong>Hostname:</strong> " + String(HOSTNAME) + "<br>";
    status += "<strong>Link Speed:</strong> " + String(ETH.linkSpeed()) + " Mbps<br>";
    status += "<strong>Full Duplex:</strong> " + String(ETH.fullDuplex() ? "Ja" : "Nein") + "</p>";
  } else {
    status += "<p><strong>❌ Ethernet:</strong> Nicht verbunden</p>";
    status += "<p style='color:red;'>Kein Ethernet-Kabel erkannt!</p>";
  }
  
  status += "</div>";
  return status;
}