#include "FreeRTOS.h"
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>

// Defined by the simulator main, mirroring the counter the firmware keeps in
// its vApplicationMallocFailedHook and shows in Sys Info.
extern int mallocFailedCount;

void NVIC_SystemReset(void) {
}

void APP_ERROR_HANDLER(int err) {
  fprintf(stderr, "APP_ERROR_HANDLER: %d", err);
}

namespace {
  bool heapTrackingAlive = false;

  struct HeapTracking {
    std::unordered_map<void*, size_t> allocatedMemory;
    size_t used = 0;
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

  // Bytes withheld from the budget, from INFINISIM_HEAP_BALLAST. The simulator
  // reports far more free heap than a watch does, for two reasons: it carries no
  // BLE stack, and InfiniTime's stdlib.c, which routes malloc/new/littlefs onto
  // this heap on the device, is not part of this build, so those allocations go
  // to the host heap untracked. Rather than model that, withhold the difference
  // you measure against Sys Info on a real watch. Read once.
  size_t HeapBallast() {
    static const size_t ballast = [] {
      const char* env = getenv("INFINISIM_HEAP_BALLAST");
      size_t value = env != nullptr ? strtoul(env, nullptr, 10) : 0;
      if (value >= configTOTAL_HEAP_SIZE) {
        fprintf(stderr, "[heap] INFINISIM_HEAP_BALLAST=%zu leaves nothing of %d, clamping\n", value, configTOTAL_HEAP_SIZE);
        value = configTOTAL_HEAP_SIZE - 1;
      }
      if (value > 0) {
        fprintf(stderr, "[heap] reserving %zu bytes, %zu of %d usable\n", value, configTOTAL_HEAP_SIZE - value, configTOTAL_HEAP_SIZE);
      }
      return value;
    }();
    return ballast;
  }

  void UpdateFree() {
    heapTracking.currentFreeHeap = configTOTAL_HEAP_SIZE - heapTracking.used - HeapBallast();
    heapTracking.minimumEverFreeHeap = std::min(heapTracking.currentFreeHeap, heapTracking.minimumEverFreeHeap);
  }
}

void* pvPortMalloc(size_t xWantedSize) {
  if (heapTrackingAlive && heapTracking.used + HeapBallast() + xWantedSize > configTOTAL_HEAP_SIZE) {
    // What heap_4 does on the watch when it cannot satisfy a request. Returning
    // host memory here instead would hide every allocation failure the firmware
    // is written to cope with. Announce the first one, since whatever the caller
    // does with the null pointer is easier to read with the cause in the log.
    UpdateFree();
    if (mallocFailedCount == 0) {
      fprintf(stderr, "[heap] allocation of %zu bytes failed, %zu free; further failures counted only\n", xWantedSize, heapTracking.currentFreeHeap);
    }
    mallocFailedCount++;
    return nullptr;
  }

  void* ptr = malloc(xWantedSize);
  if (!heapTrackingAlive || ptr == nullptr) {
    return ptr;
  }
  // Drop any stale size still recorded against this address before adding the
  // new one. A running total only stays correct if every insertion is matched,
  // and the accumulate this replaces was self-correcting by construction.
  const auto previous = heapTracking.allocatedMemory.find(ptr);
  if (previous != heapTracking.allocatedMemory.end()) {
    heapTracking.used -= previous->second;
  }
  heapTracking.allocatedMemory[ptr] = xWantedSize;
  heapTracking.used += xWantedSize;
  UpdateFree();

  return ptr;
}

void vPortFree(void* pv) {
  if (heapTrackingAlive) {
    const auto entry = heapTracking.allocatedMemory.find(pv);
    if (entry != heapTracking.allocatedMemory.end()) {
      heapTracking.used -= entry->second;
      heapTracking.allocatedMemory.erase(entry);
      UpdateFree();
    }
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
