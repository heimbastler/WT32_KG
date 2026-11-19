#!/bin/sh
set -eu

# Optional argument: pre-change main.cpp for byte-for-byte regression comparison.
if [ "$#" -gt 1 ]; then
  printf 'Usage: sh %s [main-before-p5-3.cpp]\n' "$0" >&2
  exit 2
fi
root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
webserver="$HOME/.platformio/packages/framework-arduinoespressif32/libraries/WebServer/src/WebServer.cpp"
tmp=$(mktemp -d)
trap 'rm -f -- "$tmp/main.cpp" "$tmp/check"; rmdir "$tmp"' EXIT

extract_function() {
  awk -v signature="$1" '
    index($0, signature) == 1 { printing = 1 }
    printing { print }
    printing && /^}/ { exit }
  ' "$2"
}

cat > "$tmp/main.cpp" <<'CPP'
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#define PROGMEM
#define PSTR(s) (s)
#define FPSTR(s) (s)
#define F(s) (s)
#define strlen_P strlen
#define memccpy_P memccpy
#define log_w(...) ((void)0)
#define CONTENT_LENGTH_UNKNOWN ((size_t)-1)
#define CONTENT_LENGTH_NOT_SET ((size_t)-2)
using PGM_P = const char *;
using PGM_VOID_P = const void *;

class String {
 public:
  static size_t maxLength, maxReservation;
  std::string value;
  String() = default;
  String(const char *s) : value(s) { observe(); }
  String(const std::string &s) : value(s) { observe(); }
  String(int n) : String(std::to_string(n)) {}
  String(size_t n) : String(std::to_string(n)) {}
  String(float n, int digits) {
    std::ostringstream s;
    s << std::fixed << std::setprecision(digits) << n;
    value = s.str();
    observe();
  }
  void observe() { maxLength = std::max(maxLength, value.size()); }
  void reserve(size_t n) {
    maxReservation = std::max(maxReservation, n);
    value.reserve(n);
  }
  void concat(const char *s, size_t n) { value.append(s, n); observe(); }
  unsigned int length() const { return value.size(); }
  char operator[](size_t i) const { return value[i]; }
  const char *c_str() const { return value.c_str(); }
  long toInt() const { return std::strtol(value.c_str(), nullptr, 10); }
  String &operator+=(const String &s) { value += s.value; observe(); return *this; }
  String &operator+=(char c) { value += c; observe(); return *this; }
  friend String operator+(const String &a, const String &b) { return a.value + b.value; }
  friend String operator+(const String &a, char b) { return a.value + b; }
  friend bool operator==(const String &a, const String &b) { return a.value == b.value; }
  friend bool operator!=(const String &a, const String &b) { return !(a == b); }
};
size_t String::maxLength = 0, String::maxReservation = 0;

namespace mime {
enum { html };
struct Entry { const char *mimeType; };
Entry mimeTable[] = {{"text/html"}};
const char Content_Length[] = "Content-Length";
}

struct FakeClient {
  std::string bytes;
  size_t write(const char *data, size_t size) { bytes.append(data, size); return size; }
};

class WebServer {
 public:
  int _currentVersion = 1;
  size_t _contentLength = CONTENT_LENGTH_NOT_SET;
  bool _chunked = false, _corsEnabled = false;
  String _responseHeaders;
  FakeClient _currentClient;
  size_t flashWrites = 0;
  std::map<std::string, String> arguments;
  bool hasArg(const char *name) { return arguments.count(name) != 0; }
  String arg(const char *name) { return hasArg(name) ? arguments.at(name) : String(); }
  void setContentLength(size_t length) { _contentLength = length; }
  void sendHeader(const String &name, const String &value, bool first = false) {
    String header = name + ": " + value + "\r\n";
    _responseHeaders = first ? header + _responseHeaders : _responseHeaders + header;
  }
  const char *_responseCodeToString(int code) {
    if (code == 200) return "OK";
    if (code == 303) return "See Other";
    assert(code == 400);
    return "Bad Request";
  }
  size_t _currentClientWrite(const char *data, size_t size) {
    return _currentClient.write(data, size);
  }
  size_t _currentClientWrite_P(const char *data, size_t size) {
    ++flashWrites;
    return _currentClient.write(data, size);
  }
  void _prepareHeader(String &, int, const char *, size_t);
  void send(int, const char * = "text/html", const String & = String());
  void send_P(int, PGM_P, PGM_P);
  void sendContent(const String &content) { sendContent(content.c_str(), content.length()); }
  void sendContent(const char *, size_t);
  void sendContent_P(PGM_P);
  void sendContent_P(PGM_P, size_t);
  void _finalizeResponse();
};
CPP

