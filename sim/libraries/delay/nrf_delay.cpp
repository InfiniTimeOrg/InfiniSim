#include "libraries/delay/nrf_delay.h"
#include <SDL2/SDL.h>

void nrf_delay_ms(uint32_t ms_time)
{
  SDL_Delay(ms_time);
}