#include "timers.h"
#include <stdexcept>

uint32_t timer_callback_wrapper(uint32_t interval, void *param) {
  TimerHandle_t *xTimer = static_cast<TimerHandle_t*>(param);
  if (!xTimer->running) {
    return 0;
  }
  xTimer->pxCallbackFunction(*xTimer);
  if (xTimer->auto_reload) {
    return xTimer->timer_period_in_ms;
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
  xTimer.timer_period_in_ms = pdTICKS_TO_MS(xTimerPeriodInTicks);
  xTimer.auto_reload = uxAutoReload == pdTRUE;
  xTimer.timer_name = pcTimerName;
  xTimer.pvTimerID = pvTimerID;
  xTimer.pxCallbackFunction = pxCallbackFunction;
  return xTimer;
}

bool xTimerStart(TimerHandle_t &xTimer, TickType_t xTicksToWait) {
  xTimer.running = true;
  xTimer.expiry_time = xTaskGetTickCount() + pdMS_TO_TICKS(xTimer.timer_period_in_ms);
  xTimer.timer_id = SDL_AddTimer(xTimer.timer_period_in_ms, timer_callback_wrapper, &xTimer);
  if (xTimer.pxCallbackFunction == nullptr) {
    throw std::runtime_error("xTimerStart called before xTimerCreate");
  }
  return xTimer.timer_id != 0;
}

bool xTimerChangePeriod(TimerHandle_t &xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait) {
  if (xTimer.running) {
    xTimerStop(xTimer, xTicksToWait);
    xTimer.timer_period_in_ms = pdTICKS_TO_MS(xNewPeriod);
    xTimerStart(xTimer, xTicksToWait);
  } else {
    xTimer.timer_period_in_ms = pdTICKS_TO_MS(xNewPeriod);
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

TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer )
{
  return xTimer.expiry_time;
}

BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer ) {
  return xTimer.running;
}
