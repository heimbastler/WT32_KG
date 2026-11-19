#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
source="$root/src/main.cpp"
tmp=$(mktemp -d)
trap 'rm -f -- "$tmp/main.cpp" "$tmp/check"; rmdir "$tmp"' EXIT

extract_function() {
  awk -v signature="$1" '
    index($0, signature) == 1 { printing = 1 }
    printing { print }
    printing && /^}/ { exit }
  ' "$source"
}

cat > "$tmp/main.cpp" <<'CPP'
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define LOW 0
#define HIGH 1

unsigned long clockNow = 100;
unsigned long millis() { return clockNow; }
uint8_t inputState[16];
int egToggles = 0, kgToggles = 0, kitchenToggles = 0;
int publishes[16] = {};
void toggleEGFlurlampe() { ++egToggles; }
void toggleKGFlurlampe() { ++kgToggles; }
void toggleKuechenarbeitslampe() { ++kitchenToggles; }
void publishInputState(int pin) { ++publishes[pin]; }

struct FakeSerial {
  void println(const char *) {}
  template <typename... Args>
  void printf(const char *, Args...) {}
} Serial;

struct FakeMCP {
  uint16_t gpio = 0xffff;
  int reads = 0;
  uint16_t readGPIOAB() { ++reads; return gpio; }
} mcpIn;

void handleIRSwitchKitchen();
void handleKreuzschaltungEG();
void handleKreuzschaltungKG();
CPP

awk '
  /^#define (IR_SWITCH_KITCHEN_|KREUZ_)/ { print }
  /^bool mcpInputReady =/ { print }
  /^volatile bool mcpInterruptFlag =/ { print }
  /^\/\/ ---------- IR-Switch Küche Zustand/ { printing = 1 }
  /^\/\/ ---------- Temperatursensor Zustand/ { printing = 0 }
  printing { print }
' "$source" >> "$tmp/main.cpp"

for function in synchronizeMCPInputHandlers processMCPInputs handleIRSwitchKitchen handleKreuzschaltungEG handleKreuzschaltungKG; do
  extract_function "void $function() {" >> "$tmp/main.cpp"
done

cat >> "$tmp/main.cpp" <<'CPP'
void tick(unsigned long time) {
  clockNow = time;
  processMCPInputs();
}

void assertQuiet() {
  assert(egToggles == 0 && kgToggles == 0 && kitchenToggles == 0);
}

void initialize(uint16_t gpio = 0xffff) {
  mcpInputReady = true;
  mcpIn.gpio = gpio;
  bootTime = clockNow;
  for (int i = 0; i < 16; ++i) inputState[i] = (gpio >> i) & 1;
  synchronizeMCPInputHandlers();
  processMCPInputs();
}

void arm(uint16_t gpio = 0xffff) {
  initialize(gpio);
  tick(bootTime + bootDelay);
  assertQuiet();
  tick(bootTime + bootDelay + 1);
  assertQuiet();
}

void assertToggles(int pin, int expected) {
  assert(egToggles == (pin == 2 || pin == 3 ? expected : 0));
  assert(kgToggles == (pin >= 4 && pin <= 6 ? expected : 0));
  assert(kitchenToggles == 0);
}

