#pragma once
#include <FreeRTOS.h>
// #include <task.h>
#include <queue.h>

// #include <components/heartrate/Ppg.h>

namespace Pinetime {
  namespace Drivers {
    class Hrs3300;
  }

  namespace Controllers {
    class HeartRateController;
  }

  namespace Applications {
    class HeartRateTask {
    public:
      enum class Messages : uint8_t { GoToSleep, WakeUp, Enable, Disable };

      explicit HeartRateTask(Drivers::Hrs3300& heartRateSensor, Controllers::HeartRateController& controller);
      void Start();
      void Work();
      void PushMessage(Messages msg);

    private:
      enum class States : uint8_t { Disabled, Waiting, BackgroundMeasuring, ForegroundMeasuring };
      // static void Process(void* instance);
      // void StartMeasurement();
      // void StopMeasurement();

      //      TaskHandle_t taskHandle;
      QueueHandle_t messageQueue;
      States state = States::Disabled;
      Drivers::Hrs3300& heartRateSensor;
      Controllers::HeartRateController& controller;
      //      Controllers::Ppg ppg;
      bool measurementStarted = false;
    };

  }
}
