#pragma once
#include <cstddef>
#include <cstdint>

namespace Pinetime {
namespace Drivers {
namespace Infinisim {
class SpiMaster {
public:
  SpiMaster() = default;
  SpiMaster(const SpiMaster &) = delete;
  SpiMaster &operator=(const SpiMaster &) = delete;
  SpiMaster(SpiMaster &&) = delete;
  SpiMaster &operator=(SpiMaster &&) = delete;

  bool Init() {
    return true;
  }

  bool Write(uint8_t pinCsn, const uint8_t *data, size_t size) {
    return true;
  }

  bool Read(uint8_t pinCsn, uint8_t *cmd, size_t cmdSize, uint8_t *data,
            size_t dataSize) {
    return true;
  }

  bool WriteCmdAndBuffer(uint8_t pinCsn, const uint8_t *cmd, size_t cmdSize,
                         const uint8_t *data, size_t dataSize) {
    return true;
  }

  void OnStartedEvent() {}
  void OnEndEvent() {}

  void Sleep() {}
  void Wakeup() {}

private:

};
}
}
}
