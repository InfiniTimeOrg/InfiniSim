#pragma once

#include "portmacro_cmsis.h"

// normal version has pointer to register 'NRF_RTC_Type * p_reg', but we just simulate
// the return value according to known register pointer
uint32_t nrf_rtc_counter_get(NRF_RTC_Type p_reg);
