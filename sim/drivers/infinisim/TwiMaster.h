#pragma once
#include <cstdint>
#include <cstddef>

namespace Pinetime {
  namespace Drivers {
    namespace Infinisim {
      class TwiMaster {
      public:
        enum class ErrorCodes { NoError, TransactionFailed };

        TwiMaster();
        void Init();
        ErrorCodes Read(uint8_t deviceAddress, uint8_t registerAddress,
                        uint8_t *buffer, size_t size);
        ErrorCodes Write(uint8_t deviceAddress, uint8_t registerAddress,
                         const uint8_t *data, size_t size);

        void Sleep();
        void Wakeup();
      };
    }
  }
}
