#pragma once
#include "drivers/Watchdog.h"
#include "sim/drivers/infinisim/Watchdog.h"

namespace Pinetime {
  namespace Drivers {
  using Watchdog = Interface::Watchdog<Pinetime::Drivers::Infinisim::Watchdogs::Watchdog>;
  }
}
