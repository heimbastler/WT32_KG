#ifndef HTML_UTILS_H
#define HTML_UTILS_H

#include <Arduino.h>

inline String escapeHTML(const String &value) {
  unsigned int length = value.length();
  for (unsigned int i = 0; i < value.length(); ++i) {
    switch (value[i]) {
      case '&': length += 4; break;
      case '<':
      case '>': length += 3; break;
      case '"': length += 5; break;
      case '\'': length += 4; break;
    }
  }
  String escaped;
  escaped.reserve(length);
  for (unsigned int i = 0; i < value.length(); ++i) {
    switch (value[i]) {
      case '&': escaped += F("&amp;"); break;
      case '<': escaped += F("&lt;"); break;
      case '>': escaped += F("&gt;"); break;
      case '"': escaped += F("&quot;"); break;
      case '\'': escaped += F("&#39;"); break;
      default: escaped += value[i]; break;
    }
  }
  return escaped;
}

inline String escapeJSONString(const String &value) {
  unsigned int length = value.length();
  for (unsigned int i = 0; i < value.length(); ++i) {
    const unsigned char c = static_cast<unsigned char>(value[i]);
    if (c == '"' || c == '\\') ++length;
    else if (c < 0x20) length += 5;
  }
  String escaped;
  escaped.reserve(length);
  static const char digits[] = "0123456789abcdef";
  for (unsigned int i = 0; i < value.length(); ++i) {
    const unsigned char c = static_cast<unsigned char>(value[i]);
    if (c == '"') escaped += F("\\\"");
    else if (c == '\\') escaped += F("\\\\");
    else if (c < 0x20) {
      escaped += F("\\u00");
      escaped += digits[c >> 4];
      escaped += digits[c & 0x0f];
    } else escaped += value[i];
  }
  return escaped;
}

#endif
