#include "drivers/Watchdog.h"
using namespace Pinetime::Drivers;

void Watchdog::Setup(uint8_t timeoutSeconds) {
  resetReason = ActualResetReason();
}

void Watchdog::Start() {
}

void Watchdog::Kick() {
}

Watchdog::ResetReasons Watchdog::ActualResetReason() const {
  return ResetReasons::ResetPin;
}

const char* Watchdog::ResetReasonToString(Watchdog::ResetReasons reason) {
  switch (reason) {
    case ResetReasons::ResetPin:
      return "Reset pin";
    case ResetReasons::Watchdog:
      return "Watchdog";
    case ResetReasons::DebugInterface:
      return "Debug interface";
    case ResetReasons::LpComp:
      return "LPCOMP";
    case ResetReasons::SystemOff:
      return "System OFF";
    case ResetReasons::CpuLockup:
      return "CPU Lock-up";
    case ResetReasons::SoftReset:
      return "Soft reset";
    case ResetReasons::NFC:
      return "NFC";
    case ResetReasons::HardReset:
      return "Hard reset";
    default:
      return "Unknown";
  }
}
