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

#include <SDL2/SDL.h>

/* Standard includes. */
#include <chrono>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
//#include "timers.h"
//#include "stack_macros.h"


TickType_t xTaskGetTickCount()
{
  static auto start = std::chrono::steady_clock::now();
  auto now = std::chrono::steady_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::duration<TickType_t, std::ratio<1, configTICK_RATE_HZ>>>(now - start);
  return diff.count();
}
/*-----------------------------------------------------------*/

UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime) {
  return 0;
}

void vTaskDelay( const TickType_t xTicksToDelay ) {
  SDL_Delay(xTicksToDelay);
}

int sdl_function_wrapper(void *instance)
{
  TaskHandle_t * task_handle = static_cast<TaskHandle_t*>(instance);
  task_handle->task_fn(task_handle->instance);
  return 0;
}

BaseType_t xTaskCreate(
  TaskFunction_t pxTaskCode,
  const char * const pcName,  /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
  const configSTACK_DEPTH_TYPE usStackDepth,
  void * const pvParameters,
  UBaseType_t uxPriority,
  TaskHandle_t * const pxCreatedTask )
{
  pxCreatedTask->task_fn = pxTaskCode;
  pxCreatedTask->instance = pvParameters;
  pxCreatedTask->thread_handle = SDL_CreateThread(sdl_function_wrapper, pcName, pxCreatedTask);
  return pxCreatedTask->thread_handle != nullptr;
}

BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify) {
  return pdPASS;
}
TaskHandle_t xTaskGetCurrentTaskHandle() {
  return {};
}
BaseType_t xTaskGetSchedulerState() {
  return taskSCHEDULER_NOT_STARTED;
}
