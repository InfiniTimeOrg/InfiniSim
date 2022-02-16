#include "components/motor/MotorController.h"

#include <SDL2/SDL.h>

using namespace Pinetime::Controllers;

void MotorController::Init() {
  //nrf_gpio_cfg_output(PinMap::Motor);
  //nrf_gpio_pin_set(PinMap::Motor);
  //app_timer_init();

  //app_timer_create(&shortVibTimer, APP_TIMER_MODE_SINGLE_SHOT, StopMotor);
  //app_timer_create(&longVibTimer, APP_TIMER_MODE_REPEATED, Ring);
}

void MotorController::Ring(void* p_context) {
  auto* motorController = static_cast<MotorController*>(p_context);
  motorController->RunForDuration(50);
}

Uint32 StopMotor_callback(Uint32 interval, void *param)
{
  auto* motorController = static_cast<MotorController*>(param);
  motorController->motor_is_running = false;
  return 0; // cancel timer
}
Uint32 Ring_callback(Uint32 interval, void *param)
{
  auto* motorController = static_cast<MotorController*>(param);
  motorController->RunForDuration(50);
  if (motorController->is_ringing) {
    return interval;
  }
  return 0;
}
void MotorController::RunForDuration(uint8_t motorDuration) {
  this->motor_is_running = true;
  SDL_AddTimer(motorDuration, StopMotor_callback, this);
  //nrf_gpio_pin_clear(PinMap::Motor);
  //app_timer_start(shortVibTimer, APP_TIMER_TICKS(motorDuration), nullptr);
}

void MotorController::StartRinging() {
  Ring(this);
  is_ringing = true;
  SDL_AddTimer(1000, Ring_callback, this);
  //app_timer_start(longVibTimer, APP_TIMER_TICKS(1000), this);
}

void MotorController::StopRinging() {
  is_ringing = false;
}

void MotorController::StopMotor(void* p_context) {
  //nrf_gpio_pin_set(PinMap::Motor);
  auto* motorController = static_cast<MotorController*>(p_context);
  motorController->motor_is_running = false;
}
