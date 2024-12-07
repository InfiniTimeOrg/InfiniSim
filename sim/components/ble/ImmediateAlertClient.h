#pragma once
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min
#include <cstdint>
#include "components/ble/BleClient.h"

namespace Pinetime {
  namespace System {
    class SystemTask;
  }

  namespace Controllers {
    class NotificationManager;

    class ImmediateAlertClient : public BleClient {
    public:
      enum class Levels : uint8_t { NoAlert = 0, MildAlert = 1, HighAlert = 2 };
      enum class State {
        NoConnection,
        NoIAS,
        Connected,
      };

      ImmediateAlertClient(Pinetime::System::SystemTask& systemTask);
      void Init();

      bool SendImmediateAlert(Levels level);

      State GetState() const {
        return State::NoConnection;
      }

      void Discover(uint16_t connectionHandle, std::function<void(uint16_t)> lambda) override;

    private:
      Pinetime::System::SystemTask& systemTask;

      static constexpr uint16_t immediateAlertClientId {0x1802};
      static constexpr uint16_t alertLevelId {0x2A06};

      static constexpr ble_uuid16_t immediateAlertClientUuid {.u {.type = BLE_UUID_TYPE_16}, .value = immediateAlertClientId};
      static constexpr ble_uuid16_t alertLevelCharacteristicUuid {.u {.type = BLE_UUID_TYPE_16}, .value = alertLevelId};

      std::function<void(uint16_t)> onServiceDiscovered;
    };
  }
}
