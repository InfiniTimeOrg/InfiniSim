#include "queue.h"
#include <stdexcept>
#include <SDL2/SDL.h>

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize)
{
  QueueHandle_t xQueue;
  if (uxItemSize != 1) {
    throw std::runtime_error("uxItemSize must be 1");
  }
  xQueue.queue.reserve(uxQueueLength);
  return xQueue;
}

BaseType_t xQueueSend(QueueHandle_t &xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait)
{
  std::lock_guard<std::mutex> guard(xQueue.mutex);
  xQueue.queue.push_back(*reinterpret_cast<const uint8_t *const>(pvItemToQueue));
  return true;
}

BaseType_t xQueueSendFromISR(QueueHandle_t &xQueue, const void * const pvItemToQueue, BaseType_t *xHigherPriorityTaskWoken)
{
  TickType_t xTicksToWait = 0;
  *xHigherPriorityTaskWoken = pdFALSE;
  return xQueueSend(xQueue, pvItemToQueue, 0.0);
}

BaseType_t xQueueReceive(QueueHandle_t &xQueue, void * const pvBuffer, TickType_t xTicksToWait)
{
  while (xQueue.queue.empty()) {
    if (xTicksToWait <= 25) {
      return false;
    }
    SDL_Delay(25);
    xTicksToWait -= 25;
  }
  if (xQueue.queue.empty()) {
    return false;
  }
  std::lock_guard<std::mutex> guard(xQueue.mutex);
  uint8_t *buf = reinterpret_cast<uint8_t * const>(pvBuffer);
  *buf = xQueue.queue.at(0);
  xQueue.queue.erase(xQueue.queue.begin());
  return true;
}
