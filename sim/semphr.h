#pragma once

#include "FreeRTOS.h"
#include "queue.h"

typedef QueueHandle_t SemaphoreHandle_t;

QueueHandle_t xSemaphoreCreateMutex();

BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore);
