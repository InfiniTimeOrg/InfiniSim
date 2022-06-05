#include "hal/nrf_gpio.h"

#include "drivers/PinMap.h"

#include <SDL2/SDL.h>
#include <stdexcept>
#include <string> // std::to_string

bool motor_running = false;

void nrf_gpio_cfg_default(uint32_t pin_number) {
  if (pin_number == Pinetime::PinMap::Motor)
  {
    motor_running = true;
  }
}
void nrf_gpio_pin_set(uint32_t pin_number) {
  if (pin_number == Pinetime::PinMap::Motor)
  {
    motor_running = false;
  }
}
uint32_t nrf_gpio_pin_read(uint32_t pin_number)
{
  if (pin_number == Pinetime::PinMap::Button) {
    int x, y;
    uint32_t buttons = SDL_GetMouseState(&x, &y);
    bool right_click = (buttons & SDL_BUTTON_RMASK) != 0;
    return right_click;
  }
  else if (pin_number == Pinetime::PinMap::Motor)
  {
    return motor_running;
  }
  throw std::runtime_error("nrf_gpio_pin_read: unhandled pin_number: " + std::to_string(pin_number));
  return 0;
}

void nrf_gpio_cfg_output(uint32_t pin_number) {}
void nrf_gpio_pin_clear(uint32_t pin_number) {
  if (pin_number == Pinetime::PinMap::Motor)
  {
    motor_running = true;
  }
}
void nrf_gpio_range_cfg_input(uint32_t            pin_range_start,
                              uint32_t            pin_range_end,
                              nrf_gpio_pin_pull_t pull_config) {}
void nrf_gpio_cfg_input(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config) {}

void nrfx_gpiote_in_init(uint32_t pin_number, nrfx_gpiote_in_config_t *config, nrfx_gpiote_event_handler_t) {}
void nrfx_gpiote_in_event_enable(uint32_t pin_number, bool enable) {}
void nrf_gpio_cfg_sense_input(uint32_t pin_number, nrf_gpio_pin_pull_t pin_pull, nrf_gpio_pin_sense_t sense) {}

void APP_GPIOTE_INIT(uint32_t max_users) {}
