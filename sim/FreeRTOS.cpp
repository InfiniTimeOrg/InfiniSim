#include "FreeRTOS.h"
#include <numeric>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>

void NVIC_SystemReset(void) {}

void APP_ERROR_HANDLER(int err) {
  fprintf(stderr, "APP_ERROR_HANDLER: %d", err);
}

namespace {
std::unordered_map<void *, size_t> allocatedMemory;
size_t currentFreeHeap = configTOTAL_HEAP_SIZE;
size_t minimumEverFreeHeap = configTOTAL_HEAP_SIZE;
}

void *pvPortMalloc( size_t xWantedSize ) {
  void* ptr = malloc(xWantedSize);
  allocatedMemory[ptr] = xWantedSize;

  const size_t currentSize = std::accumulate(
    allocatedMemory.begin(), allocatedMemory.end(), 0,
    [](const size_t lhs, const std::pair<void*, size_t>& item){
      return lhs + item.second;
  });

  currentFreeHeap = configTOTAL_HEAP_SIZE - currentSize;
  minimumEverFreeHeap = std::min(currentFreeHeap, minimumEverFreeHeap);

  return ptr;
}
void vPortFree( void *pv ) {
  allocatedMemory.erase(pv);
  return free(pv);
}

size_t xPortGetFreeHeapSize(void) {
  return currentFreeHeap;
}

size_t xPortGetMinimumEverFreeHeapSize(void) {
  return minimumEverFreeHeap;
}
