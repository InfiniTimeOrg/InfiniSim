#pragma once
#include "drivers/Spi.h"
#include "drivers/SpiMaster.h"
#include "drivers/SpiNorFlash.h"
#include "drivers/TwiMaster.h"
#include <cstdint>


#include "sim/drivers/infinisim/SpiMaster.h"
#include "sim/drivers/infinisim/Spi.h"
#include "sim/drivers/infinisim/TwiMaster.h"
#include "sim/drivers/infinisim/SpiNorFlash.h"

namespace Pinetime {
  namespace Drivers {
  using SpiMaster = Interface::SpiMaster<Pinetime::Drivers::Infinisim::SpiMaster>;
  using Spi = Interface::Spi<Pinetime::Drivers::Infinisim::Spi>;
  using TwiMaster = Interface::TwiMaster<Pinetime::Drivers::Infinisim::TwiMaster>;
  using SpiNorFlash = Interface::SpiNorFlash<Pinetime::Drivers::Infinisim::SpiNorFlash>;
  }
}
