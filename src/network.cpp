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
// ======================================================
void initNetworking() {
  Serial.println("=== Network Setup (Ethernet Only) ===");
  
  // ✅ GEPRÜFT & FUNKTIONIEREND (30.03.2026)
  // ETH.begin() ohne Parameter verwendet WT32-ETH01 Board-Defaults
  // Hardware-reservierte GPIOs: 0, 18, 19, 21, 22, 23, 25, 26, 27
  // 🔴 NIEMALS mit manuellen GPIO-Parametern aufrufen!
  ETH.begin();
  
  Serial.println("Warte auf Ethernet-Verbindung...");
  delay(2000);
  
  // Prüfen ob Ethernet verbunden ist
  if (ETH.linkUp()) {
    Serial.println("✅ Ethernet Kabel erkannt");
    ethConnected = true;
    
    #ifdef ETH_USE_STATIC_IP
    ETH.config(ETH_STATIC_IP, ETH_GATEWAY, ETH_SUBNET);
    Serial.println("📌 Ethernet: Statische IP konfiguriert");
    Serial.print("   IP: ");
    Serial.println(ETH.localIP());
    #else
    // Warte auf DHCP IP-Zuweisung
    int timeout = 0;
    while (ETH.localIP() == IPAddress(0, 0, 0, 0) && timeout < 20) {
      delay(500);
      Serial.print(".");
      timeout++;
    }
    Serial.println();
    
    if (ETH.localIP() != IPAddress(0, 0, 0, 0)) {
      Serial.println("✅ DHCP IP-Adresse erhalten");
      Serial.print("   IP: ");
      Serial.println(ETH.localIP());
      Serial.print("   MAC: ");
      Serial.println(ETH.macAddress());
      Serial.print("   Hostname: ");
      Serial.println(HOSTNAME);
      Serial.println();
      Serial.println("╔═══════════════════════════════════════╗");
      Serial.print("║  WEB-INTERFACE: http://");
      Serial.print(ETH.localIP());
      Serial.println("  ║");
      Serial.println("╚═══════════════════════════════════════╝");
    } else {
      Serial.println("❌ FEHLER: Keine DHCP IP-Adresse erhalten!");
    }
    #endif
  } else {
    Serial.println("❌ FEHLER: Kein Ethernet Kabel erkannt!");
    Serial.println("   Bitte Ethernet-Kabel anschließen und neu starten.");
    ethConnected = false;
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