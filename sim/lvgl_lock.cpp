#include "lvgl_lock.h"

#include "lvgl/lvgl.h"

#include <mutex>

namespace {
  std::recursive_mutex lvglMutex;
}

extern "C" void sim_lvgl_lock(void) {
  lvglMutex.lock();
}

extern "C" void sim_lvgl_unlock(void) {
  lvglMutex.unlock();
}

// main.cpp and DisplayApp.cpp are compiled with
// -Dlv_task_handler=sim_lv_task_handler so that every LVGL task handler call
// is serialized with the simulator's own LVGL/SDL access.
extern "C" uint32_t sim_lv_task_handler(void) {
  std::lock_guard<std::recursive_mutex> lock(lvglMutex);
  return lv_task_handler();
}
