#pragma once
#include "drivers/TwiMaster.h"
#include <sim/drivers/infinisim/TwiMaster.h>

namespace Pinetime {
  namespace Drivers {
    using TwiMaster = Interface::TwiMaster<Pinetime::Drivers::Infinisim::TwiMaster>;
  }
}
