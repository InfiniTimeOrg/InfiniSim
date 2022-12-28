#pragma once
#include <cstdint>
#include "drivers/Watchdog.h"

namespace Pinetime {
  namespace Drivers {
    namespace Infinisim {
      namespace Watchdogs {
        class Watchdog {
        public:
          void Setup(uint8_t timeoutSeconds);
          void Start();
          void Kick();
          Pinetime::Drivers::Watchdogs::ResetReasons ResetReason() const { return resetReason; }

        private:
          Pinetime::Drivers::Watchdogs::ResetReasons resetReason;
          Pinetime::Drivers::Watchdogs::ResetReasons ActualResetReason() const;
        };
      }
    }
  }
}