# Exercise the installed WebServer's real HTTP framing; only transport is mocked.
for signature in \
  'void WebServer::_prepareHeader(' \
  'void WebServer::send(int code, const char* content_type, const String& content)' \
  'void WebServer::send_P(int code, PGM_P content_type, PGM_P content) {' \
  'void WebServer::sendContent(const char* content, size_t contentLength)' \
  'void WebServer::sendContent_P(PGM_P content) {' \
  'void WebServer::sendContent_P(PGM_P content, size_t size)' \
  'void WebServer::_finalizeResponse()'
do
  # ESP32 size_t is 32-bit; use the matching host-width format specifier.
  extract_function "$signature" "$webserver" | sed 's/"%x%s"/"%zx%s"/g' >> "$tmp/main.cpp"
done

extract_function 'inline String escapeHTML(' "$root/include/html_utils.h" >> "$tmp/main.cpp"
extract_function 'inline String escapeJSONString(' "$root/include/html_utils.h" >> "$tmp/main.cpp"
extract_function 'inline bool parseStrictInteger(' "$root/include/index_validation.h" >> "$tmp/main.cpp"
extract_function 'inline bool parseIndex(' "$root/include/index_validation.h" >> "$tmp/main.cpp"

cat >> "$tmp/main.cpp" <<'CPP'
WebServer server;
bool mpr121Enabled;
int ledDimmerBrightness, acDimmerBrightness;
int relayState[24], inputState[16];
String relayNames[24], inputNames[16], coverNames[2];
String mpr121Name, ledDimmerName, acDimmerName;
bool mpr121HAEnabled, ledDimmerHAEnabled, acDimmerHAEnabled;
bool relayHAEnabled[24], inputHAEnabled[16], coverHAEnabled[2];
String relayHAType[24], inputHAType[16];
struct Cover { float position; bool resetToTopPending; } covers[2];
const int coverRelayPairs[2][2] = {{0, 1}, {2, 3}};
float schaltschrankTemp;
String getTemperatureHTML();
void setLEDDimmerBrightness(uint8_t value) { ledDimmerBrightness = value; }
void setACDimmerBrightness(uint8_t value) { acDimmerBrightness = value; }
struct { bool enabled; char broker[64]; int port; char user[32], password[32]; } mqttConfig;
struct { bool connected() { return false; } } mqttClient;
struct { void println(const String &) {} } Serial;
const char *MQTT_BASE_TOPIC = "topic&base", *MQTT_DISCOVERY_PREFIX = "home&assistant";
void publishMQTTDiscovery() {}
void saveRelayName(int i, const String &name) { relayNames[i] = name; }
void saveRelayHAConfig(int i, const String &type, bool enabled) { relayHAType[i] = type; relayHAEnabled[i] = enabled; }
void saveCoverConfig(int i, const String &name, bool enabled) { coverNames[i] = name; coverHAEnabled[i] = enabled; }
void saveInputConfig(int i, const String &name, const String &type, bool enabled) {
  inputNames[i] = name; inputHAType[i] = type; inputHAEnabled[i] = enabled;
}
void saveSpecialDeviceConfig(int i, const String &name, bool enabled) {
  if (i == 0) { mpr121Name = name; mpr121HAEnabled = enabled; }
  if (i == 1) { ledDimmerName = name; ledDimmerHAEnabled = enabled; }
  if (i == 2) { acDimmerName = name; acDimmerHAEnabled = enabled; }
}
enum ClientType { CLIENT_TYPE_UNKNOWN, CLIENT_TYPE_SENSOR, CLIENT_TYPE_SWITCH, CLIENT_TYPE_RELAY, CLIENT_TYPE_DIMMER, CLIENT_TYPE_CUSTOM };
struct ESPNowClient {
  char name[32]; ClientType type; bool online; uint8_t mac[6];
  unsigned long lastSeen; float sensorValue1, sensorValue2; bool switchState; int dimmerLevel;
} espnowClients[1];
int espnowClientCount = 1;
bool espnowPairingMode;
struct { String macAddress() { return "00:11:22:33:44:55"; } } WiFi;
unsigned long millis() { return 60000; }
CPP

