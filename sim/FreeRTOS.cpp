#include "FreeRTOS.h"
#include <numeric>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>

void NVIC_SystemReset(void) {
}

void APP_ERROR_HANDLER(int err) {
  fprintf(stderr, "APP_ERROR_HANDLER: %d", err);
}

namespace {
  bool heapTrackingAlive = false;

  struct HeapTracking {
    std::unordered_map<void*, size_t> allocatedMemory;
    size_t currentFreeHeap = configTOTAL_HEAP_SIZE;
    size_t minimumEverFreeHeap = configTOTAL_HEAP_SIZE;

    HeapTracking() {
      heapTrackingAlive = true;
    }

    ~HeapTracking() {
      heapTrackingAlive = false;
    }
  };

  HeapTracking heapTracking;
}

void* pvPortMalloc(size_t xWantedSize) {
  void* ptr = malloc(xWantedSize);
  if (!heapTrackingAlive) {
    return ptr;
  }
  heapTracking.allocatedMemory[ptr] = xWantedSize;

  const size_t currentSize = std::accumulate(heapTracking.allocatedMemory.begin(),
                                             heapTracking.allocatedMemory.end(),
                                             0,
                                             [](const size_t lhs, const std::pair<void*, size_t>& item) {
                                               return lhs + item.second;
                                             });

  heapTracking.currentFreeHeap = configTOTAL_HEAP_SIZE - currentSize;
  heapTracking.minimumEverFreeHeap = std::min(heapTracking.currentFreeHeap, heapTracking.minimumEverFreeHeap);

  return ptr;
}

void vPortFree(void* pv) {
  if (heapTrackingAlive) {
    heapTracking.allocatedMemory.erase(pv);
  }
  free(pv);
}

size_t xPortGetHeapSize(void) {
  return configTOTAL_HEAP_SIZE;
}

size_t xPortGetFreeHeapSize(void) {
  return heapTracking.currentFreeHeap;
}

size_t xPortGetMinimumEverFreeHeapSize(void) {
  return heapTracking.minimumEverFreeHeap;
}
