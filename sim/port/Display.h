#pragma once
#include "drivers/Display.h"
#include "sim/drivers/infinisim/St7789.h"

namespace Pinetime {
  namespace Drivers {
  using Display = Interface::Display<Pinetime::Drivers::Infinisim::Displays::St7789>;
  }
}