sed -n '/^static const char HTML_HEADER_START\[\]/,/^\/\/ ===== ESP-NOW Seite =====/p' \
  "$root/src/main.cpp" | sed '$d' >> "$tmp/main.cpp"
extract_function 'String getTemperatureHTML() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
extract_function 'void handleHomeState() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
extract_function 'void handleInputs() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
extract_function 'void handleLEDDimmer() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
extract_function 'void handleACDimmer() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
extract_function 'void handleEdit() {' "$root/src/main.cpp" >> "$tmp/main.cpp"
for signature in 'void handleSaveName() {' 'void handleSaveCover() {' \
  'void handleSaveInput() {' 'void handleSaveSpecial() {' 'void handleMQTT() {' 'void handleESPNow() {'
do
  extract_function "$signature" "$root/src/main.cpp" >> "$tmp/main.cpp"
done
for signature in 'String macToString(' 'String getClientListHTML() {' 'String getClientDetailHTML('
do
  extract_function "$signature" "$root/src/espnow_gateway.cpp" >> "$tmp/main.cpp"
done

baseline_flag=
if [ "$#" -gt 0 ]; then
  printf '\nnamespace baseline { WebServer server;\n' >> "$tmp/main.cpp"
  sed -n '/^String getHTMLHeader(String activeTab) {/,/^\/\/ ===== ESP-NOW Seite =====/p' \
    "$1" | sed '$d' >> "$tmp/main.cpp"
  extract_function 'void handleEdit() {' "$1" >> "$tmp/main.cpp"
  printf '\n}\n' >> "$tmp/main.cpp"
  baseline_flag=-DHAVE_BASELINE
fi

cat >> "$tmp/main.cpp" <<'CPP'
std::string decode(const std::string &wire, bool chunked) {
  size_t offset = wire.find("\r\n\r\n");
  assert(offset != std::string::npos);
  std::string headers = wire.substr(0, offset);
  assert(headers.find("Content-Type: text/html; charset=UTF-8\r\n") != std::string::npos);
  offset += 4;
  if (!chunked) {
    assert(headers.find("Transfer-Encoding:") == std::string::npos);
    return wire.substr(offset);
  }
  assert(headers.find("Transfer-Encoding: chunked\r\n") != std::string::npos);
  assert(headers.find("Content-Length:") == std::string::npos);
  std::string body;
  for (;;) {
    size_t end = wire.find("\r\n", offset);
    assert(end != std::string::npos);
    std::string length = wire.substr(offset, end - offset);
    size_t consumed = 0;
    size_t size = std::stoul(length, &consumed, 16);
    assert(consumed == length.size());
    offset = end + 2;
    assert(size <= wire.size() - offset);
    body.append(wire, offset, size);
    offset += size;
    assert(wire.compare(offset, 2, "\r\n") == 0);
    offset += 2;
    if (size == 0) {
      assert(offset == wire.size());
      break;
    }
  }
  return body;
}

