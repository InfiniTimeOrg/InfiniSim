#include "semphr.h"
#include <SDL2/SDL.h>
#include <stdexcept>

QueueHandle_t xSemaphoreCreateMutex() {
  SemaphoreHandle_t xSemaphore = xQueueCreate(1, 1);
  Queue_t *pxQueue = (Queue_t *)xSemaphore;
  // Queue full represents taken semaphore/locked mutex
  pxQueue->queue.push_back(0); 
  return xSemaphore;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,
                          TickType_t xTicksToWait) {
  Queue_t *pxQueue = (Queue_t *)xSemaphore;
  const int DELAY_BETWEEN_ATTEMPTS = 25;
  do {
    if (pxQueue->mutex.try_lock()) {
      std::lock_guard<std::mutex> lock(pxQueue->mutex, std::adopt_lock);
      if (pxQueue->queue.empty()) {
        pxQueue->queue.push_back(0);
        return true;
      }
    }
    // Prevent underflow
    if (xTicksToWait >= DELAY_BETWEEN_ATTEMPTS) {
      // Someone else is modifying queue, wait for them to finish
      SDL_Delay(DELAY_BETWEEN_ATTEMPTS);
      xTicksToWait -= DELAY_BETWEEN_ATTEMPTS;
    }
  } while (xTicksToWait >= DELAY_BETWEEN_ATTEMPTS);
  return false;
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
