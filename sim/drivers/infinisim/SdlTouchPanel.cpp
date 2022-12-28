#include "SdlTouchPanel.h"
#include "lv_drv_conf.h" // MONITOR_ZOOM
#include "sim/libraries/log/nrf_log.h"
#include <SDL2/SDL.h>
#include <cmath>

using namespace Pinetime::Drivers::Infinisim::TouchPanels;

SdlTouchPanel::SdlTouchPanel() {
}

bool SdlTouchPanel::Init() {
  return true;
}

Pinetime::Drivers::TouchPanels::TouchInfos SdlTouchPanel::GetTouchInfo() {
  int x, y;
  uint32_t buttons = SDL_GetMouseState(&x, &y);
  // scale down real mouse coordinates to InfiniTime scale to make zoom work
  // the MONITOR_ZOOM-factor is defined in lv_drv_conf.h
  x /= MONITOR_ZOOM;
  y /= MONITOR_ZOOM;

  Pinetime::Drivers::TouchPanels::TouchInfos info;
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
          info.gesture = Pinetime::Drivers::TouchPanels::Gestures::LongPress;
        } else if(!is_stationary) {
          // moved mouse fast enough to not be a long-press
          is_swipe = true;
          double x_diff = static_cast<double>(info.x) - x_start;
          double y_diff = static_cast<double>(info.y) - y_start;
          if (fabs(x_diff) > fabs(y_diff)) {
            // x-swipe
            if (x_diff < 0) {
              info.gesture = Pinetime::Drivers::TouchPanels::Gestures::SlideLeft;
            } else {
              info.gesture = Pinetime::Drivers::TouchPanels::Gestures::SlideRight;
            }
          } else {
            // y-swipe
            if (y_diff < 0) {
              info.gesture = Pinetime::Drivers::TouchPanels::Gestures::SlideUp;
            } else {
              info.gesture = Pinetime::Drivers::TouchPanels::Gestures::SlideDown;
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
          info.gesture = Pinetime::Drivers::TouchPanels::Gestures::SingleTap;
        }
      }
    }
  }
  return info;
}

void SdlTouchPanel::Sleep() {
  NRF_LOG_INFO("[TOUCHPANEL] Sleep");
}

void SdlTouchPanel::Wakeup() {
  Init();
  NRF_LOG_INFO("[TOUCHPANEL] Wakeup");
}

bool SdlTouchPanel::CheckDeviceIds() {
  return chipId == 0xb4 && vendorId == 0 && fwVersion == 1;
}
