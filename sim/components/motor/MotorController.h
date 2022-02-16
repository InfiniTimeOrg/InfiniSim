#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint8_t motorDuration);
      void StartRinging();
      void StopRinging();

      bool motor_is_running = false;
      bool is_ringing = false;

    private:
      static void Ring(void* p_context);
      static void StopMotor(void* p_context);
    };
  }
}
