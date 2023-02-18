#include "Watchdog.h"
using namespace Pinetime::Drivers::Infinisim::Watchdogs;

void Watchdog::Setup(uint8_t timeoutSeconds) {
  resetReason = ActualResetReason();
}

void Watchdog::Start() {
}

void Watchdog::Kick() {
}

Pinetime::Drivers::Watchdogs::ResetReasons Watchdog::ActualResetReason() const {
  return Pinetime::Drivers::Watchdogs::ResetReasons::ResetPin;
}
