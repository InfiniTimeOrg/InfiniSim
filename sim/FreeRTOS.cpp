#include "FreeRTOS.h"
#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>

void NVIC_SystemReset(void) {}

void APP_ERROR_HANDLER(int err) {
  fprintf(stderr, "APP_ERROR_HANDLER: %d", err);
}

namespace {
std::map<void *, size_t> allocatedMemory;
size_t currentFreeHeap = configTOTAL_HEAP_SIZE;
size_t minimumEverFreeHeap = configTOTAL_HEAP_SIZE;
}

void *pvPortMalloc( size_t xWantedSize ) {
  void* ptr = malloc(xWantedSize);
  allocatedMemory[ptr] = xWantedSize;

  size_t currentSize = 0;
  std::for_each(allocatedMemory.begin(), allocatedMemory.end(), [&currentSize](const std::pair<void*, size_t>& item){
    currentSize += item.second;
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
