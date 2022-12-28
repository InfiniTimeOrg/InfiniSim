#pragma once
#include "drivers/SpiNorFlash.h"
#include "sim/drivers/infinisim/SpiNorFlash.h"

namespace Pinetime {
  namespace Drivers {
    using SpiNorFlash = Interface::SpiNorFlash<Pinetime::Drivers::Infinisim::SpiNorFlash>;
  }
}
