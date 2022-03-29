/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#pragma once

#include "portmacro_cmsis.h" // TickType_t
#include "task.h" // configTICK_RATE_HZ

#include <SDL2/SDL.h>

#include <chrono>
#include <cstdint>
#include <string>

class TimerHandle_t;
/*
 * Defines the prototype to which timer callback functions must conform.
 */
typedef void (*TimerCallbackFunction_t)( TimerHandle_t xTimer );

struct TimerHandle_t {
  bool running = false;
  bool auto_reload = false;
  SDL_TimerID timer_id = 0;
  TickType_t timer_period_in_ms;
  TickType_t expiry_time;
  std::string timer_name;
  void * pvTimerID;
  TimerCallbackFunction_t pxCallbackFunction;
};


constexpr uint32_t pdMS_TO_TICKS(uint32_t pdMS) {
  return pdMS * configTICK_RATE_HZ / 1000;
}
// function only available in Simulator
constexpr uint32_t pdTICKS_TO_MS(uint32_t ticks) {
  return ticks * 1000 / configTICK_RATE_HZ;
}

/**
 * void *pvTimerGetTimerID( TimerHandle_t xTimer );
 *
 * Returns the ID assigned to the timer.
 *
 * IDs are assigned to timers using the pvTimerID parameter of the call to
 * xTimerCreated() that was used to create the timer, and by calling the
 * vTimerSetTimerID() API function.
 *
 * If the same callback function is assigned to multiple timers then the timer
 * ID can be used as time specific (timer local) storage.
 *
 * @param xTimer The timer being queried.
 *
 * @return The ID assigned to the timer being queried.
 *
 * Example usage:
 *
 * See the xTimerCreate() API function example usage scenario.
 */
void *pvTimerGetTimerID(const TimerHandle_t &xTimer ); // return pvTimerID from xTimerCreate

/**
 * void vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID );
 *
 * Sets the ID assigned to the timer.
 *
 * IDs are assigned to timers using the pvTimerID parameter of the call to
 * xTimerCreated() that was used to create the timer.
 *
 * If the same callback function is assigned to multiple timers then the timer
 * ID can be used as time specific (timer local) storage.
 *
 * @param xTimer The timer being updated.
 *
 * @param pvNewID The ID to assign to the timer.
 *
 * Example usage:
 *
 * See the xTimerCreate() API function example usage scenario.
 */
void vTimerSetTimerID(TimerHandle_t &xTimer, void *pvNewID);


TimerHandle_t xTimerCreate(const char * const pcTimerName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
  const TickType_t xTimerPeriodInTicks,
  const UBaseType_t uxAutoReload, // false=one-shot, true=recurring
  void * const pvTimerID, // pointer passed to callback
  TimerCallbackFunction_t pxCallbackFunction);

/**
 * @param xTicksToWait Specifies the time, in ticks, that the calling task should
 * be held in the Blocked state to wait for the stop command to be successfully
 * sent to the timer command queue, should the queue already be full when
 * xTimerStop() was called.  xTicksToWait is ignored if xTimerStop() is called
 * before the scheduler is started.
 */
bool xTimerStart(TimerHandle_t &xTimer, TickType_t xTicksToWait);
/*
 * xTimerChangePeriod() changes the period of a timer that was previously
 * created using the xTimerCreate() API function.
 *
 * xTimerChangePeriod() can be called to change the period of an active or
 * dormant state timer.
 *
 * The configUSE_TIMERS configuration constant must be set to 1 for
 * xTimerChangePeriod() to be available.
 */
bool xTimerChangePeriod(TimerHandle_t &xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait);

/**
 * xTimerReset() re-starts a timer that was previously created using the
 * xTimerCreate() API function.  If the timer had already been started and was
 * already in the active state, then xTimerReset() will cause the timer to
 * re-evaluate its expiry time so that it is relative to when xTimerReset() was
 * called.  If the timer was in the dormant state then xTimerReset() has
 * equivalent functionality to the xTimerStart() API function.
 *
 * Resetting a timer ensures the timer is in the active state.  If the timer
 * is not stopped, deleted, or reset in the mean time, the callback function
 * associated with the timer will get called 'n' ticks after xTimerReset() was
 * called, where 'n' is the timers defined period.
 *
 * It is valid to call xTimerReset() before the scheduler has been started, but
 * when this is done the timer will not actually start until the scheduler is
 * started, and the timers expiry time will be relative to when the scheduler is
 * started, not relative to when xTimerReset() was called.
 */
bool xTimerReset(TimerHandle_t &xTimer,  TickType_t xTicksToWait);

bool xTimerStop(TimerHandle_t &xTimer,  TickType_t xTicksToWait);

/**
* TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer );
*
* Returns the time in ticks at which the timer will expire.  If this is less
* than the current tick count then the expiry time has overflowed from the
* current time.
*
* @param xTimer The handle of the timer being queried.
*
* @return If the timer is running then the time in ticks at which the timer
* will next expire is returned.  If the timer is not running then the return
* value is undefined.
*/
TickType_t xTimerGetExpiryTime( TimerHandle_t xTimer );

/**
 * BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer );
 *
 * Queries a timer to see if it is active or dormant.
 *
 * A timer will be dormant if:
 *     1) It has been created but not started, or
 *     2) It is an expired one-shot timer that has not been restarted.
 *
 * Timers are created in the dormant state.  The xTimerStart(), xTimerReset(),
 * xTimerStartFromISR(), xTimerResetFromISR(), xTimerChangePeriod() and
 * xTimerChangePeriodFromISR() API functions can all be used to transition a timer into the
 * active state.
 *
 * @param xTimer The timer being queried.
 *
 * @return pdFALSE will be returned if the timer is dormant.  A value other than
 * pdFALSE will be returned if the timer is active.
 *
 * Example usage:
 * @verbatim
 * // This function assumes xTimer has already been created.
 * void vAFunction( TimerHandle_t xTimer )
 * {
 *     if( xTimerIsTimerActive( xTimer ) != pdFALSE ) // or more simply and equivalently "if( xTimerIsTimerActive( xTimer ) )"
 *     {
 *         // xTimer is active, do something.
 *     }
 *     else
 *     {
 *         // xTimer is not active, do something else.
 *     }
 * }
 * @endverbatim
 */
BaseType_t xTimerIsTimerActive( TimerHandle_t xTimer );
