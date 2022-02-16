#include "timers.h"
#include <stdexcept>

uint32_t timer_callback_wrapper(uint32_t interval, void *param) {
  TimerHandle_t *xTimer = static_cast<TimerHandle_t*>(param);
  if (!xTimer->running) {
    return 0;
  }
  xTimer->pxCallbackFunction(*xTimer);
  if (xTimer->auto_reload) {
    return xTimer->xTimerPeriodInTicks;
  }
  xTimer->running = false;
  return 0; // cancel timer
}

void *pvTimerGetTimerID(const TimerHandle_t &xTimer) { // return pvTimerID from xTimerCreate
  return xTimer.pvTimerID;
}
void vTimerSetTimerID(TimerHandle_t &xTimer, void *pvNewID) {
  xTimer.pvTimerID = pvNewID;
}

TimerHandle_t xTimerCreate(const char * const pcTimerName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
  const TickType_t xTimerPeriodInTicks,
  const UBaseType_t uxAutoReload, // false=one-shot, true=recurring
  void * const pvTimerID, // pointer passed to callback
  TimerCallbackFunction_t pxCallbackFunction)
{
  TimerHandle_t xTimer;
  xTimer.xTimerPeriodInTicks = xTimerPeriodInTicks;
  xTimer.auto_reload = uxAutoReload == pdTRUE;
  xTimer.timer_name = pcTimerName;
  xTimer.pvTimerID = pvTimerID;
  xTimer.pxCallbackFunction = pxCallbackFunction;
  return xTimer;
}

bool xTimerStart(TimerHandle_t &xTimer, TickType_t xTicksToWait) {
  xTimer.running = true;
  xTimer.timer_id = SDL_AddTimer(xTimer.xTimerPeriodInTicks, timer_callback_wrapper, &xTimer);
  if (xTimer.pxCallbackFunction == nullptr) {
    throw std::runtime_error("xTimerStart called before xTimerCreate");
  }
  return xTimer.timer_id != 0;
}

bool xTimerChangePeriod(TimerHandle_t &xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait) {
  if (xTimer.running) {
    xTimerStop(xTimer, xTicksToWait);
    xTimer.xTimerPeriodInTicks = xNewPeriod;
    xTimerStart(xTimer, xTicksToWait);
  } else {
    xTimer.xTimerPeriodInTicks = xNewPeriod;
  }
  return true;
}

bool xTimerReset(TimerHandle_t &xTimer,  TickType_t xTicksToWait) {
  if (xTimer.running) {
    xTimerStop(xTimer, xTicksToWait);
  }
  return xTimerStart(xTimer, xTicksToWait);
}

bool xTimerStop(TimerHandle_t &xTimer,  TickType_t xTicksToWait) {
  xTimer.running = false;
  return SDL_RemoveTimer(xTimer.timer_id);
}
