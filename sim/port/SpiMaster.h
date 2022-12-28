#pragma once
#include "drivers/SpiMaster.h"
#include "sim/drivers/infinisim/SpiMaster.h"

namespace Pinetime {
  namespace Drivers {
    using SpiMaster = Interface::SpiMaster<Pinetime::Drivers::Infinisim::SpiMaster>;
  }
}
