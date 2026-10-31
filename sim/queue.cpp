#include "FreeRTOS.h"
#include "queue.h"
#include <chrono>
#include <stdexcept>
#include <SDL.h>
#include "task.h" // configTICK_RATE_HZ

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize) {
  Queue_t* xQueue = new Queue_t;
  if (uxItemSize != 1) {
    throw std::runtime_error("uxItemSize must be 1");
  }
  xQueue->queue.reserve(uxQueueLength);
  xQueue->max_length = uxQueueLength;
  return xQueue;
}

BaseType_t xQueueSend(QueueHandle_t xQueue, const void* const pvItemToQueue, TickType_t xTicksToWait) {
  Queue_t* pxQueue = (Queue_t*) xQueue;
  std::unique_lock<std::mutex> lock(pxQueue->mutex);
  auto timeout_ms = (static_cast<uint64_t>(xTicksToWait) * 1000) / configTICK_RATE_HZ;
  if (!pxQueue->cv.wait_for(
      lock, std::chrono::milliseconds(timeout_ms),
      [pxQueue]{ return pxQueue->queue.size() < pxQueue->max_length; })) {
    return false; // timeout
  }
  pxQueue->queue.push_back(*reinterpret_cast<const uint8_t* const>(pvItemToQueue));
  pxQueue->cv.notify_one();
  return true;
}

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void* const pvItemToQueue, BaseType_t* xHigherPriorityTaskWoken) {
  TickType_t xTicksToWait = 0;
  *xHigherPriorityTaskWoken = pdFALSE;
  return xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void* const pvBuffer, TickType_t xTicksToWait) {
  Queue_t* pxQueue = (Queue_t*) xQueue;
  std::unique_lock<std::mutex> lock(pxQueue->mutex);
  auto timeout_ms = (static_cast<uint64_t>(xTicksToWait) * 1000) / configTICK_RATE_HZ;
  if (!pxQueue->cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [pxQueue]{ return !pxQueue->queue.empty(); })) {
      return false; // timeout
  }
  uint8_t* buf = reinterpret_cast<uint8_t* const>(pvBuffer);
  *buf = pxQueue->queue.at(0);
  pxQueue->queue.erase(pxQueue->queue.begin());
  pxQueue->cv.notify_one();
  return true;
}

UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue) {
  UBaseType_t uxReturn;
  SDL_assert(xQueue);
  Queue_t* pxQueue = (Queue_t*) xQueue;
  // taskENTER_CRITICAL();
  {
    std::lock_guard<std::mutex> guard(pxQueue->mutex);
    uxReturn = static_cast<UBaseType_t>(pxQueue->queue.size());
  }
  // taskEXIT_CRITICAL();
  return uxReturn;
}
