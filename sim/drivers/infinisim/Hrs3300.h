#pragma once

#include "port/TwiMaster.h"

namespace Pinetime {
  namespace Drivers {
    namespace Infinisim {
      namespace HeartRateSensors {
        class Hrs3300 {
        public:
          Hrs3300() = default;
          Hrs3300(const Hrs3300 &) = delete;
          Hrs3300 &operator=(const Hrs3300 &) = delete;
          Hrs3300(Hrs3300 &&) = delete;
          Hrs3300 &operator=(Hrs3300 &&) = delete;

          void Init();
          void Enable();
          void Disable();
          uint32_t ReadHrs();
          uint32_t ReadAls();
          void SetGain(uint8_t gain);
          void SetDrive(uint8_t drive);
        };
      }
    }
  }
}
