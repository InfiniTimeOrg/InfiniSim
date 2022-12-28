#pragma once
#include "drivers/MotionSensor.h"
#include "sim/drivers/infinisim/Bma421.h"

namespace Pinetime {
  namespace Drivers {
  using MotionSensor = Interface::MotionSensor<Pinetime::Drivers::Infinisim::MotionSensors::Bma421>;
  }
}