int main() {
  assert(escapeJSONString("\"\\\n\t") == "\\\"\\\\\\u000a\\u0009");
#ifdef HAVE_BASELINE
  for (const char *tab : {"home", "edit", "mqtt", "info", "espnow", "other"}) {
    assert(getHTMLHeader(tab) == baseline::getHTMLHeader(tab));
  }
  assert(getHTMLFooter() == baseline::getHTMLFooter());
#endif
  size_t peak = 0, peakReservation = 0;
  for (int version : {0, 1}) {
    for (int scenario = 0; scenario < 36; ++scenario) {
      mpr121Enabled = scenario & 1;
      ledDimmerBrightness = scenario * 255 / 35;
      acDimmerBrightness = 255 - ledDimmerBrightness;
      covers[0].position = scenario * 100.0f / 35;
      covers[1].position = 100.0f - covers[0].position;
      covers[0].resetToTopPending = scenario & 1;
      covers[1].resetToTopPending = false;
      const float temperatures[] = {-999, 5, 20, 32, 40};
      schaltschrankTemp = temperatures[scenario % 5];
      String name = scenario < 32 ? "Tür & Küche" : String(std::string(30, "\"'<&"[scenario - 32]));
      for (int i = 0; i < 24; ++i) {
        relayState[i] = (scenario >> (i % 5)) & 1;
        relayNames[i] = name;
      }
      for (int i = 0; i < 16; ++i) {
        inputState[i] = (scenario >> (i % 5)) & 1;
        inputNames[i] = name;
      }
      coverNames[0] = coverNames[1] = name;
      server = WebServer();
      server._currentVersion = version;
      String::maxLength = String::maxReservation = 0;
      handleHome();
      assert(!server._chunked); // Handler, not framework fallback, finishes the response.
      assert(server._contentLength == CONTENT_LENGTH_UNKNOWN);
      assert(server.flashWrites > 20);
      assert(String::maxLength <= 1024);
      assert(String::maxReservation <= 1024);
      peak = std::max(peak, String::maxLength);
      peakReservation = std::max(peakReservation, String::maxReservation);
      const std::string wire = server._currentClient.bytes;
      server._finalizeResponse();
      assert(wire == server._currentClient.bytes); // No duplicate terminal chunk.
      std::string body = decode(wire, version != 0);
      assert(body.find(".info-box{background:#1a237e;border-color:#3f51b5;margin:15px 0;}") != std::string::npos);
      assert(body.find("<div class='card info-box' id='cover-timer-info'>") != std::string::npos);
      assert(body.find("47 Sekunden (Fenster) bzw. 48 Sekunden (Tür)") != std::string::npos);
      assert(body.find("nicht gemessen") != std::string::npos);
      assert(body.find("Reset auf oben&ldquo;") != std::string::npos);
      for (int i = 4; i < 24; ++i) {
        assert(body.find("id='relay-" + std::to_string(i) + "'") != std::string::npos);
      }
      const char *mcpPins[] = {
        "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",
        "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7"
      };
      for (int i = 0; i < 16; ++i) {
        assert(body.find("id='input-" + std::to_string(i) + "-status'") != std::string::npos);
        assert(body.find("<tr><td>IN" + std::to_string(i) + " (" + mcpPins[i] + ")</td>")
               != std::string::npos);
      }
      for (int i = 0; i < 2; ++i) {
        for (const char *action : {"OPEN", "CLOSE", "STOP", "RESET"}) {
          assert(body.find("coverAction(" + std::to_string(i) + ", \"" + action + "\", this)")
                 != std::string::npos);
        }
      }
      assert(body.find("<td>" + escapeHTML(name).value + "</td>") != std::string::npos);
      const std::string footer = "</div></div></body></html>";
      assert(body.size() >= footer.size());
      assert(body.compare(body.size() - footer.size(), footer.size(), footer) == 0);
#ifdef HAVE_BASELINE
      baseline::server = WebServer();
      baseline::server._currentVersion = version;
      baseline::handleHome();
      const auto baselineWire = baseline::server._currentClient.bytes;
      assert(body == decode(baselineWire, baselineWire.find("Transfer-Encoding: chunked\r\n") != std::string::npos));
#endif
    }
  }
  std::cout << "PASS: 72 Home responses, HTTP/1.0 and HTTP/1.1 framing, one transfer "
               "terminator; maximum String length " << peak << " bytes, reservation "
            << peakReservation << " bytes";
#ifdef HAVE_BASELINE
  std::cout << "; byte-identical to baseline, including six header tabs";
#endif
  std::cout << '\n';
  size_t editPeak = 0, editReservation = 0;
  for (int version : {0, 1}) {
    for (int scenario = 0; scenario < 8; ++scenario) {
      mpr121Name = ledDimmerName = acDimmerName = "Device name";
      mpr121HAEnabled = scenario & 1;
      ledDimmerHAEnabled = scenario & 2;
      acDimmerHAEnabled = scenario & 4;
      for (int i = 0; i < 24; ++i) {
        relayNames[i] = "Relay";
        relayHAEnabled[i] = (scenario + i) % 2;
        const char *types[] = {"none", "switch", "light", "cover", "fan"};
        relayHAType[i] = types[(scenario + i) % 5];
      }
      for (int i = 0; i < 16; ++i) {
        inputNames[i] = "Input";
        inputHAEnabled[i] = (scenario + i) % 2;
        const char *types[] = {"none", "switch", "door", "window", "motion", "occupancy", "opening", "garage_door"};
        inputHAType[i] = types[(scenario + i) % 8];
      }
      for (int i = 0; i < 2; ++i) {
        coverNames[i] = "Cover";
        coverHAEnabled[i] = (scenario + i) % 2;
      }
      server = WebServer();
      server._currentVersion = version;
      String::maxLength = String::maxReservation = 0;
      handleEdit();
      assert(!server._chunked);
      assert(String::maxLength <= 1024 && String::maxReservation <= 1024);
      editPeak = std::max(editPeak, String::maxLength);
      editReservation = std::max(editReservation, String::maxReservation);
      std::string body = decode(server._currentClient.bytes, version != 0);
      assert(body.find("<div class='card info-box' id='auto-calibration-info'>") != std::string::npos);
      assert(body.find("<p class='info-title'><b>⏰ Auto-Kalibrierung:</b></p>") != std::string::npos);
      const auto wire = server._currentClient.bytes;
      server._finalizeResponse();
      assert(wire == server._currentClient.bytes);
      for (int i = 4; i < 24; ++i) {
        assert(body.find("id='name" + std::to_string(i) + "' value='Relay'") != std::string::npos);
        assert(body.find("saveConfig(" + std::to_string(i) + ")") != std::string::npos);
      }
      for (int i = 0; i < 16; ++i) {
        assert(body.find("id='inputname" + std::to_string(i) + "' value='Input'") != std::string::npos);
        assert(body.find("saveInputConfig(" + std::to_string(i) + ")") != std::string::npos);
      }
#ifdef HAVE_BASELINE
      baseline::server = WebServer();
      baseline::server._currentVersion = version;
      baseline::handleEdit();
      assert(body == decode(baseline::server._currentClient.bytes, version != 0));
#endif
    }
  }
  std::cout << "PASS: 16 Edit responses; all HA types/flags and save controls; maximum String "
            << editPeak << " bytes, reservation " << editReservation << " bytes\n";
  for (int clients : {0, 1}) {
    for (bool pairing : {false, true}) {
      espnowClientCount = clients;
      espnowPairingMode = pairing;
      server = WebServer();
      handleESPNow();
      const auto body = decode(server._currentClient.bytes, false);
      assert(body.find("<div class='card info-box' id='espnow-howto'>") != std::string::npos);
      assert(body.find("src/espnow_gateway.h") != std::string::npos);
      assert(body.find("Gateway-Neustart erneut pairen") != std::string::npos);
      assert(body.find("endet derzeit nicht automatisch") != std::string::npos);
      assert(body.find("<code>00:11:22:33:44:55</code>") != std::string::npos);
      assert(body.find(pairing ? "Pairing AKTIV" : "Pairing aktivieren") != std::string::npos);
      assert(body.find("background:#e3f2fd") == std::string::npos);
    }
  }
  server = WebServer();
  handleMQTT();
  const auto mqttInfo = decode(server._currentClient.bytes, false);
  assert(mqttInfo.find("<div class='card info-box' id='ha-integration-info'>") != std::string::npos);
  assert(mqttInfo.find("<p class='info-title'><b>📌 Home Assistant Integration:</b></p>") != std::string::npos);
  assert(mqttInfo.find("<code>topic&amp;base</code>") != std::string::npos);
  assert(mqttInfo.find("<code>home&amp;assistant</code>") != std::string::npos);
  assert(mqttInfo.find("LED- und AC-Dimmer") != std::string::npos);
  std::cout << "PASS: shared blue info boxes, cover timer/reset explanations, HA guidance and ESP-NOW how-to with zero/one client and both pairing modes\n";
  for (auto handler : {handleLEDDimmer, handleACDimmer}) {
    for (const char *value : {"0", "50", "100"}) {
      server = WebServer();
      server.arguments["brightness"] = value;
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 303") == 0);
      assert(server._currentClient.bytes.find("Location: /\r\n") != std::string::npos);
      server = WebServer();
      server.arguments["brightness"] = value;
      server.arguments["ajax"] = "1";
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 200") == 0);
      assert(server._currentClient.bytes.find("{\"success\":true}") != std::string::npos);
      assert(server._currentClient.bytes.find("Location:") == std::string::npos);
      assert((handler == handleLEDDimmer ? ledDimmerBrightness : acDimmerBrightness)
             == std::atoi(value) * 255 / 100);
    }
    for (const char *value : {"", "-1", "101", "abc", "1x"}) {
      server = WebServer();
      server.arguments["brightness"] = value;
      server.arguments["ajax"] = "1";
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 400") == 0);
    }
  }
  std::cout << "PASS: both dimmer routes preserve legacy redirects, validate AJAX values and return JSON without redirects\n";
  const String hostile = "<tag>\"'&\\ Tür";
  for (const String &name : {hostile, String(std::string(30, '"')), String("&lt;"), String("Küche")}) {
    mpr121Name = ledDimmerName = acDimmerName = name;
    for (auto &value : relayNames) value = name;
    for (auto &value : inputNames) value = name;
    for (auto &value : coverNames) value = name;
    server = WebServer();
    String::maxLength = String::maxReservation = 0;
    handleEdit();
    assert(String::maxLength <= 1024 && String::maxReservation <= 1024);
    auto body = decode(server._currentClient.bytes, true);
    assert(body.find("value='" + escapeHTML(name).value + "'") != std::string::npos);
  }
  std::cout << "PASS: Edit names escape special characters, existing entities, UTF-8 and maximum-length expansion within 1024-byte String bounds\n";
  if (const char *directory = std::getenv("HOME_POLLING_FIXTURE_DIR")) {
    std::string header = getHTMLHeader("home").value;
    size_t start = header.find("<script>") + 8;
    size_t end = header.find("</script>", start);
    assert(start >= 8 && end != std::string::npos);
    std::ofstream script(std::string(directory) + "/home.js");
    script << header.substr(start, end - start);
    script.close();
    assert(script.good());
    server = WebServer();
    handleHomeState();
    std::ofstream state(std::string(directory) + "/home-state.json");
    state << server._currentClient.bytes.substr(server._currentClient.bytes.find("\r\n\r\n") + 4);
    state.close();
    assert(state.good());
    server = WebServer();
    handleInputs();
    std::ofstream inputs(std::string(directory) + "/inputs.json");
    inputs << server._currentClient.bytes.substr(server._currentClient.bytes.find("\r\n\r\n") + 4);
    inputs.close();
    assert(inputs.good());
    mpr121Name = ledDimmerName = acDimmerName = hostile;
    for (auto &value : relayNames) value = hostile;
    for (auto &value : inputNames) value = hostile;
    for (auto &value : coverNames) value = hostile;
    std::strcpy(mqttConfig.broker, hostile.c_str());
    std::strcpy(mqttConfig.user, hostile.c_str());
    std::strcpy(mqttConfig.password, hostile.c_str());
    std::strcpy(espnowClients[0].name, hostile.c_str());
    espnowClients[0].type = CLIENT_TYPE_SENSOR;
    espnowClients[0].online = true;
    server = WebServer();
    handleHome();
    const String home = decode(server._currentClient.bytes, true);
    server = WebServer();
    handleEdit();
    const String edit = decode(server._currentClient.bytes, true);
    const size_t editStart = edit.value.rfind("<script>") + 8;
    const size_t editEnd = edit.value.find("</script>", editStart);
    assert(editStart >= 8 && editEnd != std::string::npos);
    std::ofstream editScript(std::string(directory) + "/edit.js");
    editScript << edit.value.substr(editStart, editEnd - editStart);
    editScript.close();
    assert(editScript.good());
    server = WebServer();
    handleMQTT();
    const String mqtt = decode(server._currentClient.bytes, false);
    server = WebServer();
    handleESPNow();
    const String espnow = decode(server._currentClient.bytes, false);
    String saves = "[";
    int item = 0;
    for (auto handler : {handleSaveName, handleSaveCover, handleSaveInput, handleSaveSpecial}) {
      server = WebServer();
      server.arguments["relay"] = "4";
      server.arguments["cover"] = server.arguments["input"] = server.arguments["device"] = "0";
      server.arguments["name"] = hostile;
      server.arguments["enabled"] = "1";
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 200") == 0);
      if (item++) saves += ",";
      saves += server._currentClient.bytes.substr(server._currentClient.bytes.find("\r\n\r\n") + 4);
    }
    saves += "]";
    assert(relayNames[4] == hostile && coverNames[0] == hostile && inputNames[0] == hostile && mpr121Name == hostile);
    for (auto handler : {handleSaveName, handleSaveCover, handleSaveInput, handleSaveSpecial}) {
      server = WebServer();
      server.arguments["relay"] = "4";
      server.arguments["cover"] = server.arguments["input"] = server.arguments["device"] = "0";
      server.arguments["name"] = std::string(30, '"');
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 200") == 0);
      server = WebServer();
      server.arguments["relay"] = "4";
      server.arguments["cover"] = server.arguments["input"] = server.arguments["device"] = "0";
      server.arguments["name"] = std::string(31, 'x');
      handler();
      assert(server._currentClient.bytes.find("HTTP/1.1 400") == 0);
    }
    assert(relayNames[4] == String(std::string(30, '"')) && coverNames[0] == relayNames[4]
           && inputNames[0] == relayNames[4] && mpr121Name == relayNames[4]);
    String controls;
    for (char c = 0; c < 0x20; ++c) controls += c;
    std::ofstream escaping(std::string(directory) + "/web-escaping.json");
    escaping << "{\"name\":\"" << escapeJSONString(hostile).value
             << "\",\"home\":\"" << escapeJSONString(home).value
             << "\",\"edit\":\"" << escapeJSONString(edit).value
             << "\",\"mqtt\":\"" << escapeJSONString(mqtt).value
             << "\",\"espnow\":\"" << escapeJSONString(espnow).value
             << "\",\"clientList\":\"" << escapeJSONString(getClientListHTML()).value
             << "\",\"clientDetail\":\"" << escapeJSONString(getClientDetailHTML(0)).value
             << "\",\"controls\":\"" << escapeJSONString(controls).value
             << "\",\"saves\":" << saves.value << "}";
    escaping.close();
    assert(escaping.good());
  }
}
CPP

c++ -std=c++11 -Wall -Wextra -Werror -Wno-deprecated-declarations -Wno-error=switch \
  ${baseline_flag:+$baseline_flag} "$tmp/main.cpp" -o "$tmp/check"
"$tmp/check"
