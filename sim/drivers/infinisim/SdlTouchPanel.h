#pragma once

#include "drivers/TouchPanel.h"
#include <chrono>
#include <cstdint>

namespace Pinetime {
  namespace Drivers {
    namespace Infinisim {
      namespace TouchPanels {
        class SdlTouchPanel {
        public:
          SdlTouchPanel();
          SdlTouchPanel(const SdlTouchPanel &) = delete;
          SdlTouchPanel &operator=(const SdlTouchPanel &) = delete;
          SdlTouchPanel(SdlTouchPanel &&) = delete;
          SdlTouchPanel &operator=(SdlTouchPanel &&) = delete;

          bool Init();
          Pinetime::Drivers::TouchPanels::TouchInfos GetTouchInfo();
          void Sleep();
          void Wakeup();

          uint8_t GetChipId() const { return chipId; }
          uint8_t GetVendorId() const { return vendorId; }
          uint8_t GetFwVersion() const { return fwVersion; }

        private:
          bool CheckDeviceIds();

          static constexpr uint8_t maxX = 240;
          static constexpr uint8_t maxY = 240;

          const uint8_t chipId = 0xb4;
          const uint8_t vendorId = 0;
          const uint8_t fwVersion = 1;

          // simulation members for swipe detection from mouse
          std::chrono::time_point<std::chrono::steady_clock> pressed_since;
          bool is_pressed = false;
          bool is_long_press = false;
          bool is_stationary = true;
          bool is_swipe = false;
          uint8_t x_start;
          uint8_t y_start;
        };
      }
    }
  }
}
