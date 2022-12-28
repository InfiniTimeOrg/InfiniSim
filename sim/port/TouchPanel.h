#pragma once
#include "drivers/TouchPanel.h"
#include "sim/drivers/infinisim/SdlTouchPanel.h"

namespace Pinetime {
  namespace Drivers {
  using TouchPanel = Interface::Touchpanel<Pinetime::Drivers::Infinisim::TouchPanels::SdlTouchPanel>;
  }
}
