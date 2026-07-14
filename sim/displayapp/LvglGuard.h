#pragma once

// Shadows InfiniTime's no-op displayapp/LvglGuard.h (the sim include path wins).
//
// LVGL is not thread-safe, and unlike the watch the simulator has TWO threads
// driving it: the DisplayApp task, and the SDL main thread which renders the
// "Screen is OFF" overlay (and runs lv_task_handler while the display task
// sleeps). The wake/sleep handoff between them is a use-after-free factory —
// AddressSanitizer showed main-thread lv_obj_del(screen_off_bg) racing the
// display task's lv_task_handler. This mutex serializes both sides.
//
// The two sides lock differently on purpose:
//
//   LVGL_GUARD()      blocking. Used by the DisplayApp task, whose whole job is
//                     LVGL: if the main thread is mid-overlay it must wait.
//
//   LVGL_TRY_GUARD()  non-blocking. Used by the SDL main thread, which also
//                     pumps SDL events and the GATT bridge. It must never park
//                     on a lock held by another thread: doing so would let any
//                     stall inside the display task wedge the whole process
//                     (input dead, bridge unresponsive) instead of merely
//                     freezing the watch screen. Missing an overlay refresh is
//                     harmless — the main loop runs again in ~30 ms.

#include <mutex>

namespace Pinetime {
  namespace Sim {
    extern std::recursive_mutex lvglMutex;
  }
}

#define LVGL_GUARD() std::lock_guard<std::recursive_mutex> lvglGuardLock(Pinetime::Sim::lvglMutex)

// Skips the enclosing function when the display task holds the lock.
#define LVGL_TRY_GUARD()                                                                                                                   \
  std::unique_lock<std::recursive_mutex> lvglGuardLock(Pinetime::Sim::lvglMutex, std::try_to_lock);                                        \
  if (!lvglGuardLock.owns_lock()) {                                                                                                        \
    return;                                                                                                                                \
  }
