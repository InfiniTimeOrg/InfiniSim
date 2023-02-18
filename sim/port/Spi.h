#pragma once
#include "drivers/Spi.h"
#include "sim/drivers/infinisim/Spi.h"

namespace Pinetime {
  namespace Drivers {
    using Spi = Interface::Spi<Pinetime::Drivers::Infinisim::Spi>;
  }
}