int main(int argc, char **argv) {
  assert(argc >= 2);
  assert(mcpPollInterval == 20 && kreuzDebounceInterval == 60);
  int pin = argc > 2 ? std::atoi(argv[2]) : 2;
  if (!std::strcmp(argv[1], "switch")) {
    arm();
    unsigned long start = clockNow;
    mcpIn.gpio &= ~(1u << pin);
    mcpInterruptFlag = true;
    tick(start + 1);
    tick(start + 60);
    assertQuiet();
    tick(start + 61);
    assertToggles(pin, 1);
    tick(start + 261);
    assertToggles(pin, 1);
    mcpIn.gpio |= 1u << pin;
    mcpInterruptFlag = true;
    tick(start + 262);
    tick(start + 321);
    assertToggles(pin, 1);
    tick(start + 322);
    tick(start + 522);
    assertToggles(pin, 2);
    assert(publishes[pin] == 2);
  } else if (!std::strcmp(argv[1], "noise")) {
    arm();
    unsigned long start = clockNow;
    for (unsigned long offset = 1; offset <= 91; offset += 10) {
      mcpIn.gpio ^= 1u << pin;
      mcpInterruptFlag = true;
      tick(start + offset);
      assertQuiet();
    }
    mcpIn.gpio &= ~(1u << pin);
    mcpInterruptFlag = true;
    tick(start + 101);
    tick(start + 160);
    assertQuiet();
    tick(start + 161);
    assertToggles(pin, 1);
  } else if (!std::strcmp(argv[1], "poll")) {
    arm();
    unsigned long start = clockNow;
    int reads = mcpIn.reads;
    mcpIn.gpio ^= 1u << pin;
    tick(start + 19);
    assert(mcpIn.reads == reads && inputState[pin] == 1);
    tick(start + 20);
    assert(mcpIn.reads == reads + 1 && inputState[pin] == 0);
    assert(publishes[pin] == 1);
    tick(start + 79);
    assertQuiet();
    tick(start + 80);
    assertToggles(pin, pin >= 2 && pin <= 6 ? 1 : 0);
    tick(start + 100);
    assert(publishes[pin] == 1);
  } else if (!std::strcmp(argv[1], "irq")) {
    arm();
    unsigned long start = clockNow;
    int reads = mcpIn.reads;
    mcpIn.gpio &= ~(1u << 2);
    mcpInterruptFlag = true;
    tick(start + 1);
    assert(mcpIn.reads == reads + 1 && !mcpInterruptFlag);
    // No new interrupt: confirmation must still run before the next poll.
    tick(start + 59);
    assertQuiet();
    tick(start + 61);
    assert(egToggles == 1 && mcpIn.reads == reads + 2);
  } else if (!std::strcmp(argv[1], "recovery")) {
    arm();
    unsigned long start = clockNow;
    mcpIn.gpio &= ~(1u << 2);
    mcpInterruptFlag = true;
    tick(start + 1);
    mcpIn.gpio |= 1u << 2;  // Return edge arrives without an interrupt.
    tick(start + 21);
    tick(start + 100);
    assertQuiet();
    assert(inputState[2] == 1 && publishes[2] == 2);
  } else if (!std::strcmp(argv[1], "independent")) {
    arm();
    unsigned long start = clockNow;
    mcpIn.gpio &= ~(1u << 2);
    mcpInterruptFlag = true;
    tick(start + 1);
    mcpIn.gpio &= ~(1u << 4);
    mcpInterruptFlag = true;
    tick(start + 21);
    tick(start + 61);
    assert(egToggles == 1 && kgToggles == 0);
    tick(start + 81);
    assert(egToggles == 1 && kgToggles == 1);
  } else if (!std::strcmp(argv[1], "boot")) {
    initialize();
    unsigned long start = bootTime;
    mcpIn.gpio = static_cast<uint16_t>(0xff80 | pin);
    mcpInterruptFlag = true;
    tick(start + 1000);
    tick(start + bootDelay);
    tick(start + bootDelay + 1);
    tick(start + bootDelay + 100);
    assertQuiet();
    assert(kreuzstateEG == ((pin >> 2) & 3));
    assert(kreuzstateKG == ((pin >> 4) & 7));
    // Both groups must remain functional after boot synchronization.
    mcpIn.gpio ^= (1u << 2) | (1u << 4);
    mcpInterruptFlag = true;
    tick(start + bootDelay + 101);
    tick(start + bootDelay + 161);
    assert(egToggles == 1 && kgToggles == 1 && kitchenToggles == 0);
  } else if (!std::strcmp(argv[1], "late-start")) {
    initialize();
    clockNow = bootTime + bootDelay + 1000;
    mcpIn.gpio = 0;
    processMCPInputs();
    tick(clockNow + 100);
    assertQuiet();
    assert(kreuzstateEG == 0 && kreuzstateKG == 0);
  } else if (!std::strcmp(argv[1], "boot-boundary")) {
    initialize();
    tick(bootTime + bootDelay);
    int reads = mcpIn.reads;
    mcpIn.gpio = 0;  // No IRQ, last poll only one millisecond ago.
    tick(bootTime + bootDelay + 1);
    assert(mcpIn.reads == reads + 1);
    tick(clockNow + 100);
    assertQuiet();
    assert(kreuzstateEG == 0 && kreuzstateKG == 0);
  } else if (!std::strcmp(argv[1], "kitchen")) {
    arm();
    unsigned long start = clockNow;
    mcpIn.gpio &= ~(1u << pin);
    mcpInterruptFlag = true;
    tick(start + 1);
    tick(start + 51);
    assertQuiet();
    mcpIn.gpio |= 1u << pin;
    mcpInterruptFlag = true;
    tick(start + 61);
    tick(start + 110);
    assertQuiet();
    tick(start + 111);
    assert(kitchenToggles == 1 && egToggles == 0 && kgToggles == 0);
    tick(start + 500);
    assert(kitchenToggles == 1);
  } else if (!std::strcmp(argv[1], "wrap")) {
    clockNow = ULONG_MAX - 10000;
    arm();
    unsigned long start = ULONG_MAX - 30;
    tick(start);
    mcpIn.gpio &= ~(1u << 2);
    mcpInterruptFlag = true;
    tick(start + 1);
    tick(start + 60);
    assertQuiet();
    tick(start + 61);
    assert(egToggles == 1 && inputState[2] == 0);
  } else if (!std::strcmp(argv[1], "uptime-wrap")) {
    arm();
    unsigned long start = bootTime - 30;
    tick(start);
    mcpIn.gpio &= ~(1u << 2);
    mcpInterruptFlag = true;
    tick(start + 1);
    tick(start + 60);
    assertQuiet();
    tick(start + 61);
    assert(egToggles == 1);
  } else if (!std::strcmp(argv[1], "missing")) {
    tick(10000);
    assert(mcpIn.reads == 0);
    assertQuiet();
  } else {
    assert(false && "Unknown test scenario");
  }
}
CPP

c++ -std=c++11 -Wall -Wextra -Werror "$tmp/main.cpp" -o "$tmp/check"
for scenario in irq recovery independent late-start boot-boundary wrap uptime-wrap missing; do
  "$tmp/check" "$scenario"
done
for pin in 2 3 4 5 6; do
  "$tmp/check" switch "$pin"
  "$tmp/check" noise "$pin"
done
for pin in 0 1; do
  "$tmp/check" kitchen "$pin"
done
pin=0
while [ "$pin" -lt 16 ]; do
  "$tmp/check" poll "$pin"
  pin=$((pin + 1))
done
state=0
while [ "$state" -lt 128 ]; do
  "$tmp/check" boot "$state"
  state=$((state + 1))
done
printf 'MCP input regression passed: 164 scenarios, 20 ms polling, 60 ms debounce, boot sync, IRQ recovery, kitchen and timer wrap.\n'
