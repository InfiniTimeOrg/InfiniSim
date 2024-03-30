#include "semphr.h"
#include <SDL2/SDL.h>
#include <stdexcept>

QueueHandle_t xSemaphoreCreateMutex() {
  SemaphoreHandle_t xSemaphore = xQueueCreate(1, 1);
  Queue_t *pxQueue = (Queue_t *)xSemaphore;
  pxQueue->queue.push_back(0);
  return xSemaphore;
};

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,
                          TickType_t xTicksToWait) {
  Queue_t *pxQueue = (Queue_t *)xSemaphore;
  while (!pxQueue->queue.empty()) {
    if (xTicksToWait <= 25) {
      return false;
    }
    SDL_Delay(25);
    xTicksToWait -= 25;
  }
  std::lock_guard<std::mutex> guard(pxQueue->mutex);
  if (!pxQueue->queue.empty()) {
    return false;
  }
  pxQueue->queue.push_back(0);
  return true;
}
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
  Queue_t *pxQueue = (Queue_t *)xSemaphore;
  std::lock_guard<std::mutex> guard(pxQueue->mutex);
  if (pxQueue->queue.size() != 1) {
    throw std::runtime_error("Mutex released without being held");
  }
  pxQueue->queue.pop_back();
  return true;
}
