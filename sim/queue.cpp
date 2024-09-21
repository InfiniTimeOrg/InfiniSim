#include "FreeRTOS.h"
#include "queue.h"
#include <stdexcept>
#include <SDL2/SDL.h>

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize)
{
  Queue_t *xQueue = new Queue_t;
  if (uxItemSize != 1) {
    throw std::runtime_error("uxItemSize must be 1");
  }
  xQueue->queue.reserve(uxQueueLength);
  return xQueue;
}

BaseType_t xQueueSend(QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait)
{
  Queue_t* pxQueue = ( Queue_t * ) xQueue;
  std::lock_guard<std::mutex> guard(pxQueue->mutex);
  pxQueue->queue.push_back(*reinterpret_cast<const uint8_t *const>(pvItemToQueue));
  return true;
}

BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t *xHigherPriorityTaskWoken)
{
  TickType_t xTicksToWait = 0;
  *xHigherPriorityTaskWoken = pdFALSE;
  return xQueueSend(xQueue, pvItemToQueue, 0.0);
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait)
{
  Queue_t* pxQueue = ( Queue_t * ) xQueue;
  while (pxQueue->queue.empty()) {
    if (xTicksToWait <= 25) {
      return false;
    }
    SDL_Delay(25);
    xTicksToWait -= 25;
  }
  if (pxQueue->queue.empty()) {
    return false;
  }
  std::lock_guard<std::mutex> guard(pxQueue->mutex);
  uint8_t *buf = reinterpret_cast<uint8_t * const>(pvBuffer);
  *buf = pxQueue->queue.at(0);
  pxQueue->queue.erase(pxQueue->queue.begin());
  return true;
}

UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue)
{
  UBaseType_t uxReturn;
  SDL_assert(xQueue);
  Queue_t* pxQueue = ( Queue_t * ) xQueue;
  // taskENTER_CRITICAL();
  {
    std::lock_guard<std::mutex> guard(pxQueue->mutex);
    uxReturn = static_cast<UBaseType_t>(pxQueue->queue.size());
  }
  // taskEXIT_CRITICAL();
  return uxReturn;
}
