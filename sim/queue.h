#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

/**
 * Type by which queues are referenced.  For example, a call to xQueueCreate()
 * returns an QueueHandle_t variable that can then be used as a parameter to
 * xQueueSend(), xQueueReceive(), etc.
 */
typedef void* QueueHandle_t;

struct Queue_t {
  std::mutex mutex;
  std::condition_variable cv;
  std::vector<uint8_t> queue;
  size_t max_length;

  Queue_t() = default;

  Queue_t(const Queue_t& o) {
    queue = o.queue;
    max_length = o.max_length;
  }

  Queue_t& operator=(const Queue_t& o) {
    queue = o.queue;
    return *this;
  }
};

// using QueueHandle_t = std::vector<uint8_t>;

QueueHandle_t xQueueCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize);
BaseType_t xQueueSend(QueueHandle_t xQueue, const void* const pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void* const pvItemToQueue, BaseType_t* xHigherPriorityTaskWoken);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void* const pvBuffer, TickType_t xTicksToWait);
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue);
