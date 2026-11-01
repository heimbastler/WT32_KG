#ifndef INDEX_VALIDATION_H
#define INDEX_VALIDATION_H

#include <Arduino.h>
#include <stdlib.h>

inline bool parseStrictInteger(const String &value, long &parsedValue) {
  if (value.length() == 0) return false;

  char *end = nullptr;
  parsedValue = strtol(value.c_str(), &end, 10);
  return end != value.c_str() && *end == '\0';
}

inline bool parseIndex(const String &value, int upperBound, int &index) {
  long parsedValue;
  if (!parseStrictInteger(value, parsedValue) || parsedValue < 0 || parsedValue >= upperBound) {
    return false;
  }

  index = static_cast<int>(parsedValue);
  return true;
}

#endif