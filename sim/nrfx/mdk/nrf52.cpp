#include "mdk/nrf52.h"

// pointer variable used by the rest of the code, and its initialization
NRF_WDT_Type *NRF_WDT;
void init_NRF_WDT()
{
  static NRF_WDT_Type NRF_WDT_object;
  NRF_WDT = &NRF_WDT_object;
}

// pointer variable used by the rest of the code, and its initialization
NRF_POWER_Type *NRF_POWER;
void init_NRF_POWER()
{
  static NRF_POWER_Type NRF_POWER_object;
  NRF_POWER = &NRF_POWER_object;
  // sim: always return ResetPin as reason for reboot
  NRF_POWER->RESETREAS = 0x01;
}
