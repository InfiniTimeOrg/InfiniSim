#include "hal/nrf_rtc.h"

#include "task.h"
#include <chrono>
#include <stdexcept>

uint32_t nrf_rtc_counter_get(NRF_RTC_Type p_reg)
{
  if (p_reg == portNRF_RTC_REG) {
    TickType_t ticks_now = xTaskGetTickCount();
    return ticks_now;
  }
  throw std::runtime_error("nrf_rtc_counter_get: unhandled NRF_RTC_Type: " + std::to_string(p_reg));
  return 0;
}
