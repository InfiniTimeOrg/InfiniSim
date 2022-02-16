#include "drivers/Cst816s.h"
#include <SDL2/SDL.h>
#include <libraries/log/nrf_log.h>
#include <cmath>

using namespace Pinetime::Drivers;

/* References :
 * This implementation is based on this article :
 * https://medium.com/@ly.lee/building-a-rust-driver-for-pinetimes-touch-controller-cbc1a5d5d3e9 Touch panel datasheet (weird chinese
 * translation) : https://wiki.pine64.org/images/5/51/CST816S%E6%95%B0%E6%8D%AE%E6%89%8B%E5%86%8CV1.1.en.pdf
 *
 * TODO : we need a complete datasheet and protocol reference!
 * */

//Cst816S::Cst816S(TwiMaster& twiMaster, uint8_t twiAddress) : twiMaster {twiMaster}, twiAddress {twiAddress} {
//}
Cst816S::Cst816S() {
}

bool Cst816S::Init() {
  return true;
}

Cst816S::TouchInfos Cst816S::GetTouchInfo() {
  int x, y;
  uint32_t buttons = SDL_GetMouseState(&x, &y);

  Cst816S::TouchInfos info;
  info.x = x;
  info.y = y;
  info.touching = (buttons & SDL_BUTTON_LMASK) != 0;
  //info.gesture = gesture;
  info.isValid = x > 0 && x <= maxX && y > 0 && y <= maxY;
  if(info.isValid) {
    if(!is_pressed && info.touching) {
      // start klick
      pressed_since = std::chrono::steady_clock::now();
      is_pressed = true;
      is_long_press = false;
      is_swipe = false;
      is_stationary = true;
      x_start = info.x;
      y_start = info.y;
    } else if(is_pressed && info.touching) {
      // is it long press?
      if (is_stationary) { // check if while touching we moved away from the start coordinates
        double x_diff = static_cast<double>(info.x) - x_start;
        double y_diff = static_cast<double>(info.y) - y_start;
        double norm = hypot(x_diff, y_diff);
        if(norm > 20) { // we moved out of start area
          is_stationary = false;
        }
      }
      if (!is_long_press && !is_swipe) { // check for long-press only if it's not yet a long-press and didn't move
        std::chrono::duration<double> press_duration = std::chrono::steady_clock::now() - pressed_since;
        if(is_stationary && press_duration.count() > 1.0) {
          // longer than 1 second pressed, then it is long-press
          is_long_press = true;
          info.gesture = Gestures::LongPress;
        } else if(!is_stationary) {
          // moved mouse fast enough to not be a long-press
          is_swipe = true;
          double x_diff = static_cast<double>(info.x) - x_start;
          double y_diff = static_cast<double>(info.y) - y_start;
          if (fabs(x_diff) > fabs(y_diff)) {
            // x-swipe
            if (x_diff < 0) {
              info.gesture = Gestures::SlideLeft;
            } else {
              info.gesture = Gestures::SlideRight;
            }
          } else {
            // y-swipe
            if (y_diff < 0) {
              info.gesture = Gestures::SlideUp;
            } else {
              info.gesture = Gestures::SlideDown;
            }
          }
        }
      }
    } else if(is_pressed && !info.touching) {
      // end klick
      is_pressed = false;

      double x_diff = static_cast<double>(info.x) - x_start;
      double y_diff = static_cast<double>(info.y) - y_start;
      double norm = hypot(x_diff, y_diff);
      if(norm < 20) {
        if(is_stationary && !is_long_press && !is_swipe) {
          // no swipe with less than 5 pixel mouse movement
          info.gesture = Gestures::SingleTap;
        }
      }
    }
  }
  return info;
}

void Cst816S::Sleep() {
  NRF_LOG_INFO("[TOUCHPANEL] Sleep");
}

void Cst816S::Wakeup() {
  Init();
  NRF_LOG_INFO("[TOUCHPANEL] Wakeup");
}

bool Cst816S::CheckDeviceIds() {
  return chipId == 0xb4 && vendorId == 0 && fwVersion == 1;
}
