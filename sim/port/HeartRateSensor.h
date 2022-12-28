#pragma once
#include "drivers/HeartRateSensor.h"
#include "sim/drivers/infinisim/Hrs3300.h"

namespace Pinetime {
  namespace Drivers {
  using HeartRateSensor = Interface::HeartRateSensor<Pinetime::Drivers::Infinisim::HeartRateSensors::Hrs3300>;
  }
}
