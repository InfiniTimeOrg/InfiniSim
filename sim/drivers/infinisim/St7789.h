#pragma once
#include "drivers/Spi.h"
#include <cstddef>
#include <cstdint>
#include "port/Spi.h"

namespace Pinetime {
  namespace Drivers {
    namespace Infinisim {
      namespace Displays {
        class St7789 {
        public:
          St7789() = default;
          St7789(const St7789 &) = delete;
          St7789 &operator=(const St7789 &) = delete;
          St7789(St7789 &&) = delete;
          St7789 &operator=(St7789 &&) = delete;

          void Init();
          void Uninit();
          void DrawPixel(uint16_t x, uint16_t y, uint32_t color);

          void VerticalScrollDefinition(uint16_t topFixedLines, uint16_t scrollLines,
                                        uint16_t bottomFixedLines);
          void VerticalScrollStartAddress(uint16_t line);

          void DrawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                          const uint8_t *data, size_t size);

          void Sleep();
          void Wakeup();
        };
      }
    }
  }
}
