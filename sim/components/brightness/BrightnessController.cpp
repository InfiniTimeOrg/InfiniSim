#include "BrightnessController.h"
// #include <hal/nrf_gpio.h>
#include "displayapp/screens/Symbols.h"
#include "drivers/PinMap.h"

#include <tuple> // std::ignore

using namespace Pinetime::Controllers;

void BrightnessController::Init() {
  Set(level);
}

void BrightnessController::ApplyBrightness(uint16_t rawBrightness) {
  std::ignore = rawBrightness;
}

void BrightnessController::Set(BrightnessController::Levels level) {
  this->level = level;
  switch (level) {
    default:
    case Levels::High:
      ApplyBrightness(3 * timerPeriod);
      break;
    case Levels::Medium:
      ApplyBrightness(2 * timerPeriod);
      break;
    case Levels::Low:
      ApplyBrightness(timerPeriod);
      break;
    case Levels::AlwaysOn:
      ApplyBrightness(timerPeriod / 10);
      break;
    case Levels::Off:
      ApplyBrightness(0);
      break;
  }
}

void BrightnessController::Lower() {
  switch (level) {
    case Levels::High:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::Low);
      break;
    case Levels::Low:
      Set(Levels::Off);
      break;
    default:
      break;
  }
}

void BrightnessController::Higher() {
  switch (level) {
    case Levels::Off:
      Set(Levels::Low);
      break;
    case Levels::Low:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::High);
      break;
    default:
      break;
  }
}

BrightnessController::Levels BrightnessController::Level() const {
  return level;
}

void BrightnessController::Step() {
  switch (level) {
    case Levels::Low:
      Set(Levels::Medium);
      break;
    case Levels::Medium:
      Set(Levels::High);
      break;
    case Levels::High:
      Set(Levels::Low);
      break;
    default:
      break;
  }
}
const char* BrightnessController::GetIcon() {
  switch (level) {
    case Levels::Medium:
      return Applications::Screens::Symbols::brightnessMedium;
    case Levels::High:
      return Applications::Screens::Symbols::brightnessHigh;
    default:
      break;
  }
  return Applications::Screens::Symbols::brightnessLow;
}

const char* BrightnessController::ToString() {
  switch (level) {
    case Levels::Off:
      return "Off";
    case Levels::Low:
      return "Low";
    case Levels::Medium:
      return "Medium";
    case Levels::High:
      return "High";
    default:
      return "???";
  }
}
