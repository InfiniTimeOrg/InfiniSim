#pragma once

#include "FreeRTOS.h"
#include "queue.h"

typedef QueueHandle_t SemaphoreHandle_t;

QueueHandle_t xSemaphoreCreateMutex();

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);

// Recursive mutex, as used by FS. Handles from xSemaphoreCreateRecursiveMutex
// must only be passed to the *Recursive functions and vice versa (same rule as
// real FreeRTOS).
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex();
BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t xSemaphore);
